#pragma once
#include <Arduino.h>

class NTPClient; // Forward declaration

class Logger {
public:
    static Logger& instance();

    void begin(NTPClient* client = nullptr);
    void log(const String& message);

    

private:
    Logger() = default;
    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger* _instance;
    NTPClient* _ntpClient = nullptr;
};
