#pragma once
#include "HolidayAnims.h"
#include "Animations.h"

// Forward declarations of your static animations
extern Animation* animRainbow;
extern Animation* animBlink;
extern Animation* animFirework;

// Function to register all static animations into HolidayAnims
inline void registerStaticAnimations(HolidayAnims* holiday) {
    if (!holiday) return;

    // Add your static animations here
    holiday->addAnimation(animRainbow);
    holiday->addAnimation(animBlink);
    holiday->addAnimation(animFirework);
}
