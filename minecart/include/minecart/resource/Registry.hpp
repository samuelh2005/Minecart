#pragma once

#include "ResourceTypes.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <spdlog/spdlog.h>
#include "minecart/Panic.hpp"

namespace minecart::resource {

class IRegistry {
public:
    virtual ~IRegistry() = default;

    virtual const std::string& name() const = 0;
    virtual void load(const ResourceKey& key, const std::filesystem::path& file) = 0;
    virtual bool contains(const ResourceKey& key) const = 0;
};

template <typename T>
class Registry final : public IRegistry {
public:
    using LoaderFn = std::function<std::shared_ptr<T>(const ResourceKey&, const std::filesystem::path&)>;

    Registry(std::string name, LoaderFn loader)
        : name_(std::move(name)), loader_(std::move(loader)) {
        if (name_.empty()) {
            minecart::panic("Registry name must not be empty");
        }
        if (!loader_) {
            minecart::panic("Registry loader must not be empty");
        }
        spdlog::debug("[Registry] created name='{}'", name_);
    }

    const std::string& name() const override { return name_; }

    void load(const ResourceKey& key, const std::filesystem::path& file) override {
        spdlog::debug("[Registry:{}] load key={} file={}", name_, key.toString(), file.string());
        auto loaded = loader_(key, file);
        if (!loaded) {
            spdlog::error("[Registry:{}] loader returned null key={} file={}", name_, key.toString(), file.string());
            minecart::panic("Registry loader returned null for " + key.toString());
        }
        entries_[key.toString()] = std::move(loaded);
    }

    bool contains(const ResourceKey& key) const override {
        const bool found = entries_.find(key.toString()) != entries_.end();
        spdlog::debug("[Registry:{}] contains key={} -> {}", name_, key.toString(), found);
        return found;
    }

    T* get(const ResourceKey& key) {
        auto it = entries_.find(key.toString());
        spdlog::debug("[Registry:{}] get key={} -> {}", name_, key.toString(), it != entries_.end());
        return it == entries_.end() ? nullptr : it->second.get();
    }

    const T* get(const ResourceKey& key) const {
        auto it = entries_.find(key.toString());
        spdlog::debug("[Registry:{}] get (const) key={} -> {}", name_, key.toString(), it != entries_.end());
        return it == entries_.end() ? nullptr : it->second.get();
    }

    std::vector<ResourceKey> keys() const {
        spdlog::debug("[Registry:{}] keys count={}", name_, entries_.size());
        std::vector<ResourceKey> out;
        out.reserve(entries_.size());
        for (const auto& [k, _] : entries_) {
            const auto pos = k.find(':');
            if (pos == std::string::npos) {
                spdlog::warn("[Registry:{}] skipping malformed key='{}'", name_, k);
                continue;
            }
            out.emplace_back(k.substr(0, pos), k.substr(pos + 1));
        }
        return out;
    }

private:
    std::string name_;
    LoaderFn loader_;
    std::unordered_map<std::string, std::shared_ptr<T>> entries_;
};

} // namespace minecart::resource
