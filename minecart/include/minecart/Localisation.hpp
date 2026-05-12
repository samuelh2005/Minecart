#pragma once

#include "minecart/resource/JsonResource.hpp"
#include "minecart/resource/ResourceTypes.hpp"

#include <optional>
#include <unordered_map>

#include "minecart/resource/ResourceManager.hpp"

namespace minecart {

using json = nlohmann::json;

class TranslationDefinition final : public resource::JsonResource {
public:
    TranslationDefinition() = default;

    void loadFromJson(const resource::ResourceKey& key,
            const nlohmann::json& json,
            const std::filesystem::path& sourceFile) override;

    [[nodiscard]] const resource::ResourceKey& key() const { return key_; }
    [[nodiscard]] const std::filesystem::path& sourceFile() const { return sourceFile_; }

    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] const std::unordered_map<std::string, std::string>& translations() const {
        return translations_;
    }
    [[nodiscard]] std::optional<std::string> get(const std::string& translationKey) const;

private:
    resource::ResourceKey key_;
    std::filesystem::path sourceFile_;

    std::string name_;
    std::unordered_map<std::string, std::string> translations_;
};

class Localisation {
public:
    void init(const resource::ResourceManager& resourceManager);
    void SetLanguage(const std::string& language);
    std::string GetTranslation(const std::string& key) const;

private:
    const resource::ResourceManager* m_resourceManager = nullptr;
    std::string m_currentLanguage;
};

} // namespace minecart