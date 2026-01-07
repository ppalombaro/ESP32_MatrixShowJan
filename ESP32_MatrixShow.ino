#include <Arduino.h>
#include "MatrixDisplay.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "HolidayAnims.h"
#include "Logger.h"

// Global objects
MatrixDisplay display;
ContentManager& content = ContentManager::instance();
ThemeManager themes;
WebController web;
HolidayAnims holidayAnims(&display);

void setup() {
    Serial.begin(115200);

    // Initialize display
    if (!display.begin()) {
        Logger::instance().log("Display init failed!");
        while (1); // halt
    }

    // Initialize content
    content.begin(); // overload returns bool if used in if(!content.begin())

    // Initialize themes
    themes.begin(&display, &content); // uses V15.4FIX.1 overload

    // Initialize web controller
    web.begin(&themes, &content); // uses V15.4FIX.1 overload

    // Register holiday scenes
    holidayAnims.registerScenes();

    Logger::instance().log("Setup complete.");
}

void loop() {
    // Update content or animations
    content.update();
    themes.update();  // safe now — update() exists
    web.handle();     // process web requests

    // Update animations
    holidayAnims.updateAll();

    delay(10); // small delay to reduce CPU usage
}

// Scheduling logic stub (was broken by missing ScheduleConfig/loadSchedule)
bool isScheduledTime() {
    // Temporarily disable scheduling to allow compilation
    return true;
}
