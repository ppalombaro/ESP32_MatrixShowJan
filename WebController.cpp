/* WebController.cpp
   Lightweight web server coordinator - delegates to WebPages and WebActions
   VERSION: V15.1.0-2026-01-04T10:25:00Z - Refactored for modularity
   
   V15.1.0-2026-01-04T10:25:00Z - Split into WebPages + WebActions modules
   V15.1.0-2026-01-04T10:25:00Z - Removed hardcoded HTML (now in WebPages)
   V15.1.0-2026-01-04T10:25:00Z - WebController is now "dumb" - just routes requests
*/

#include "WebController.h"
#include "Logger.h"

extern Logger logger;  // V15.1.0-2026-01-04T10:25:00Z

// V15.1.0-2026-01-04T10:25:00Z - Ring buffer for web logs
#define MAX_WEB_LOGS 25
String webLogs[MAX_WEB_LOGS];
int webLogIndex = 0;

void addWebLog(String message) {
    webLogs[webLogIndex] = message;
    webLogIndex = (webLogIndex + 1) % MAX_WEB_LOGS;
}

WebController::WebController() : server(80), pages(nullptr), actions(nullptr) {
}

void WebController::begin(ThemeManager* themeMgr, ContentManager* contentMgr) {
    themes = themeMgr;        // V15.1.0-2026-01-04T10:25:00Z
    content = contentMgr;     // V15.1.0-2026-01-04T10:25:00Z
    
    // V15.1.0-2026-01-04T10:25:00Z - Initialize page generator and action handler
    pages = new WebPages(contentMgr);
    actions = new WebActions(themeMgr, contentMgr);
    
    setupWiFi();
    
    // V15.1.0-2026-01-04T10:25:00Z - Main page routes
    server.on("/", [this](){ handleRoot(); });
    server.on("/config", [this](){ handleConfig(); });
    server.on("/preview", [this](){ handlePreview(); });
    server.on("/logs", [this](){ handleLogs(); });
    server.on("/randomconfig", HTTP_GET, [this](){ 
        if (server.hasArg("content")) {
            handleSaveRandomConfig();
        } else {
            handleRandomConfigPage();
        }
    });
    
    // V15.1.0-2026-01-04T10:25:00Z - Countdown configuration routes
    server.on("/countdownconfig", [this](){ handleCountdownConfig(); });
    server.on("/setxmascountdown", [this](){ handleSetChristmasCountdown(); });
    server.on("/setnycountdown", [this](){ handleSetNewYearCountdown(); });
    server.on("/resetcountdown", [this](){ handleResetCountdown(); });
    
    // V15.1.0-2026-01-04T10:25:00Z - Control action routes
    server.on("/mode", [this](){ handleSetMode(); });
    server.on("/setscene", [this](){ handleSetScene(); });
    server.on("/setanim", [this](){ handleSetAnimation(); });
    server.on("/settest", [this](){ handleSetTest(); });
    server.on("/clear", [this](){ handleClear(); });
    server.on("/brightness", [this](){ handleBrightness(); });
    server.on("/saveconfig", [this](){ handleSaveConfig(); });
    
    server.begin();
    logger.log("Web server started");
}

