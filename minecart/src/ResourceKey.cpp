#include "minecart/resource/ResourceKey.hpp"

namespace minecart::resource {

ResourceKey::ResourceKey(std::string ns_, std::string path_)
    : ns(std::move(ns_)), path(std::move(path_)) {}

ResourceKey ResourceKey::parse(const std::string& full) {
    auto pos = full.find(':');
    if (pos == std::string::npos)
        throw std::runtime_error("Invalid ResourceKey: " + full);

    return {
        full.substr(0, pos),
        full.substr(pos + 1)
    };
}

std::string ResourceKey::toString() const {
    return ns + ":" + path;
}

bool ResourceKey::operator==(const ResourceKey& other) const {
    return ns == other.ns && path == other.path;
}

size_t std::hash<ResourceKey>::operator()(const ResourceKey& k) const noexcept {
    size_t h1 = std::hash<std::string>{}(k.ns);
    size_t h2 = std::hash<std::string>{}(k.path);
    return h1 ^ (h2 << 1);
}

} // namespace minecart::resource
