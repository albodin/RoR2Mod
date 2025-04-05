#pragma once
#include <fstream>
#include <mutex>
#include <string>

class Logger {
private:
    std::ofstream logFile;
    std::mutex logMutex;
    
public:
    Logger(const std::string& path);
    ~Logger();
    
    void Log(const std::string& message);
    void Log(const char* format, ...);
    void LogError(const std::string& message);
    void LogWarning(const std::string& message);
    void LogInfo(const std::string& message);
    void LogError(const char* format, ...);
    void LogWarning(const char* format, ...);
    void LogInfo(const char* format, ...);
};