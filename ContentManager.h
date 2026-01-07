#pragma once
#include <Arduino.h>
#include "HolidayAnims.h"
#include "Animations.h"

class ContentManager {
public:
    ContentManager();

    void begin();                         // Initialize file system and content
    void update();                        // Update content if needed
    void populateAnimations(HolidayAnims* ha); // Scan dynamic JSON + register all animations

private:
    // private helpers
    void loadDynamicAnimations(HolidayAnims* ha);
    void loadStaticAnimations(HolidayAnims* ha);
};
