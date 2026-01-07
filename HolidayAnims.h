#pragma once

#include <vector>

// Forward declarations
class MatrixDisplay;
class Animation;

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    void beginAll();
    void updateAll();
    Animation* getAnimationById(int id);
    void registerScenes();

private:
    MatrixDisplay* _display = nullptr;
    std::vector<Animation*> _animations;
};
};
