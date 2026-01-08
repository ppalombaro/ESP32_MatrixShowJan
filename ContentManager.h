#pragma once

#include <Arduino.h>

class HolidayAnims;

class ContentManager {
public:
    ContentManager();

    void begin();
    void populateAnimations(HolidayAnims* anims);
    void update();
};
