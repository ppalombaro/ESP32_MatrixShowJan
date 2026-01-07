#pragma once
#include <Arduino.h>
#include "ContentManager.h"

#define THEMEMANAGER_VERSION "15.4FIX"

class ThemeManager {
public:
    static ThemeManager& instance();
    void begin();

    // Filtered views
    std::vector<const SceneEntry*> getScenesForTheme(ThemeId theme);

private:
    ThemeManager();
    ~ThemeManager();

    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
};
