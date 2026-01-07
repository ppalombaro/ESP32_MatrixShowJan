#pragma once
#include <Arduino.h>
#include <vector>
#include "MatrixDisplay.h"
#include "Animation.h" // Include the full Animation class

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    void beginAll();       // Initialize all animations
    void updateAll();      // Update all animations
    Animation* getAnimationById(int id);

    // Add animations to the container
    void addAnimation(Animation* anim) { _animations.push_back(anim); }

private:
    MatrixDisplay* _display;
    std::vector<Animation*> _animations;
};
