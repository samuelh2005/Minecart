#pragma once

#include "Registry.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include "minecart/Panic.hpp"

namespace minecart::resource {

class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    template <typename T>
    void addRegistry(std::unique_ptr<Registry<T>> registry) {
        if (!registry) {
            minecart::panic("Cannot add a null registry");
        }
        const std::string key = registry->name();
        registries_[key] = std::move(registry);
        spdlog::info("[ResourceManager] added registry='{}'", key);
    }

    bool hasRegistry(std::string_view name) const;

    bool dispatch(std::string_view registryName,
                  const ResourceKey& key,
                  const std::filesystem::path& file);

    template <typename T>
    Registry<T>* getRegistry(std::string_view name) {
        auto it = registries_.find(std::string(name));
        if (it == registries_.end()) {
            spdlog::debug("[ResourceManager] getRegistry name='{}' -> not found", name);
            return nullptr;
        }
        spdlog::debug("[ResourceManager] getRegistry name='{}' -> found", name);
        return dynamic_cast<Registry<T>*>(it->second.get());
    }

    template <typename T>
    const Registry<T>* getRegistry(std::string_view name) const {
        auto it = registries_.find(std::string(name));
        if (it == registries_.end()) {
            spdlog::debug("[ResourceManager] getRegistry (const) name='{}' -> not found", name);
            return nullptr;
        }
        spdlog::debug("[ResourceManager] getRegistry (const) name='{}' -> found", name);
        return dynamic_cast<const Registry<T>*>(it->second.get());
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IRegistry>> registries_;
};

} // namespace minecart::resource
