#pragma once

#include <Arduino.h>

class MatrixDisplay;
class ContentManager;

class ThemeManager {
public:
    ThemeManager();

    void begin(MatrixDisplay* display, ContentManager* content);
    void update();

    void setTheme(uint8_t mode);

private:
    MatrixDisplay* display = nullptr;
    ContentManager* content = nullptr;

    uint8_t currentTheme = 0;
};
