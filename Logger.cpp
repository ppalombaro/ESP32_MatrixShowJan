/* Logger.cpp
   Centralized logging and heartbeat implementation
   VERSION: V15.0.1-2026-01-03T10:05:00Z - Extracted from main controller
*/

#include "Logger.h"
#include <Arduino.h>
#include <cstdarg>

Logger* Logger::_instance = nullptr;

Logger& Logger::instance() {
    if (!_instance) _instance = new Logger();
    return *_instance;
}

Logger::Logger() {}

void Logger::begin(NTPClient* client) {
    timeClient = client;
}

void Logger::log(const String& msg) {
    Serial.println(msg);
}

void Logger::logf(const char* fmt, ...) {
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Serial.println(buf);
}
