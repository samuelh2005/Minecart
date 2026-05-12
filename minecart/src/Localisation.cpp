#include "minecart/Localisation.hpp"

namespace minecart {

void Localisation::init(const resource::ResourceManager& resourceManager) {
    m_resourceManager = &resourceManager;
}

void Localisation::SetLanguage(const std::string& language) {
    m_currentLanguage = language;
}

std::string Localisation::GetTranslation(const std::string& key) const {
    if (!m_resourceManager) {
        spdlog::error("[Localisation] Resource manager not initialized");
        return key;
    }

    auto registry = m_resourceManager->getRegistry<TranslationDefinition>("lang");
    if (!registry) {
        spdlog::error("[Localisation] Translation registry not found");
        return key; // Fallback to the key itself
    }

    // 1. we need to fetch the current language from the registry, then we can fetch the translation for the key

    // get the namespace and path from key
    const auto pos = key.find(':');
    const auto keyNamespace = pos != std::string::npos ? key.substr(0, pos) : "";
    const auto keyPath = pos != std::string::npos ? key.substr(pos + 1) : key;

    minecart::resource::ResourceKey languageKey{keyNamespace, "lang/" + m_currentLanguage};
    const auto* translationDef = registry->get(languageKey);

    minecart::resource::ResourceKey fallbackKey{keyNamespace, "lang/en_us"};
    auto fallbackTranslationDef = registry->get(fallbackKey);

    // 2. Query the translation definition for the key, if not found, fallback to the default language (en_us)
    if (!translationDef) {
        if (!fallbackTranslationDef) {
            return key; // Fallback to the key itself
        }
        auto fallbackTranslation = fallbackTranslationDef->get(key);
        return fallbackTranslation ? *fallbackTranslation : key;
    }

    auto translation = translationDef->get(key);
    return translation ? *translation : key;
}

void TranslationDefinition::loadFromJson(const resource::ResourceKey& key,
        const json& json,
        const std::filesystem::path& sourceFile) {
    key_ = key;
    sourceFile_ = sourceFile;

    translations_.clear();

    name_ = json.at("name").get<std::string>();

    auto entries = json.at("entries");

    if (!entries.is_object()) {
        minecart::panic("TranslationDefinition JSON must contain an 'entries' object.");
    }

    for (const auto& [translationKey, value] : entries.items()) {
        if (!value.is_string()) {
            minecart::panic("Translation value for key '" + translationKey + "' must be a string.");
        }

        translations_[translationKey] = value.get<std::string>();
    }
}

std::optional<std::string> TranslationDefinition::get(const std::string& translationKey) const {
    auto it = translations_.find(translationKey);

    if (it == translations_.end()) {
        return std::nullopt;
    }

    return it->second;
}
} // namespace minecart