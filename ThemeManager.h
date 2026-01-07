#pragma once

#include <Arduino.h>

// Forward declarations
class MatrixDisplay;
class ContentManager;

class ThemeManager {
public:
    ThemeManager();

    void begin(); // original zero-arg
    void begin(MatrixDisplay* display, ContentManager* content);

    void update();

private:
    MatrixDisplay* _display = nullptr;
    ContentManager* _content = nullptr;
};
