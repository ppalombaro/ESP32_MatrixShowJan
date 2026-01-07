#pragma once

#include "Animations.h"      // Required: defines Animation
#include "MatrixDisplay.h"

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    // Initialize all animations
    void beginAll();

    // Update all animations (call in loop)
    void updateAll();

    // Access specific animations
    Animation* getAnimationById(int id);

    // Register scenes (used in HolidayAnimations.cpp)
    void registerScenes();

private:
    MatrixDisplay* _display = nullptr;

    // Example: store animations, may already exist in your original code
    std::vector<Animation*> _animations;

    // Any other private members from the original class should remain
};