void WebController::setupWiFi() {
    // V15.1.0-2026-01-04T10:25:00Z - WiFi connection (unchanged)
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.setHostname(HOSTNAME);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        
        if (MDNS.begin(HOSTNAME)) {
            Serial.println("mDNS responder started");
            Serial.print("Access at: http://");
            Serial.print(HOSTNAME);
            Serial.println(".local");
        }
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

void WebController::handleClient() {
    server.handleClient();  // V15.1.0-2026-01-04T10:25:00Z
}

void WebController::handleRoot() {
    // V15.1.0-2026-01-04T10:25:00Z - Delegate to WebPages
    String html = pages->getHomePage(themes->getCurrentTheme(), FastLED.getBrightness());
    server.send(200, "text/html", html);
}

void WebController::handleConfig() {
    // V15.1.0-2026-01-04T10:25:00Z - Delegate to WebPages
    ScheduleConfig config = actions->getCurrentSchedule();
    String html = pages->getConfigPage(config);
    server.send(200, "text/html", html);
}

void WebController::handlePreview() {
    // V15.1.0-2026-01-04T10:25:00Z - Auto-generated from ContentManager
    String html = pages->getPreviewPage(themes->getCurrentTheme());
    server.send(200, "text/html", html);
}

void WebController::handleRandomConfigPage() {
    // V15.1.0-2026-01-04T10:25:00Z - Checkbox-based random config
    ScheduleConfig config = actions->getCurrentSchedule();
    String html = pages->getRandomConfigPage(config);
    server.send(200, "text/html", html);
}

void WebController::handleCountdownConfig() {
    // V15.1.0-2026-01-04T10:25:00Z - Countdown date configuration page
    CountdownDate xmas = themes->getChristmasCountdownDate();
    CountdownDate ny = themes->getNewYearCountdownDate();
    
    String html = pages->getCountdownConfigPage(
        xmas.year, xmas.month, xmas.day,
        ny.year, ny.month, ny.day
    );
    server.send(200, "text/html", html);
}

void WebController::handleLogs() {
    // V15.1.0-2026-01-04T10:25:00Z - System logs display
    String html = pages->getLogsPage(webLogs, MAX_WEB_LOGS, webLogIndex);
    server.send(200, "text/html", html);
}

void WebController::handleSetMode() {
    // V15.1.0-2026-01-04T10:25:00Z - Simple action delegation
    if (server.hasArg("theme")) {
        uint8_t theme = server.arg("theme").toInt();
        actions->setMode(theme);
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void WebController::handleSetScene() {
    // V15.1.0-2026-01-04T10:25:00Z - Action delegation
    if (server.hasArg("id")) {
        int index = server.arg("id").toInt();
        actions->setScene(index);
    }
    server.sendHeader("Location", "/preview");
    server.send(303);
}

void WebController::handleSetAnimation() {
    // V15.1.0-2026-01-04T10:25:00Z - Action delegation
    if (server.hasArg("id")) {
        int index = server.arg("id").toInt();
        actions->setAnimation(index);
    }
    server.sendHeader("Location", "/preview");
    server.send(303);
}

void WebController::handleSetTest() {
    // V15.1.0-2026-01-04T10:25:00Z - Action delegation
    if (server.hasArg("id")) {
        int index = server.arg("id").toInt();
        actions->setTest(index);
    }
    server.sendHeader("Location", "/preview");
    server.send(303);
}

void WebController::handleBrightness() {
    // V15.1.0-2026-01-04T10:25:00Z - Action delegation
    if (server.hasArg("val")) {
        uint8_t val = server.arg("val").toInt();
        actions->setBrightness(val);
    }
    server.send(200, "text/plain", "OK");
}

void WebController::handleClear() {
    // V15.1.0-2026-01-04T10:25:00Z - Action delegation
    actions->clearDisplay();
    server.sendHeader("Location", "/");
    server.send(303);
}

void WebController::handleSaveConfig() {
    // V15.1.0-2026-01-04T10:25:00Z - Parse form and save schedule
    ScheduleConfig config;
    
    if (server.hasArg("startH")) config.startHour = server.arg("startH").toInt();
    if (server.hasArg("startM")) config.startMinute = server.arg("startM").toInt();
    if (server.hasArg("endH")) config.endHour = server.arg("endH").toInt();
    if (server.hasArg("endM")) config.endMinute = server.arg("endM").toInt();
    if (server.hasArg("mode")) config.mode = server.arg("mode");
    config.enabled = server.hasArg("enabled");
    
    actions->saveScheduleConfig(config);
    
    server.sendHeader("Location", "/config");
    server.send(303);
}

void WebController::handleSaveRandomConfig() {
    // V15.1.0-2026-01-04T10:25:00Z - Parse checkboxes and save
    std::vector<String> enabledContent;
    
    // Collect all checked items
    for (int i = 0; i < server.args(); i++) {
        if (server.argName(i) == "content") {
            enabledContent.push_back(server.arg(i));
        }
    }
    
    actions->saveRandomConfig(enabledContent);
    
    server.sendHeader("Location", "/randomconfig");
    server.send(303);
}

void WebController::handleSetChristmasCountdown() {
    // V15.1.0-2026-01-04T10:25:00Z - Parse and save Christmas countdown
    if (server.hasArg("year") && server.hasArg("month") && server.hasArg("day")) {
        uint16_t year = server.arg("year").toInt();
        uint8_t month = server.arg("month").toInt();
        uint8_t day = server.arg("day").toInt();
        
        actions->setChristmasCountdown(year, month, day);
    }
    
    server.sendHeader("Location", "/countdownconfig");
    server.send(303);
}

void WebController::handleSetNewYearCountdown() {
    // V15.1.0-2026-01-04T10:25:00Z - Parse and save New Year countdown
    if (server.hasArg("year") && server.hasArg("month") && server.hasArg("day")) {
        uint16_t year = server.arg("year").toInt();
        uint8_t month = server.arg("month").toInt();
        uint8_t day = server.arg("day").toInt();
        
        actions->setNewYearCountdown(year, month, day);
    }
    
    server.sendHeader("Location", "/countdownconfig");
    server.send(303);
}

void WebController::handleResetCountdown() {
    // V15.1.0-2026-01-04T10:25:00Z - Reset countdowns to defaults
    actions->resetCountdowns();
    
    server.sendHeader("Location", "/countdownconfig");
    server.send(303);
}
