#include "HolidayAnims.h"

HolidayAnims::HolidayAnims(MatrixDisplay* disp)
    : _display(disp) {}

void HolidayAnims::beginAll() {
    for (auto anim : _animations) {
        if (anim) anim->begin(_display);
    }
}

void HolidayAnims::updateAll() {
    for (auto anim : _animations) {
        if (anim) anim->update();
    }
}

void HolidayAnims::addAnimation(Animation* anim) {
    if (anim) _animations.push_back(anim);
}

Animation* HolidayAnims::getAnimationById(int id) {
    for (auto anim : _animations) {
        if (anim && anim->id == id) return anim;
    }
    return nullptr;
}
