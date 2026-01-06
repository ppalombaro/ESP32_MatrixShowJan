/* WebActions.h
   Web request action handlers
   VERSION: V15.1.0-2026-01-04T10:20:00Z - Split from WebController
   
   V15.1.0-2026-01-04T10:20:00Z - Dumb action layer (no business logic)
*/

#ifndef WEB_ACTIONS_H
#define WEB_ACTIONS_H

#include <Arduino.h>
#include "ThemeManager.h"
#include "ContentManager.h"
#include "Logger.h"

class WebActions {
public:
    WebActions(ThemeManager* tm, ContentManager* cm);  // V15.1.0-2026-01-04T10:20:00Z
    
    // Display control actions
    void setMode(uint8_t theme);            // V15.1.0-2026-01-04T10:20:00Z
    void setScene(int index);               // V15.1.0-2026-01-04T10:20:00Z
    void setAnimation(int index);           // V15.1.0-2026-01-04T10:20:00Z
    void setTest(int index);                // V15.1.0-2026-01-04T10:20:00Z
    void setBrightness(uint8_t value);      // V15.1.0-2026-01-04T10:20:00Z
    void clearDisplay();                    // V15.1.0-2026-01-04T10:20:00Z
    
    // Configuration actions
    void saveScheduleConfig(const ScheduleConfig& config);          // V15.1.0-2026-01-04T10:20:00Z
    void saveRandomConfig(const std::vector<String>& enabledContent);  // V15.1.0-2026-01-04T10:20:00Z
    
    // Countdown actions
    void setChristmasCountdown(uint16_t year, uint8_t month, uint8_t day, 
                              uint8_t hour = 0, uint8_t minute = 0);  // V15.1.0-2026-01-04T10:20:00Z
    void setNewYearCountdown(uint16_t year, uint8_t month, uint8_t day,
                            uint8_t hour = 0, uint8_t minute = 0);    // V15.1.0-2026-01-04T10:20:00Z
    void resetCountdowns();                                          // V15.1.0-2026-01-04T10:20:00Z
    
    // Query current state
    ScheduleConfig getCurrentSchedule();  // V15.1.0-2026-01-04T10:20:00Z
    
private:
    ThemeManager* themeMgr;    // V15.1.0-2026-01-04T10:20:00Z
    ContentManager* contentMgr;  // V15.1.0-2026-01-04T10:20:00Z
};

#endif
