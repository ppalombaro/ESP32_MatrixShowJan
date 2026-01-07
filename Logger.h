#pragma once
#include <Arduino.h>

#define LOGGER_VERSION "15.4FIX"

// Singleton Logger
class Logger {
public:
    static Logger& instance();

    // printf-style logging
    void logf(const char* fmt, ...);

    // simple string logging
    void log(const String& message);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
