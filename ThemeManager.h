#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "ContentManager.h"

ThemeManager::ThemeManager() {
    // optional constructor logic
}

// Original begin
void ThemeManager::begin() {
    // existing initialization logic
}

// V15.4FIX.1 overload to accept display + content (used in .ino)
void ThemeManager::begin(MatrixDisplay* display, ContentManager* content) {
    _display = display;
    _content = content;

    // reuse original begin logic
    begin();
}

// Update / tick method called in loop
void ThemeManager::update() {
    // existing update logic for current theme/scene/animation
}
