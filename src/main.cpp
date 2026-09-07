/* ESP32_MatrixShow - Main program entry point
   VERSION: V16.4.13-2026-09-07 - DST-aware SNTP (configTzTime), NTPClient removed
*/

#include <Arduino.h>
#include <WiFi.h>
#include <FastLED.h>
#include <Preferences.h>
#include <time.h>
#include "Config.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "ThemeManager.h"
#include "ContentManager.h"
#include "WebController.h"

// POSIX TZ for US Eastern with automatic DST (EDT Mar 2nd Sun - Nov 1st Sun)
#define TZ_STRING "EST5EDT,M3.2.0/2,M11.1.0/2"

// Global objects
Preferences preferences;

MatrixDisplay display;
ThemeManager themeManager;  // Must be constructed before ContentManager uses it
ContentManager content;
WebController web;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Logger::instance().log("=================================");
    Logger::instance().log("ESP32 Matrix Show " FW_VERSION);
    Logger::instance().log("Content Auto-Discovery System");
    Logger::instance().log("=================================");

    // Initialize display hardware
    display.begin();
    Logger::instance().log("[SETUP] Display initialized");

    // Discover all content from the custom flash blob
    content.begin(&display);
    Logger::instance().log("[SETUP] Content discovery complete");

    // Initialize WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Logger::instance().log("[SETUP] Connecting to WiFi...");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Logger::instance().log("[SETUP] WiFi connected: " + WiFi.localIP().toString());
        // Start SNTP + local timezone (DST-aware). getLocalTime() is used elsewhere.
        configTzTime(TZ_STRING, "pool.ntp.org", "time.nist.gov");
        Logger::instance().log("[SETUP] SNTP started (TZ " TZ_STRING ")");
    } else {
        Logger::instance().log("[SETUP] WiFi connection FAILED - continuing offline");
    }

    // Initialize theme manager
    themeManager.begin(&display, &content);
    Logger::instance().log("[SETUP] Theme manager initialized");

    // Initialize web interface
    web.begin(&content, &themeManager, &display);
    Logger::instance().log("[SETUP] Web interface started");

    Logger::instance().log("[SETUP] System ready!");
    Logger::instance().log("=================================");
}

void loop() {
    web.handle();
    themeManager.update();
    content.update();
    delay(10);
}
