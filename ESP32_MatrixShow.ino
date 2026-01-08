#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>      // Required for permanent storage
#include "esp_task_wdt.h"      // Required for watchdog timer
#include <WiFi.h>


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

    display.begin();
    Serial.println("--- Display Begin ---");

    content.begin();
    Serial.println("--- Content Begin ---");

    // -----------------------------
    // WiFi MUST be up before NTP/Web
    // -----------------------------
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());

    // -----------------------------
    // Network consumers AFTER WiFi
    // -----------------------------
    timeClient.begin();
    Serial.println("--- Time client started ---");

    themes.begin(&display, &content);
    Serial.println("--- Themes initialized ---");

    web.begin(&themes, &content);
    Serial.println("--- Web server started ---");

    content.populateAnimations(&holidayAnims);

    Logger::instance().log("Setup complete.");
}
 
// -----------------------------
// Main loop
// -----------------------------
void loop() {
    esp_task_wdt_reset();

    timeClient.update();
    web.handle();

    content.update();
    holidayAnims.updateAll();

    delay(10);
}
