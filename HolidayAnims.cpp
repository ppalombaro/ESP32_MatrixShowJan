#include "HolidayAnims.h"

HolidayAnims::HolidayAnims(MatrixDisplay* disp)
    : _display(disp) {}

// Initialize all animations
void HolidayAnims::beginAll() {
    for (auto anim : _animations) {
        if (anim) anim->begin();  // safe call
    }
}

// Update all animations
void HolidayAnims::updateAll() {
    for (auto anim : _animations) {
        if (anim) anim->update();  // safe call
    }
}

// Add a static or dynamic animation
void HolidayAnims::addAnimation(Animation* anim) {
    if (anim) _animations.push_back(anim);
}

// Retrieve animation by index
Animation* HolidayAnims::getAnimationById(int id) {
    if (id >= 0 && id < _animations.size()) {
        return _animations[id];
    }
    return nullptr;
}
