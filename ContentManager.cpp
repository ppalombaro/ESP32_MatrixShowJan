#include "ContentManager.h"

void ContentManager::loadDynamicAnimations(HolidayAnims* holidayAnims) {
    if (!holidayAnims) return;

    // Example: create a base Animation and add it
    Animation* anim = new Animation(nullptr); // Replace nullptr with real MatrixDisplay*
    holidayAnims->addAnimation(anim);

    // Keep any additional dynamic animation logic you already have
}
