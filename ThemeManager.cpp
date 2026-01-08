#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "ContentManager.h"
#include "Logger.h"

ThemeManager::ThemeManager() {}

void ThemeManager::begin(MatrixDisplay* disp, ContentManager* cont) {
    display = disp;
    content = cont;
    currentTheme = 0;

    Logger::instance().log("Themes initialized");
}

void ThemeManager::setTheme(uint8_t mode) {
    currentTheme = mode;
    Logger::instance().log("Theme set to " + String(mode));
}

void ThemeManager::update() {
    // Reserved for future theme animation logic
}
