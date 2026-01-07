#pragma once
#include <Arduino.h>
#include <vector>
#include "MatrixDisplay.h"
#include "Animations.h"  // Fix: Animation declared

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    void beginAll();      // Initialize all animations
    void updateAll();     // Update all animations

    // Existing interface
    void addAnimation(Animation* anim);         // Add a static or dynamic animation
    Animation* getAnimationById(int id);        // Retrieve animation by index

private:
    MatrixDisplay* _display;

    // Fixed vector type
    std::vector<Animation*> _animations;
};
