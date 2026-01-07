#pragma once

#include <Arduino.h>
#include <NTPClient.h>

class Logger {
public:
    static Logger& instance();
    void begin(NTPClient* client);
    void log(const String& message);

private:
    Logger() = default;
    ~Logger() = default;

    static Logger* _instance;
    NTPClient* _ntpClient = nullptr;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
