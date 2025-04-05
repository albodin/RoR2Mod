#include "Logger.h"
#include <ctime>
#include <cstdarg>
#include <cstdio> 

Logger::Logger(const std::string& path) {
    logFile.open(path, std::ios::trunc);
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::Log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        time_t now = time(0);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now);
        std::string timestamp(timeStr);
        if (!timestamp.empty() && timestamp[timestamp.size()-1] == '\n') {
            timestamp.pop_back();
        }
        logFile << "[" << timestamp << "] " << message << std::endl;
        logFile.flush();
    }
}

void Logger::Log(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Log(std::string(buffer));
}

void Logger::LogError(const std::string& message) {
    Log("[ERROR] " + message);
}

void Logger::LogError(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Log("[ERROR] " + std::string(buffer));
}

void Logger::LogWarning(const std::string& message) {
    Log("[WARNING] " + message);
}

void Logger::LogWarning(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Log("[WARNING] " + std::string(buffer));
}

void Logger::LogInfo(const std::string& message) {
    Log("[INFO] " + message);
}

void Logger::LogInfo(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Log("[INFO] " + std::string(buffer));
}