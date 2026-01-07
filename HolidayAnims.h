#pragma once
#include <vector>
#include <Arduino.h>
#include "MatrixDisplay.h"
#include "Animations.h"

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    void beginAll();                    // Initialize all animations
    void updateAll();                   // Update all animations per loop
    void addAnimation(Animation* anim); // Add a static or dynamic animation
    Animation* getAnimationById(int id);

private:
    MatrixDisplay* _display;
    std::vector<Animation*> _animations;
};
