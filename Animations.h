#pragma once
#include <Arduino.h>
#include <MatrixDisplay.h>  // Keep existing dependency

// ----------------------------
// Base Animation Class
// ----------------------------
class Animation {
public:
    Animation(MatrixDisplay* disp) : _display(disp) {}
    virtual ~Animation() = default;

    // Existing virtual functions you may already have
    virtual void begin() {}   // Initialize animation
    virtual void update() {}  // Update frame

protected:
    MatrixDisplay* _display;
};

// ----------------------------
// Add any derived animations here if they exist
// Example from your existing code could remain
// ----------------------------
