#include <Arduino.h>
#include "MatrixDisplay.h"
#include "HolidayAnims.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "Logger.h"

// -----------------------------
// Global objects
// -----------------------------
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

    display.begin();    // Initialize the LED matrix
    content.begin();    // Initialize SPIFFS / content system
    themes.begin(&display, &content);
    web.begin(&themes, &content);

    // Populate all animations (static + dynamic)
    content.populateAnimations(&holidayAnims);

    Logger::instance().log("Setup complete.");
}

// -----------------------------
// Main loop
// -----------------------------
void loop() {
    content.update();       // Update dynamic content if needed
    themes.update();        // Update any active theme animations
    web.handle();           // Handle web requests
    holidayAnims.updateAll(); // Update all registered animations

    delay(10);              // Loop throttle
}

// -----------------------------
// Scheduled content (placeholder for future use)
// -----------------------------
bool isScheduledTime() {
    return true; // Currently always true; can integrate Scheduler later
}
