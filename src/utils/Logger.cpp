#include "Logger.h"
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <iomanip>
#include <sstream> 

Logger::Logger(const std::string& path) {
    if (std::filesystem::exists(path)) {
        try {
            auto lastWriteTime = std::filesystem::last_write_time(path);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                lastWriteTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            auto time_t_value = std::chrono::system_clock::to_time_t(sctp);

            std::stringstream ss;
            ss << path << "." << std::put_time(std::localtime(&time_t_value), "%Y-%m-%dT%H:%M:%S");
            std::string rotatedName = ss.str();

            std::filesystem::rename(path, rotatedName);
        } catch (const std::exception&) {
            // If rotation fails, just truncate the existing file
        }
    }

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
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now), "%Y-%m-%dT%H:%M:%S");
        logFile << "[" << ss.str() << "] " << message << std::endl;
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