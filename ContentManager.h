#pragma once
#include <Arduino.h>
#include "Logger.h"

class HolidayAnims;

class ContentManager {
public:
    ContentManager() {}

    void begin() {
        Logger::instance().log("ContentManager ready");
    }

    void populateAnimations(HolidayAnims* anims) {
        (void)anims;
        Logger::instance().log("Animations populated");
    }

    void update() {
        // no-op for now
    }
};
