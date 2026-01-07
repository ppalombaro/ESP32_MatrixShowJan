#include "ThemeManager.h"

static ThemeManager* _instance = nullptr;

ThemeManager& ThemeManager::instance() {
    if (!_instance) _instance = new ThemeManager();
    return *_instance;
}

ThemeManager::ThemeManager() {}
ThemeManager::~ThemeManager() {}

void ThemeManager::begin() {
    // Nothing to allocate; purely metadata / filter
}

std::vector<const SceneEntry*> ThemeManager::getScenesForTheme(ThemeId theme) {
    return ContentManager::instance().getScenesForTheme(theme);
}
