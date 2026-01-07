#include <Arduino.h>
#include "MatrixDisplay.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "HolidayAnims.h"
#include "Logger.h"

MatrixDisplay display;
ContentManager& content = ContentManager::instance();
ThemeManager themes;
WebController web;
HolidayAnims holidayAnims(&display);

void setup() {
    Serial.begin(115200);

    if (!display.begin()) {
        Logger::instance().log("Display init failed!");
        while (1);
    }

    content.begin();
    themes.begin(&display, &content);
    web.begin(&themes, &content);

    holidayAnims.registerScenes();

    Logger::instance().log("Setup complete.");
}

void loop() {
    content.update();
    themes.update();
    web.handle();
    holidayAnims.updateAll();
    delay(10);
}

bool isScheduledTime() {
    return true;
}
