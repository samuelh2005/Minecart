#include "minecart/resource/FileSystemResourceLoader.hpp"
#include "minecart/Panic.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace minecart::resource {

namespace {

fs::path stripOneExtension(fs::path p) {
    p.replace_extension();
    return p;
}

std::string joinPathComponents(const fs::path& p) {
    std::string out;
    for (const auto& part : p) {
        const std::string s = part.string();
        if (s.empty() || s == ".") {
            continue;
        }
        if (!out.empty()) {
            out.push_back('/');
        }
        out += s;
    }
    return out;
}

} // namespace

FileSystemResourceLoader::FileSystemResourceLoader(ResourceManager& manager)
    : manager_(manager) {}

bool FileSystemResourceLoader::isDatapackRoot(const fs::path& packRoot) {
    const bool exists = fs::exists(packRoot / "data") && fs::is_directory(packRoot / "data");
    spdlog::debug("[FileSystemResourceLoader] isDatapackRoot root='{}' -> {}", packRoot.string(), exists);
    return exists;
}

std::optional<ParsedDatapackPath> FileSystemResourceLoader::parseFile(const fs::path& packRoot,
                                                            const fs::path& file) {
    const fs::path dataRoot = packRoot / "data";
    if (!fs::exists(dataRoot) || !fs::is_directory(dataRoot)) {
        spdlog::debug("[FileSystemResourceLoader] parseFile skip file='{}' reason=no data dir", file.string());
        return std::nullopt;
    }

    std::error_code ec;
    fs::path rel = fs::relative(file, dataRoot, ec);
    if (ec || rel.empty()) {
        spdlog::debug("[FileSystemResourceLoader] parseFile skip file='{}' reason=not relative", file.string());
        return std::nullopt;
    }

    std::vector<fs::path> parts;
    for (const auto& part : rel) {
        const std::string s = part.string();
        if (!s.empty() && s != ".") {
            parts.emplace_back(s);
        }
    }

    if (parts.size() < 3) {
        spdlog::debug("[FileSystemResourceLoader] parseFile skip file='{}' reason=too few parts", file.string());
        return std::nullopt;
    }

    ParsedDatapackPath out;
    out.key.namespace_ = parts[0].string();
    out.registry = parts[1].string();

    fs::path resourcePath;
    for (std::size_t i = 2; i < parts.size(); ++i) {
        resourcePath /= parts[i];
    }
    const fs::path keyedPath = fs::path(out.registry) / stripOneExtension(resourcePath);
    out.key.path = joinPathComponents(keyedPath);
    out.file = file;

    if (out.key.namespace_.empty() || out.registry.empty() || out.key.path.empty()) {
        spdlog::debug("[FileSystemResourceLoader] parseFile skip file='{}' reason=empty key", file.string());
        return std::nullopt;
    }

    spdlog::debug("[FileSystemResourceLoader] parseFile ok file='{}' registry='{}' key={}",
                 file.string(), out.registry, out.key.toString());
    return out;
}

std::vector<ParsedDatapackPath> FileSystemResourceLoader::scanDataDirectory(const fs::path& dataRoot) {
    std::vector<ParsedDatapackPath> discovered;
    if (!fs::exists(dataRoot) || !fs::is_directory(dataRoot)) {
        spdlog::error("[FileSystemResourceLoader] scanDataDirectory invalid pack: missing data dir='{}'", dataRoot.string());
        minecart::panic("Resource pack missing data directory: " + dataRoot.string());
    }

    const fs::path packRoot = dataRoot.parent_path();
    std::size_t namespace_dirs = 0;
    for (const auto& nsEntry : fs::directory_iterator(dataRoot)) {
        if (!nsEntry.is_directory()) {
            spdlog::debug("[FileSystemResourceLoader] scanDataDirectory skip path='{}' reason=not namespace dir",
                          nsEntry.path().string());
            continue;
        }
        ++namespace_dirs;

        for (const auto& regEntry : fs::directory_iterator(nsEntry.path())) {
            if (!regEntry.is_directory()) {
                spdlog::debug("[FileSystemResourceLoader] scanDataDirectory skip path='{}' reason=not registry dir",
                              regEntry.path().string());
                continue;
            }

            for (const auto& entry : fs::recursive_directory_iterator(regEntry.path())) {
                if (!entry.is_regular_file()) {
                    spdlog::debug("[FileSystemResourceLoader] scanDataDirectory skip path='{}' reason=not file",
                                  entry.path().string());
                    continue;
                }
                spdlog::info("[FileSystemResourceLoader] scanDataDirectory attempt file='{}'", entry.path().string());
                auto parsed = parseFile(packRoot, entry.path());
                if (!parsed) {
                    spdlog::error("[FileSystemResourceLoader] invalid resource path='{}'", entry.path().string());
                    minecart::panic("Invalid resource path: " + entry.path().string());
                }
                discovered.push_back(std::move(*parsed));
            }
        }
    }
    spdlog::debug("[FileSystemResourceLoader] scanDataDirectory done dir='{}' count={}",
                 dataRoot.string(), discovered.size());

    if (namespace_dirs == 0) {
        spdlog::error("[FileSystemResourceLoader] invalid pack: no namespace directories found in '{}'", dataRoot.string());
        minecart::panic("Resource pack missing namespace directories under: " + dataRoot.string());
    }
    return discovered;
}

