#pragma once

#include <Arduino.h>

class MatrixDisplay;
class ContentManager;

class ThemeManager {
public:
    ThemeManager() {}

    // Match ESP32_MatrixShow.ino
    void begin(MatrixDisplay*, ContentManager*) {}
    void update() {}

    void setTheme(uint8_t) {}
    void setBrightness(uint8_t) {}

    void setSpecificScene(int) {}
    void setSpecificAnimation(int) {}
    void setSpecificTest(int) {}

    void setChristmasCountdownDate(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t) {}
    void setNewYearCountdownDate(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t) {}
    void resetCountdownDates() {}
};
