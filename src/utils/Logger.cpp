#include "Logger.h"
#include <filesystem>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Initializers/RollingFileInitializer.h>

static const std::string logsDir = "ror2mod/logs/";
static const std::string logPath = "ror2mod/logs/ror2mod.log";
static const int MAX_BACKUP_FILES = 10;

static void rotateExistingLogs() {
    namespace fs = std::filesystem;

    if (!fs::exists(logPath)) {
        return;
    }

    std::string baseName = logsDir + "ror2mod";
    std::string extension = ".log";

    std::string oldestLog = baseName + "." + std::to_string(MAX_BACKUP_FILES) + extension;
    if (fs::exists(oldestLog)) {
        fs::remove(oldestLog);
    }

    // Rotate all existing backups
    for (int i = MAX_BACKUP_FILES - 1; i >= 1; i--) {
        std::string currentFile = baseName + "." + std::to_string(i) + extension;

        if (fs::exists(currentFile)) {
            std::string nextFile = baseName + "." + std::to_string(i + 1) + extension;
            fs::rename(currentFile, nextFile);
        }
    }

    // Move current log to .1
    fs::rename(logPath, baseName + ".1" + extension);
}

Logger::Logger() {
    if (plog::get() != nullptr) {
        return;
    }

    try {
        std::filesystem::create_directories(logsDir);

        rotateExistingLogs();

        static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logPath.c_str(), 0, 0);
        plog::init(plog::verbose, &fileAppender);
    } catch (...) {
        // Logging system failed to initialize, but we can continue
    }
}
