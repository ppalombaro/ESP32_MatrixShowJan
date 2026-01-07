#pragma once

#include <Arduino.h>
#include "Animations.h"

// Container for all code-based holiday animations
class HolidayAnims {
public:
    explicit HolidayAnims(MatrixDisplay* disp);

    // Initialize all animations
    void beginAll();

    // Update all animations (call every loop)
    void updateAll();

    // Resolve a code-based animation by ID
    // Returns nullptr if ID is not handled here
    Animation* getAnimationById(int id);

private:
    MatrixDisplay* disp;

    // Code-based animations
    SnowfallEffect        snowfall;
    ChaseAnimation        chase;
    ScrollAnimation       scroll;
    MerryChristmasCount   countdown;
    SpinningSnowflake     snowflake;
    SparklingStar         star;
};
