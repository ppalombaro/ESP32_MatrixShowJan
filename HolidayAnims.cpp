#include "HolidayAnims.h"

HolidayAnims::HolidayAnims(MatrixDisplay* disp)
    : _display(disp) {}

// Initialize all animations
void HolidayAnims::beginAll() {
    for (auto anim : _animations) {
        if (anim) anim->begin(_display);
    }
}

// Update all animations (call in loop)
void HolidayAnims::updateAll() {
    for (auto anim : _animations) {
        if (anim) anim->update();
    }
}

// Retrieve animation by ID
Animation* HolidayAnims::getAnimationById(int id) {
    for (auto anim : _animations) {
        if (anim && anim->id == id) {
            return anim;
        }
    }
    return nullptr;
}
