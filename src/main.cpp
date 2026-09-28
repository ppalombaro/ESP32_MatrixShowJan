/* ESP32_MatrixShow - Main program entry point
   VERSION: V16.4.13-2026-09-07 - DST-aware SNTP (configTzTime), NTPClient removed
*/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <Preferences.h>
#include <time.h>
#include <esp_heap_caps.h>
#include "Config.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "ThemeManager.h"
#include "ContentManager.h"
#include "WebController.h"

// V16.4.14 - Nightly reboot hour (local time) to clear heap fragmentation from
// long-running String/JSON churn. Chosen to fall outside the default schedule
// window (17:00-22:00) so it never interrupts an active display.
#define NIGHTLY_REBOOT_HOUR 3
#define HEAP_LOG_INTERVAL_MS (10UL * 60UL * 1000UL)  // every 10 minutes

// POSIX TZ for US Eastern with automatic DST (EDT Mar 2nd Sun - Nov 1st Sun)
#define TZ_STRING "EST5EDT,M3.2.0/2,M11.1.0/2"

// Global objects
Preferences preferences;

MatrixDisplay display;
ThemeManager themeManager;  // Must be constructed before ContentManager uses it
ContentManager content;
WebController web;

// V16.4.14 - Set while an ArduinoOTA flash is in progress. loop() gives OTA
// exclusive access to the CPU/flash bus while this is true, since content
// playback reads the same flash chip that Update.h is writing to.
static bool otaInProgress = false;

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

        // V16.4.14 - mDNS responder so the control page is reachable at
        // http://<HOSTNAME>.local even if the DHCP-assigned IP changes.
        if (MDNS.begin(HOSTNAME)) {
            MDNS.addService("http", "tcp", 80);
            Logger::instance().log("[SETUP] mDNS started: http://" HOSTNAME ".local");
        } else {
            Logger::instance().log("[SETUP] mDNS start FAILED");
        }

        // V16.4.14 - ArduinoOTA: network firmware flashing, independent of the
        // HTTP web server. This is the recovery path when the control page is
        // wedged but WiFi/OTA are still alive - `pio run -t upload
        // --upload-port <ip-or-HOSTNAME.local>` pushes a new build directly.
        ArduinoOTA.setHostname(HOSTNAME);
        ArduinoOTA.setPassword(OTA_PASSWORD);
        ArduinoOTA.onStart([]() {
            otaInProgress = true;
            content.stopPlayback();
            display.clear();
            display.show();
            String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
            Logger::instance().log("[OTA] Update starting (" + type + ")");
        });
        ArduinoOTA.onEnd([]() {
            Logger::instance().log("[OTA] Update complete - rebooting");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            static unsigned int lastPct = 999;
            unsigned int pct = total ? (progress * 100) / total : 0;
            if (pct != lastPct) {
                lastPct = pct;
                Serial.printf("[OTA] Progress: %u%%\n", pct);
            }
        });
        ArduinoOTA.onError([](ota_error_t error) {
            otaInProgress = false;
            String msg;
            switch (error) {
                case OTA_AUTH_ERROR:    msg = "Auth Failed";    break;
                case OTA_BEGIN_ERROR:   msg = "Begin Failed";   break;
                case OTA_CONNECT_ERROR: msg = "Connect Failed"; break;
                case OTA_RECEIVE_ERROR: msg = "Receive Failed"; break;
                case OTA_END_ERROR:     msg = "End Failed";     break;
                default:                msg = "Unknown";        break;
            }
            Logger::instance().log("[OTA] Error: " + msg);
        });
        ArduinoOTA.begin();
        Logger::instance().log("[SETUP] ArduinoOTA started (network flashing enabled)");
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

// V16.4.14 - Periodic free-heap logging so fragmentation trends are visible
// on the /logs page instead of only showing up as a mystery lockup.
static void logHeapStatus() {
    static unsigned long lastLog = 0;
    unsigned long now = millis();
    if (lastLog != 0 && now - lastLog < HEAP_LOG_INTERVAL_MS) return;
    lastLog = now;

    size_t freeHeap = ESP.getFreeHeap();
    size_t largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    Logger::instance().log("[Heap] free=" + String(freeHeap) +
                            " largestBlock=" + String(largestBlock) +
                            " uptimeMin=" + String(now / 60000UL));
}

// V16.4.14 - Nightly auto-reboot at NIGHTLY_REBOOT_HOUR local time. Clears any
// heap fragmentation from long-running String/JSON churn before it can cause
// the web UI or schedule gate to wedge. Falls outside the default schedule
// window so it doesn't interrupt an active show.
static void checkNightlyReboot() {
    static int lastRebootDay = -1;  // guards against multiple triggers within the same minute/hour
    struct tm ti;
    if (!getLocalTime(&ti, 0)) return;  // no blocking wait - skip if time not synced yet

    if (ti.tm_hour == NIGHTLY_REBOOT_HOUR && ti.tm_mday != lastRebootDay) {
        lastRebootDay = ti.tm_mday;
        Logger::instance().log("[SETUP] Nightly reboot (heap fragmentation guard) - restarting now");
        delay(200);  // let the log line flush over Serial
        ESP.restart();
    }
}

void loop() {
    ArduinoOTA.handle();
    if (otaInProgress) {
        delay(1);
        return;  // give the flash write exclusive access to CPU/flash bus
    }
    web.handle();
    themeManager.update();
    content.update();
    logHeapStatus();
    checkNightlyReboot();
    delay(10);
}
