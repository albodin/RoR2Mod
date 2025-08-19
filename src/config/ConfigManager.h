#pragma once
#include "utils/json.hpp"
#include <memory>
#include <string>
#include <vector>

using json = nlohmann::json;

class InputControl;

class ConfigManager {
  private:
    static std::string configDirectory;
    static std::string defaultConfigName;
    static std::string currentConfigName;
    static std::vector<std::string> availableConfigs;
    static std::vector<InputControl*> registeredControls;

    static void EnsureConfigDirectoryExists();
    static std::string GetConfigPath(const std::string& configName);

  public:
    static void Initialize();
    static void CreateDefaultConfig();
    static void RefreshConfigList();
    static bool SaveConfig(const std::string& configName);
    static bool LoadConfig(const std::string& configName);
    static bool DeleteConfig(const std::string& configName);
    static void RegisterControl(InputControl* control);
    static void UnregisterControl(InputControl* control);

    static const std::vector<std::string>& GetAvailableConfigs() { return availableConfigs; }
    static const std::string& GetCurrentConfigName() { return currentConfigName; }
    static bool ConfigExists(const std::string& configName);
};
