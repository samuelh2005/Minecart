#pragma once

#include <nlohmann/json.hpp>
#include "ResourceTypes.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace minecart::resource {

using json = nlohmann::json;

class JsonResource {
public:
    virtual ~JsonResource() = default;

    virtual void loadFromJson(const ResourceKey& key,
                              const json& json,
                              const std::filesystem::path& sourceFile) = 0;
};

json readJsonFile(const std::filesystem::path& file);

template <typename T>
std::shared_ptr<T> loadJsonResource(const ResourceKey& key,
                                    const std::filesystem::path& file) {
    static_assert(std::is_base_of_v<JsonResource, T>,
                  "T must derive from minecart::resource::JsonResource");

    auto json = readJsonFile(file);
    auto obj = std::make_shared<T>();
    obj->loadFromJson(key, json, file);
    return obj;
}

} // namespace minecart::resource
