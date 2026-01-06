/* WebController.h
   Web server coordinator - now delegates to WebPages and WebActions
   VERSION: V15.1.0-2026-01-04T10:25:00Z - Refactored architecture
   
   V15.1.0-2026-01-04T10:25:00Z - Split functionality into modular components
   V15.1.0-2026-01-04T10:25:00Z - WebController is now thin routing layer
*/

#ifndef WEB_CONTROLLER_H
#define WEB_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "Config.h"
#include "ThemeManager.h"
#include "ContentManager.h"  // V15.1.0-2026-01-04T10:25:00Z
#include "WebPages.h"        // V15.1.0-2026-01-04T10:25:00Z
#include "WebActions.h"      // V15.1.0-2026-01-04T10:25:00Z

class WebController {
public:
    WebController();
    void begin(ThemeManager* themeMgr, ContentManager* contentMgr);  // V15.1.0-2026-01-04T10:25:00Z - Added ContentManager
    void handleClient();

private:
    WebServer server;
    ThemeManager* themes;
    ContentManager* content;  // V15.1.0-2026-01-04T10:25:00Z
    WebPages* pages;          // V15.1.0-2026-01-04T10:25:00Z - HTML generation
    WebActions* actions;      // V15.1.0-2026-01-04T10:25:00Z - Request handling
    
    void setupWiFi();
    
    // V15.1.0-2026-01-04T10:25:00Z - Page request handlers (delegate to WebPages)
    void handleRoot();
    void handleConfig();
    void handlePreview();
    void handleRandomConfigPage();
    void handleCountdownConfig();
    void handleLogs();
    
    // V15.1.0-2026-01-04T10:25:00Z - Action request handlers (delegate to WebActions)
    void handleSetMode();
    void handleSetScene();
    void handleSetAnimation();
    void handleSetTest();
    void handleBrightness();
    void handleClear();
    void handleSaveConfig();
    void handleSaveRandomConfig();
    void handleSetChristmasCountdown();
    void handleSetNewYearCountdown();
    void handleResetCountdown();
};

#endif
