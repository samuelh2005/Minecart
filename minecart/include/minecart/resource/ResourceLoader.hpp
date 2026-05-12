#pragma once

#include "ResourceManager.hpp"
#include "ResourceTypes.hpp"

#include <filesystem>
#include <optional>
#include <vector>

namespace minecart::resource {

class ResourceLoader {
public:
    explicit ResourceLoader(ResourceManager& manager);

    void loadPack(const std::filesystem::path& packRoot);

    static bool isDatapackRoot(const std::filesystem::path& packRoot);
    static std::vector<ParsedDatapackPath> scan(const std::filesystem::path& packRoot);
    static std::vector<ParsedDatapackPath> scanDataDirectory(const std::filesystem::path& dataRoot);
    static std::optional<ParsedDatapackPath> parseFile(const std::filesystem::path& packRoot,
                                                       const std::filesystem::path& file);

private:
    ResourceManager& manager_;
};

} // namespace minecart::resource
