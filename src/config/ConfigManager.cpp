#include "ConfigManager.h"
#include "menu/InputControls.h"
#include "globals/globals.h"
#include "fonts/FontManager.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

std::string ConfigManager::configDirectory = "ror2modconfigs";
std::string ConfigManager::defaultConfigName = "default";
std::string ConfigManager::currentConfigName = "";
std::vector<std::string> ConfigManager::availableConfigs;
std::vector<InputControl*> ConfigManager::registeredControls;

void ConfigManager::Initialize() {
    EnsureConfigDirectoryExists();
    RefreshConfigList();

    if (!ConfigExists(defaultConfigName)) {
        CreateDefaultConfig();
    }
    LoadConfig(defaultConfigName);
}

void ConfigManager::CreateDefaultConfig() {
    G::logger.LogInfo("Creating default config...");
    SaveConfig(defaultConfigName);
}

void ConfigManager::EnsureConfigDirectoryExists() {
    if (!std::filesystem::exists(configDirectory)) {
        try {
            std::filesystem::create_directory(configDirectory);
            G::logger.LogInfo("Created config directory: %s", configDirectory.c_str());
        } catch (const std::exception& e) {
            G::logger.LogError("Failed to create config directory: %s", e.what());
        }
    }
}

std::string ConfigManager::GetConfigPath(const std::string& configName) {
    return configDirectory + "/" + configName + ".json";
}

void ConfigManager::RefreshConfigList() {
    availableConfigs.clear();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(configDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                availableConfigs.push_back(filename);
            }
        }
    } catch (const std::exception& e) {
        G::logger.LogError("Error scanning config directory: %s", e.what());
    }

    // Sort configs alphabetically, but keep "default" first
    std::sort(availableConfigs.begin(), availableConfigs.end(),
        [](const std::string& a, const std::string& b) {
            if (a == "default") return true;
            if (b == "default") return false;
            return a < b;
        });
}

bool ConfigManager::SaveConfig(const std::string& configName) {
    try {
        json config;

        for (InputControl* control : registeredControls) {
            if (control) {
                json controlData = control->Serialize();
                if (!controlData.empty()) {
                    config[control->GetId()] = controlData;
                }
            }
        }

        config["fontSettings"] = {
            {"fontIndex", FontManager::CurrentFontIndex},
            {"fontSize", FontManager::ESPFontSize}
        };

        std::ofstream file(GetConfigPath(configName));
        if (!file.is_open()) {
            G::logger.LogError("Failed to open config file for writing: %s", configName.c_str());
            return false;
        }

        file << config.dump(4);
        file.close();

        currentConfigName = configName;
        RefreshConfigList();

        G::logger.LogInfo("Saved config: %s", configName.c_str());
        return true;

    } catch (const std::exception& e) {
        G::logger.LogError("Error saving config: %s", e.what());
        return false;
    }
}

bool ConfigManager::LoadConfig(const std::string& configName) {
    try {
        std::ifstream file(GetConfigPath(configName));
        if (!file.is_open()) {
            G::logger.LogError("Failed to open config file: %s", configName.c_str());
            return false;
        }

        json config;
        file >> config;
        file.close();

        for (InputControl* control : registeredControls) {
            if (control && config.contains(control->GetId())) {
                control->Deserialize(config[control->GetId()]);
            }
        }

        if (config.contains("fontSettings")) {
            const auto& fontSettings = config["fontSettings"];
            if (fontSettings.contains("fontIndex")) {
                FontManager::CurrentFontIndex = fontSettings["fontIndex"];
                if (FontManager::CurrentFontIndex >= FontManager::AvailableFonts.size()) {
                    FontManager::CurrentFontIndex = 0;
                }
            }
            if (fontSettings.contains("fontSize")) {
                FontManager::ESPFontSize = fontSettings["fontSize"];
            }
        }

        currentConfigName = configName;
        G::logger.LogInfo("Loaded config: %s", configName.c_str());
        return true;

    } catch (const std::exception& e) {
        G::logger.LogError("Error loading config: %s", e.what());
        return false;
    }
}

bool ConfigManager::DeleteConfig(const std::string& configName) {
    if (configName == defaultConfigName) {
        G::logger.LogWarning("Cannot delete default config");
        return false;
    }

    try {
        std::filesystem::remove(GetConfigPath(configName));
        RefreshConfigList();

        if (currentConfigName == configName) {
            currentConfigName = "";
        }

        G::logger.LogInfo("Deleted config: %s", configName.c_str());
        return true;

    } catch (const std::exception& e) {
        G::logger.LogError("Error deleting config: %s", e.what());
        return false;
    }
}

void ConfigManager::RegisterControl(InputControl* control) {
    if (control && std::find(registeredControls.begin(), registeredControls.end(), control) == registeredControls.end()) {
        registeredControls.push_back(control);
    }
}

void ConfigManager::UnregisterControl(InputControl* control) {
    registeredControls.erase(
        std::remove(registeredControls.begin(), registeredControls.end(), control),
        registeredControls.end()
    );
}

bool ConfigManager::ConfigExists(const std::string& configName) {
    return std::filesystem::exists(GetConfigPath(configName));
}