#include "HolidayAnims.h"
#include "Animations.h"

HolidayAnims::HolidayAnims(MatrixDisplay* disp) : _display(disp) {}

void HolidayAnims::beginAll() {
    // Initialize all animations
}

void HolidayAnims::updateAll() {
    for (auto anim : _animations) {
        if (anim) anim->update();
    }
}

Animation* HolidayAnims::getAnimationById(int id) {
    if (id < 0 || id >= _animations.size()) return nullptr;
    return _animations[id];
}

void HolidayAnims::registerScenes() {
    // Register code scenes
}
