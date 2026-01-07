#pragma once
#include <Arduino.h>
#include <vector>

 
#include "Animations.h"  
class MatrixDisplay;

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    void beginAll();
    void updateAll();
    Animation* getAnimationById(int id);
    void registerScenes();

private:
    MatrixDisplay* _display;
    std::vector<Animation*> _animations;
};
