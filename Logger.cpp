#include "Logger.h"

Logger* Logger::_instance = nullptr;

Logger& Logger::instance() {
    if (!_instance) _instance = new Logger();
    return *_instance;
}

void Logger::begin(NTPClient* client) {
    _ntpClient = client;
}

void Logger::log(const String& message) {
    Serial.println(message);
}
