#pragma once

#include <Arduino.h>
#include <NTPClient.h>   // needed for Logger::begin
#include <WiFiUdp.h>

class Logger {
public:
    // Singleton access
    static Logger& instance();

    // Initialize logger with optional NTP client
    void begin(NTPClient* client);

    // Logging functions (example: preserve existing methods)
    void log(const String& message);

private:
    Logger() = default;
    ~Logger() = default;

    static Logger* _instance;   // fixes 'not a static data member' error

    NTPClient* _ntpClient = nullptr;  // optional, stores client reference

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
