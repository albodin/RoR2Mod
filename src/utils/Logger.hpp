#pragma once
#include <cstdio>
#include <plog/Log.h>
#include <string>

class Logger {
  public:
    Logger();
};

// Undefine plog's macros if they exist
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif
#ifdef LOG_WARNING
#undef LOG_WARNING
#endif
#ifdef LOG_INFO
#undef LOG_INFO
#endif
#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif

#define LOG_ERROR(fmt, ...)                                                                                                                                    \
    do {                                                                                                                                                       \
        char buffer[4096];                                                                                                                                     \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__);                                                                                                  \
        PLOG(plog::error) << buffer;                                                                                                                           \
    } while (0)

#define LOG_WARNING(fmt, ...)                                                                                                                                  \
    do {                                                                                                                                                       \
        char buffer[4096];                                                                                                                                     \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__);                                                                                                  \
        PLOG(plog::warning) << buffer;                                                                                                                         \
    } while (0)

#define LOG_INFO(fmt, ...)                                                                                                                                     \
    do {                                                                                                                                                       \
        char buffer[4096];                                                                                                                                     \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__);                                                                                                  \
        PLOG(plog::info) << buffer;                                                                                                                            \
    } while (0)

#define LOG_DEBUG(fmt, ...)                                                                                                                                    \
    do {                                                                                                                                                       \
        char buffer[4096];                                                                                                                                     \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__);                                                                                                  \
        PLOG(plog::debug) << buffer;                                                                                                                           \
    } while (0)
