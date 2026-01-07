#include "Logger.h"

Logger* Logger::_instance = nullptr;  // define static member

Logger& Logger::instance() {
    if (!_instance) _instance) {
        _instance = new Logger();
    }
    return *_instance;
}

void Logger::begin(NTPClient* client) {
    _ntpClient = client;
    // existing initialization logic
}

void Logger::log(const String& message) {
    // example logging logic
    Serial.println(message);
    // optionally log with timestamp from _ntpClient if set
    if (_ntpClient) {
        Serial.print("[");
        Serial.print(_ntpClient->getFormattedTime());
        Serial.print("] ");
        Serial.println(message);
    }
}
