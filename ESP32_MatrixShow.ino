#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>
#include "esp_task_wdt.h"

#include "Config.h"
#include "MatrixDisplay.h"
#include "HolidayAnims.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "Logger.h"

Preferences preferences;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000);

MatrixDisplay display;
HolidayAnims holidayAnims(&display);
ContentManager content;
ThemeManager themes;
WebController web;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Logger::instance().log("Starting ESP32 Matrix Show");

    display.begin();
    content.begin();

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());

    timeClient.begin();

    themes.begin(&display, &content);
    web.begin(&themes, &content);

    content.populateAnimations(&holidayAnims);

    Logger::instance().log("Setup complete");
}

void loop() {
    esp_task_wdt_reset();
    timeClient.update();

    web.handle();
    themes.update();
    content.update();
    holidayAnims.updateAll();

    delay(10);
}
