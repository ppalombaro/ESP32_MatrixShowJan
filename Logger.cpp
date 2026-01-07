/* Logger.cpp
   Centralized logging and heartbeat implementation
   VERSION: V15.0.1-2026-01-03T10:05:00Z - Extracted from main controller
*/

#includ#include "Logger.h"
#include <stdarg.h>

static Logger* _instance = nullptr;

Logger& Logger::instance() {
    if (!_instance) {
        _instance = new Logger();
    }
    return *_instance;
}

Logger::Logger() {
}

Logger::~Logger() {
}

// ------------------------------------------------------------
// printf-style logging, safe for long strings
// ------------------------------------------------------------
void Logger::logf(const char* fmt, ...) {
    static const size_t BUF_SIZE = 1024; // safe for long paths / JSON names
    char buf[BUF_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, BUF_SIZE, fmt, args);
    va_end(args);

    Serial.println(buf);
}

// ------------------------------------------------------------
// simple string logging
// ------------------------------------------------------------
void Logger::log(const String& message) {
    Serial.println(message);
}
e "Logger.h"
#include "Config.h"

// V15.0.1-2026-01-03T10:05:00Z - Global logger instance
Logger logger;

Logger::Logger() : timeClient(nullptr), webLogIndex(0), lastHeartbeat(0) {
    // V15.0.1-2026-01-03T10:05:00Z - Initialize web log buffer
    for (int i = 0; i < MAX_LOG_MESSAGES; i++) {
        webLogBuffer[i] = "";
    }
}

void Logger::begin(NTPClient* ntpClient) {
    // V15.0.1-2026-01-03T10:05:00Z - Store NTP client reference
    timeClient = ntpClient;
    lastHeartbeat = millis();
    
    log("Logger initialized");
}

String Logger::getTimestamp() {
    // V15.0.1-2026-01-03T10:05:00Z - Generate timestamp string
    if (!timeClient) return "[NO TIME]";
    
    time_t epochTime = timeClient->getEpochTime();
    struct tm *ptm = gmtime(&epochTime);
    
    char timestamp[32];
    snprintf(timestamp, sizeof(timestamp), "[%04d-%02d-%02d %02d:%02d:%02d]",
             ptm->tm_year + 1900,
             ptm->tm_mon + 1,
             ptm->tm_mday,
             timeClient->getHours(),
             timeClient->getMinutes(),
             timeClient->getSeconds());
    
    return String(timestamp);
}

String Logger::getThemeName(uint8_t theme) {
    // V15.0.1-2026-01-03T10:05:00Z - Convert theme ID to name
    switch(theme) {
        case THEME_OFF: return "OFF";
        case THEME_CHRISTMAS_STATIC: return "XMAS STATIC";
        case THEME_CHRISTMAS_ANIMATED: return "XMAS ANIM";
        case THEME_RANDOM_MAGIC: return "RANDOM";
        case THEME_HALLOWEEN_STATIC: return "HALLOWEEN STATIC";
        case THEME_HALLOWEEN_ANIMATED: return "HALLOWEEN ANIM";
        case THEME_THANKSGIVING_STATIC: return "THANKSGIVING STATIC";
        case THEME_THANKSGIVING_ANIMATED: return "THANKSGIVING ANIM";
        case THEME_NEWYEAR_STATIC: return "NEW YEAR STATIC";
        case THEME_NEWYEAR_ANIMATED: return "NEW YEAR ANIM";
        case THEME_OSU_STATIC: return "OSU STATIC";
        case THEME_OSU_ANIMATED: return "OSU ANIM";
        case THEME_TEST_MODE: return "TEST";
        default: return "UNKNOWN";
    }
}

void Logger::log(const String& message) {
    // V15.0.1-2026-01-03T10:05:00Z - Log to serial and web buffer
    String logMsg = getTimestamp() + " " + message;
    Serial.println(logMsg);
    addWebLog(logMsg);
}

void Logger::logf(const char* format, ...) {
    // V15.0.1-2026-01-03T10:05:00Z - Formatted logging
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    log(String(buffer));
}

void Logger::updateHeartbeat(uint8_t currentTheme, uint32_t freeHeap) {
    // V15.0.1-2026-01-03T10:05:00Z - Periodic heartbeat message
    unsigned long now = millis();
    
    if (now - lastHeartbeat > HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = now;
        
        String msg = "Heartbeat - Theme: " + String(currentTheme) + 
                     " (" + getThemeName(currentTheme) + 
                     "), Free heap: " + String(freeHeap) + " bytes";
        log(msg);
    }
}

void Logger::addWebLog(const String& message) {
    // V15.0.1-2026-01-03T10:05:00Z - Add to circular web log buffer
    webLogBuffer[webLogIndex] = message;
    webLogIndex = (webLogIndex + 1) % MAX_LOG_MESSAGES;
}

String Logger::getWebLogs() {
    // V15.0.1-2026-01-03T10:05:00Z - Get all web logs in order
    String logs = "";
    
    // Start from oldest message
    for (int i = 0; i < MAX_LOG_MESSAGES; i++) {
        int index = (webLogIndex + i) % MAX_LOG_MESSAGES;
        if (webLogBuffer[index].length() > 0) {
            logs += webLogBuffer[index] + "\n";
        }
    }
    
    return logs;
}
