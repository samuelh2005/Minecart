#pragma once

#include <string>
#include <stdexcept>
#include <functional>

namespace minecart::resource {

struct ResourceKey {
    std::string ns;
    std::string path;

    ResourceKey() = default;

    ResourceKey(std::string ns_, std::string path_);

    static ResourceKey parse(const std::string& full);

    std::string toString() const;

    bool operator==(const ResourceKey& other) const;
};


} // namespace minecart::resource

namespace std {
    template <>
    struct hash<minecart::resource::ResourceKey> {
        size_t operator()(const minecart::resource::ResourceKey& k) const noexcept;
    };
}