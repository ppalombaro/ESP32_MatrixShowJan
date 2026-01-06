/* WebPages.h
   HTML page generation interface
   VERSION: V15.1.0-2026-01-04T10:15:00Z - Split from WebController
   
   V15.1.0-2026-01-04T10:15:00Z - Modular HTML generation with PROGMEM
*/

#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>
#include "ContentManager.h"

class WebPages {
public:
    WebPages(ContentManager* cm);  // V15.1.0-2026-01-04T10:15:00Z
    
    String getHomePage(uint8_t currentTheme, uint8_t brightness);  // V15.1.0-2026-01-04T10:15:00Z
    String getConfigPage(const ScheduleConfig& schedule);         // V15.1.0-2026-01-04T10:15:00Z
    String getPreviewPage(uint8_t currentTheme);                  // V15.1.0-2026-01-04T10:15:00Z
    String getRandomConfigPage(const ScheduleConfig& schedule);   // V15.1.0-2026-01-04T10:15:00Z
    String getCountdownConfigPage(uint16_t xmasYear, uint8_t xmasMonth, uint8_t xmasDay,
                                 uint16_t nyYear, uint8_t nyMonth, uint8_t nyDay);  // V15.1.0-2026-01-04T10:15:00Z
    String getLogsPage(const String* logs, int maxLogs, int currentIndex);  // V15.1.0-2026-01-04T10:15:00Z
    
private:
    ContentManager* contentMgr;  // V15.1.0-2026-01-04T10:15:00Z
    
    String buildPage(const String& content);  // V15.1.0-2026-01-04T10:15:00Z - Wraps content with header/footer
};

#endif
