#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <functional>
#include <ostream>

namespace minecart::resource {

struct ResourceKey {
    std::string namespace_;
    std::string path;

    ResourceKey() = default;
    ResourceKey(std::string ns, std::string p)
        : namespace_(std::move(ns)), path(std::move(p)) {}

    [[nodiscard]] std::string toString() const {
        return namespace_ + ":" + path;
    }

    [[nodiscard]] bool empty() const {
        return namespace_.empty() || path.empty();
    }
};

inline bool operator==(const ResourceKey& a, const ResourceKey& b) {
    return a.namespace_ == b.namespace_ && a.path == b.path;
}

inline bool operator!=(const ResourceKey& a, const ResourceKey& b) {
    return !(a == b);
}

inline std::ostream& operator<<(std::ostream& os, const ResourceKey& key) {
    return os << key.toString();
}

struct ParsedDatapackPath {
    std::string registry;
    ResourceKey key;
    std::filesystem::path file;
};

} // namespace minecart::resource

namespace std {
template<>
struct hash<minecart::resource::ResourceKey> {
    std::size_t operator()(const minecart::resource::ResourceKey& key) const noexcept {
        std::size_t h1 = std::hash<std::string>{}(key.namespace_);
        std::size_t h2 = std::hash<std::string>{}(key.path);
        return h1 ^ (h2 << 1);
    }
};
}
