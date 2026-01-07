#pragma once
#include "ContentManager.h"

class HolidayAnims {
public:
    static void registerScenes() {
        ContentManager::instance().registerCodeScene("Santa Parade", ThemeId::NEWYEAR, []() {
            // Example code-based animation
        });
        ContentManager::instance().registerCodeScene("Haunted House", ThemeId::HALLOWEEN, []() {
            // Example code-based animation
        });
    }
};
