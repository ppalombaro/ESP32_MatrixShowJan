#include "Logger.h"
#include <Arduino.h>

Logger* Logger::_instance = nullptr;

Logger& Logger::instance() {
    if (!_instance) {
        _instance = new Logger();
    }
    return *_instance;
}

void Logger::begin(NTPClient* client) {
    _ntpClient = client;
    log("Logger initialized");
}

void Logger::log(const String& message) {
    Serial.println(message);
}
