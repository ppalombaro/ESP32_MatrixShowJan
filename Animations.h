#pragma once

#include <Arduino.h>
#include "MatrixDisplay.h"   // FIX: local project header must use quotes

// Base class for all animations
class Animation {
public:
    explicit Animation(MatrixDisplay* display)
        : matrix(display) {}

    virtual ~Animation() = default;

    // Called once when animation starts
    virtual void begin() {}

    // Called repeatedly from loop()
    virtual void update() = 0;

protected:
    MatrixDisplay* matrix;
};
