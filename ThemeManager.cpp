#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "ContentManager.h"

ThemeManager::ThemeManager() {}

void ThemeManager::begin() {
    // zero-arg begin
}

void ThemeManager::begin(MatrixDisplay* display, ContentManager* content) {
    _display = display;
    _content = content;
}

void ThemeManager::update() {
    // update logic
}
