#include "HolidayAnims.h"
#include "Logger.h"   // optional if needed

HolidayAnims::HolidayAnims(MatrixDisplay* disp) : _display(disp) {
    // constructor logic if any
}

// Initialize all animations
void HolidayAnims::beginAll() {
    for (auto anim : _animations) {
        if (anim) anim->begin(); // assuming Animation has begin()
    }
}

// Update all animations
void HolidayAnims::updateAll() {
    for (auto anim : _animations) {
        if (anim) anim->update(); // assuming Animation has update()
    }
}

// Access animation by ID
Animation* HolidayAnims::getAnimationById(int id) {
    for (auto anim : _animations) {
        if (anim && anim->getId() == id) {  // assuming Animation has getId()
            return anim;
        }
    }
    return nullptr;
}

// Register scenes (stub or original logic)
void HolidayAnims::registerScenes() {
    // existing scene registration logic should remain
    // this was missing in the header before
}
