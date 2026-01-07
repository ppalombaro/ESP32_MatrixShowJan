#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>      // Required for permanent storage
#include "esp_task_wdt.h"      // Required for watchdog timer


// Project Headers
#include "Config.h"
#include "MatrixDisplay.h"
#include "HolidayAnims.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "Logger.h"

// -----------------------------
// Global objects
// -----------------------------
Preferences preferences;  

// NTP Client Setup
WiFiUDP ntpUDP;
// -18000 is the offset in seconds for EST (-5 hours)
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000); 

MatrixDisplay display;
HolidayAnims holidayAnims(&display);
ContentManager content;
ThemeManager themes;
WebController web;

// -----------------------------
// Setup
// -----------------------------
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- Starting ESP32 Matrix Show V15.4 Fix ---");

    // Watchdog setup - 30s timeout
    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);
    Serial.println("System: Watchdog set to 30s");

    // Initialize components
    if (!display.begin()) {
        Serial.println("Display Init Failed!");
    }

    if (!content.begin()) {
        Serial.println("Content Manager Init Failed!");
    }

    themes.begin(&display, &content);
    web.begin(&themes, &content);

    // Start NTP
    timeClient.begin();

    // Populate static and dynamic animations
    content.populateAnimations(&holidayAnims);

    Logger::instance().log("Setup complete.");
}

// -----------------------------
// Main loop
// -----------------------------
void loop() {
    esp_task_wdt_reset();      // Reset watchdog

    timeClient.update();       // Update NTP

    web.handleClient();        // Correct WebController handling

    if (isScheduledTime()) {
        themes.update();           // Run active theme
    } else {
        static bool was_on = true;
        if (was_on) {
            display.clear();
            display.show();
            was_on = false;
            Serial.println("Schedule: Display entering Sleep Mode");
        }
    }

    content.update();          // Dynamic content
    holidayAnims.updateAll();  // Update animations

    delay(10);
}
