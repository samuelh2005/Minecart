#include "minecart/resource/ResourceManager.hpp"
#include <spdlog/spdlog.h>

namespace minecart::resource {

bool ResourceManager::hasRegistry(std::string_view name) const {
    const bool found = registries_.find(std::string(name)) != registries_.end();
    spdlog::debug("[ResourceManager] hasRegistry name='{}' -> {}", name, found);
    return found;
}

bool ResourceManager::dispatch(std::string_view registryName,
                               const ResourceKey& key,
                               const std::filesystem::path& file) {
    auto it = registries_.find(std::string(registryName));
    if (it == registries_.end()) {
        spdlog::warn("[ResourceManager] dispatch missing registry='{}' key={} file={}",
                    registryName, key.toString(), file.string());
        return false;
    }
    spdlog::debug("[ResourceManager] dispatch registry='{}' key={} file={}",
                 registryName, key.toString(), file.string());
    it->second->load(key, file);
    return true;
}

} // namespace minecart::resource
