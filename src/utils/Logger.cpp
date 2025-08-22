#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

static const std::string logsDir = "ror2mod/logs/";
static const std::string logPath = "ror2mod/ror2mod.log";

Logger::Logger() {
    bool logsDirFailed = false;
    std::string logsDirError;

    // Ensure logs directory exists
    if (!std::filesystem::exists(logsDir) || !std::filesystem::is_directory(logsDir)) {
        try {
            if (!std::filesystem::create_directories(logsDir)) {
                logsDirFailed = true;
                logsDirError = "Failed to create logs directory";
            }
        } catch (const std::exception& e) {
            logsDirFailed = true;
            logsDirError = e.what();
        }
    }

    // Rotate existing log to logs subdirectory
    if (std::filesystem::exists(logPath)) {
        try {
            auto lastWriteTime = std::filesystem::last_write_time(logPath);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastWriteTime - std::filesystem::file_time_type::clock::now() +
                                                                                          std::chrono::system_clock::now());
            auto time_t_value = std::chrono::system_clock::to_time_t(sctp);

            std::stringstream ss;
            ss << logsDir << "/ror2mod_" << std::put_time(std::localtime(&time_t_value), "%Y-%m-%dT%H:%M:%S") << ".log";
            std::string rotatedName = ss.str();

            std::filesystem::rename(logPath, rotatedName);
        } catch (const std::exception&) {
            // If rotation fails, just truncate the existing file
        }
    }

    logFile.open(logPath, std::ios::trunc);

    if (logsDirFailed) {
        this->LogError("Failed to create logs directory '%s': %s", logsDir.c_str(), logsDirError.c_str());
        this->LogWarning("Log rotation disabled - old logs will not be archived");
    }
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

void Logger::LogError(const std::string& message) { Log("[ERROR] " + message); }

void Logger::LogError(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Log("[ERROR] " + std::string(buffer));
}

void Logger::LogWarning(const std::string& message) { Log("[WARNING] " + message); }

void Logger::LogWarning(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Log("[WARNING] " + std::string(buffer));
}

void Logger::LogInfo(const std::string& message) { Log("[INFO] " + message); }

void Logger::LogInfo(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Log("[INFO] " + std::string(buffer));
}
