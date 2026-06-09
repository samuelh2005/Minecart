#pragma once

#include <unordered_map>
#include "ResourceKey.hpp"

namespace minecart::resource {

template <typename T>
class Registry {
public:
    bool registerItem(const ResourceKey& key, T value) {
        return data.emplace(key, std::move(value)).second;
    }

    const T* get(const ResourceKey& key) const {
        auto it = data.find(key);
        if (it == data.end()) return nullptr;
        return &it->second;
    }

    bool contains(const ResourceKey& key) const {
        return data.contains(key);
    }

private:
    std::unordered_map<ResourceKey, T> data;
};

} // namespace minecart::resource
