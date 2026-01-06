/* Logger.h
   Centralized logging and heartbeat system
   VERSION: V15.0.1-2026-01-03T10:00:00Z - Extracted from main controller
   
   V15.0.1-2026-01-03T10:00:00Z - Created separate logging module for better maintainability
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <NTPClient.h>

// V15.0.1-2026-01-03T10:00:00Z - Web log configuration
#define MAX_LOG_MESSAGES 50
#define HEARTBEAT_INTERVAL_MS 10000

class Logger {
public:
    Logger();
    
    // Initialize with NTP client reference
    void begin(NTPClient* ntpClient);
    
    // Logging functions
    void log(const String& message);
    void logf(const char* format, ...);
    
    // Heartbeat function
    void updateHeartbeat(uint8_t currentTheme, uint32_t freeHeap);
    
    // Web log access
    String getWebLogs();
    void addWebLog(const String& message);
    
private:
    NTPClient* timeClient;
    
    // Web log buffer
    String webLogBuffer[MAX_LOG_MESSAGES];
    int webLogIndex;
    
    // Heartbeat tracking
    unsigned long lastHeartbeat;
    
    // Helper functions
    String getTimestamp();
    String getThemeName(uint8_t theme);
};

// V15.0.1-2026-01-03T10:00:00Z - Global logger instance
extern Logger logger;

#endif // LOGGER_H
