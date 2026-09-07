/* ThemeManager.cpp
   Theme control implementation
   VERSION: V16.1.2-2026-01-08T15:00:00Z
*/

#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "ContentManager.h"
#include "Logger.h"
#include "Config.h"

ThemeManager::ThemeManager() {}

void ThemeManager::begin(MatrixDisplay* display, ContentManager* content) {
    disp = display;
    contentMgr = content;
    currentTheme = 0;
    Logger::instance().log("[ThemeManager] Initialized");
}

void ThemeManager::setTheme(uint8_t theme) {
    currentTheme = theme;
    Logger::instance().log("[ThemeManager] Theme set to " + String(theme));
}

void ThemeManager::update() {
    // V16.1.2 - Future: auto-cycling, theme-based updates
}

void ThemeManager::renderContent(uint16_t contentId) {
    if (contentMgr) {
        contentMgr->renderContent(contentId);
    }
}

// V16.4.13 - Color accessors keyed on the THEME_* constants, using the
// palette macros from Config.h. Off / unknown -> dim white (keeps text visible).
CRGB ThemeManager::getColor1() const {
    switch (currentTheme) {
        case THEME_CHRISTMAS:    return CHRISTMAS_COLOR_1;
        case THEME_HALLOWEEN:    return HALLOWEEN_COLOR_1;
        case THEME_THANKSGIVING: return THANKSGIVING_COLOR_1;
        case THEME_NEWYEAR:      return NEWYEAR_COLOR_1;
        case THEME_OSU:          return OSU_COLOR_1;
        default:                 return CRGB(40, 40, 40);
    }
}

CRGB ThemeManager::getColor2() const {
    switch (currentTheme) {
        case THEME_CHRISTMAS:    return CHRISTMAS_COLOR_2;
        case THEME_HALLOWEEN:    return HALLOWEEN_COLOR_2;
        case THEME_THANKSGIVING: return THANKSGIVING_COLOR_2;
        case THEME_NEWYEAR:      return NEWYEAR_COLOR_2;
        case THEME_OSU:          return OSU_COLOR_2;
        default:                 return CRGB(40, 40, 40);
    }
}

CRGB ThemeManager::getColor3() const {
    switch (currentTheme) {
        case THEME_CHRISTMAS:    return CHRISTMAS_COLOR_3;
        case THEME_HALLOWEEN:    return HALLOWEEN_COLOR_3;
        case THEME_THANKSGIVING: return THANKSGIVING_COLOR_3;
        case THEME_NEWYEAR:      return NEWYEAR_COLOR_3;
        case THEME_OSU:          return OSU_COLOR_3;
        default:                 return CRGB(60, 60, 60);
    }
}

uint8_t ThemeManager::themeNameToId(const String& name) {
    String n = name;
    n.toLowerCase();
    if (n == "christmas")    return THEME_CHRISTMAS;
    if (n == "halloween")    return THEME_HALLOWEEN;
    if (n == "thanksgiving") return THEME_THANKSGIVING;
    if (n == "newyear" || n == "new_year" || n == "new-year") return THEME_NEWYEAR;
    if (n == "osu")          return THEME_OSU;
    return THEME_OFF;
}
