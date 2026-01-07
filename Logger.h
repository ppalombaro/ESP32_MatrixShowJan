#pragma once
#include <Arduino.h>

class Logger {
public:
    static Logger& instance() {
        static Logger _instance;
        return _instance;
    }

    void log(const String& message) {
        Serial.println(message);
    }

    template<typename... Args>
    void logf(const char* fmt, Args... args) {
        char buf[256];
        snprintf(buf, sizeof(buf), fmt, args...);
        Serial.println(buf);
    }

private:
    Logger() { Serial.begin(115200); }
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
