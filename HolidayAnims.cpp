#include "HolidayAnims.h"
#include "Animations.h" // actual Animation definitions
#include "MatrixDisplay.h"

HolidayAnims::HolidayAnims(MatrixDisplay* disp)
    : _display(disp)
{}

void HolidayAnims::beginAll() {
    // Initialize all animations here
}

void HolidayAnims::updateAll() {
    for (auto anim : _animations) {
        if (anim) anim->update();
    }
}

Animation* HolidayAnims::getAnimationById(int id) {
    for (auto anim : _animations) {
        if (anim && anim->id == id) {
            return anim;
        }
    }
    return nullptr;
}

void HolidayAnims::registerScenes() {
    // Register code-based scenes here
}