std::vector<ParsedDatapackPath> FileSystemResourceLoader::scan(const fs::path& packRoot) {
    return scanDataDirectory(packRoot / "data");
}

void FileSystemResourceLoader::loadPack(const fs::path& packRoot) {
    const fs::path dataRoot = packRoot / "data";
    spdlog::info("[FileSystemResourceLoader] loadPack root='{}'", packRoot.string());

    if (!fs::exists(dataRoot) || !fs::is_directory(dataRoot)) {
        spdlog::error("[FileSystemResourceLoader] loadPack invalid pack: missing data dir='{}'", dataRoot.string());
        minecart::panic("Resource pack missing data directory: " + dataRoot.string());
    }

    std::size_t namespace_dirs = 0;
    for (const auto& nsEntry : fs::directory_iterator(dataRoot)) {
        if (!nsEntry.is_directory()) {
            spdlog::error("[FileSystemResourceLoader] invalid pack: non-namespace entry='{}'", nsEntry.path().string());
            minecart::panic("Invalid entry in data directory: " + nsEntry.path().string());
        }

        ++namespace_dirs;
        bool hasRegistryDir = false;
        bool hasLooseFile = false;

        for (const auto& regEntry : fs::directory_iterator(nsEntry.path())) {
            if (regEntry.is_regular_file()) {
                hasLooseFile = true;
                continue;
            }

            if (!regEntry.is_directory()) {
                continue;
            }

            hasRegistryDir = true;
            const std::string registryName = regEntry.path().filename().string();
            if (!manager_.hasRegistry(registryName)) {
                spdlog::error("[FileSystemResourceLoader] invalid registry dir='{}'", regEntry.path().string());
                minecart::panic("Invalid registry directory: " + regEntry.path().string());
            }
        }

        if (!hasRegistryDir) {
            spdlog::error("[FileSystemResourceLoader] invalid pack: namespace '{}' has no registry dirs",
                          nsEntry.path().string());
            minecart::panic("Namespace missing registry directories: " + nsEntry.path().string());
        }

        if (hasLooseFile) {
            spdlog::error("[FileSystemResourceLoader] invalid pack: namespace '{}' has files directly under it",
                          nsEntry.path().string());
            minecart::panic("Namespace has loose files: " + nsEntry.path().string());
        }
    }
    if (namespace_dirs == 0) {
        spdlog::error("[FileSystemResourceLoader] invalid pack: no namespace directories found in '{}'", dataRoot.string());
        minecart::panic("Resource pack missing namespace directories under: " + dataRoot.string());
    }

    auto discovered = scanDataDirectory(dataRoot);
    for (const auto& entry : discovered) {
        spdlog::info("[FileSystemResourceLoader] dispatch registry='{}' key={} file={}",
                     entry.registry, entry.key.toString(), entry.file.string());
        const bool dispatched = manager_.dispatch(entry.registry, entry.key, entry.file);
        if (!dispatched) {
            spdlog::warn("[FileSystemResourceLoader] no registry registry='{}' file='{}'",
                        entry.registry, entry.file.string());
        }
    }
    spdlog::info("[FileSystemResourceLoader] loadPack complete root='{}'", packRoot.string());
}

} // namespace minecart::resource
