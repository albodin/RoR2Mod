#include "ConfigManager.h"
#include "fonts/FontManager.h"
#include "globals/globals.h"
#include "menu/InputControls.h"
#include "menu/NotificationManager.h"
#include <algorithm>
#include <filesystem>
#include <fstream>

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
    LOG_INFO("Creating default config...");
    SaveConfig(defaultConfigName);
}

void ConfigManager::EnsureConfigDirectoryExists() {
    if (!std::filesystem::exists(configDirectory)) {
        try {
            std::filesystem::create_directories(configDirectory);
            LOG_INFO("Created config directory: %s", configDirectory.c_str());
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to create config directory: %s", e.what());
        }
    }
}

std::string ConfigManager::GetConfigPath(const std::string& configName) { return configDirectory + "/" + configName + ".json"; }

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
        LOG_ERROR("Error scanning config directory: %s", e.what());
    }

    // Sort configs alphabetically, but keep "default" first
    std::sort(availableConfigs.begin(), availableConfigs.end(), [](const std::string& a, const std::string& b) {
        if (a == "default")
            return true;
        if (b == "default")
            return false;
        return a < b;
    });
}

bool ConfigManager::SaveConfig(const std::string& configName) {
    try {
        json config;

        for (InputControl* control : registeredControls) {
            if (control) {
                try {
                    json controlData = control->Serialize();
                    if (!controlData.empty()) {
                        config[control->GetId()] = controlData;
                    }
                } catch (const json::exception& e) {
                    LOG_ERROR("Error serializing control '%s': %s", control->GetId().c_str(), e.what());
                }
            }
        }

        config["fontSettings"] = {
            {"fontIndex", FontManager::CurrentFontIndex},
            {"fontSize", FontManager::ESPFontSize},
        };

        config["notificationSettings"] = NotificationManager::Serialize();

        std::ofstream file(GetConfigPath(configName));
        if (!file.is_open()) {
            LOG_ERROR("Failed to open config file for writing: %s", configName.c_str());
            return false;
        }

        file << config.dump(4);
        file.close();

        currentConfigName = configName;
        RefreshConfigList();

        LOG_INFO("Saved config: %s", configName.c_str());
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("Error saving config: %s", e.what());
        return false;
    }
}

bool ConfigManager::LoadConfig(const std::string& configName) {
    try {
        std::ifstream file(GetConfigPath(configName));
        if (!file.is_open()) {
            LOG_ERROR("Failed to open config file: %s", configName.c_str());
            return false;
        }

        json config;
        file >> config;
        file.close();

        int errorCount = 0;
        for (InputControl* control : registeredControls) {
            if (control && config.contains(control->GetId())) {
                try {
                    control->Deserialize(config[control->GetId()]);
                } catch (const json::exception& e) {
                    errorCount++;
                    LOG_ERROR("Failed to load settings for '%s'", control->GetId().c_str());
                    LOG_ERROR("Error details: %s", e.what());
                    LOG_ERROR("Problematic data: %s", config[control->GetId()].dump().c_str());
                }
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

        if (config.contains("notificationSettings")) {
            try {
                NotificationManager::Deserialize(config["notificationSettings"]);
            } catch (const json::exception& e) {
                LOG_ERROR("Failed to load notification settings: %s", e.what());
            }
        }

        currentConfigName = configName;

        if (errorCount > 0) {
            LOG_WARNING("Loaded config '%s' with %d errors. Settings with errors will use default values.", configName.c_str(), errorCount);
            LOG_INFO("The config will be automatically updated to the new format when saved.");
        } else {
            LOG_INFO("Successfully loaded config: %s", configName.c_str());
        }

        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("Error loading config: %s", e.what());
        return false;
    }
}

bool ConfigManager::DeleteConfig(const std::string& configName) {
    if (configName == defaultConfigName) {
        LOG_WARNING("Cannot delete default config");
        return false;
    }

    try {
        std::filesystem::remove(GetConfigPath(configName));
        RefreshConfigList();

        if (currentConfigName == configName) {
            currentConfigName = "";
        }

        LOG_INFO("Deleted config: %s", configName.c_str());
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("Error deleting config: %s", e.what());
        return false;
    }
}

void ConfigManager::RegisterControl(InputControl* control) {
    if (control && std::find(registeredControls.begin(), registeredControls.end(), control) == registeredControls.end()) {
        registeredControls.push_back(control);
    }
}

void ConfigManager::UnregisterControl(InputControl* control) {
    registeredControls.erase(std::remove(registeredControls.begin(), registeredControls.end(), control), registeredControls.end());
}

bool ConfigManager::ConfigExists(const std::string& configName) { return std::filesystem::exists(GetConfigPath(configName)); }
