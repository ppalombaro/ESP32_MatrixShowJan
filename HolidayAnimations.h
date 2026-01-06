/* HolidayAnimations.h
   Animated effects for all holidays
   VERSION: V14.0-2025-12-27T16:30:00Z - Multi-holiday support
*/

#ifndef HOLIDAY_ANIMATIONS_H
#define HOLIDAY_ANIMATIONS_H

#include "MatrixDisplay.h"
#include "Config.h"
#include "SceneData.h"
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <FFat.h>

class AnimationLoader {
public:
    AnimationLoader(MatrixDisplay* display);
    
    bool loadTimeline(const char* theme, const char* animName);
    void update();
    void begin();
    bool isPlaying();
    
private:
    MatrixDisplay* disp;
    
    struct Frame {
        String sceneId;
        int durationMs;
    };
    
    Frame frames[50];
    int frameCount;
    int currentFrame;
    unsigned long lastFrameTime;
    bool playing;
    bool loop;
    
    String basePath;
    SceneData* sceneData;
};

// ============== HALLOWEEN ANIMATIONS ==============

class FlyingBatsAnimation {
public:
    FlyingBatsAnimation(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    struct Bat {
        float x;
        int y;
        float speed;
        int direction;
    };
    static const int MAX_BATS = 5;
    Bat bats[MAX_BATS];
    unsigned long lastUpdate;
};

class PirateShipAnimation {
public:
    PirateShipAnimation(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    float shipX;
    int shipY;
    unsigned long lastUpdate;
    void drawShip(int globalX, int y);
};

class BleedingWindowsAnimation {
public:
    BleedingWindowsAnimation(MatrixDisplay* display);
    void begin();
    void update();
    
private:
    MatrixDisplay* disp;
    struct Window {
        int matrix;
        int x;
        int y;
        int bloodLevel;
    };
    static const int MAX_WINDOWS = 6;
    Window windows[MAX_WINDOWS];
    unsigned long lastUpdate;
};
 
class LightningFlashAnimation {
public:
    LightningFlashAnimation(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    unsigned long lastFlash;
    unsigned long flashDuration;
    bool isFlashing;
    uint8_t flashBrightness;
    unsigned long lastUpdate;
};

class HalloweenScrollText {
public:
    HalloweenScrollText(MatrixDisplay* display);
    void begin();
    void update();
    void setMessage(const char* msg);
private:
    MatrixDisplay* disp;
    char message[32];
    int scrollPos;
    unsigned long lastUpdate;
};

class SpookyTrackingEyesAnimation {
public:
    SpookyTrackingEyesAnimation(MatrixDisplay* display);
    void begin();
    void update();
    
private:
    MatrixDisplay* disp;
    unsigned long lastChange;
    unsigned long lastBlink;
    unsigned long nextBlinkTime;
    int frameIndex;
    bool isBlinking;
    int blinkState;
    uint8_t veinPulseBrightness;
    unsigned long lastVeinPulse;
    
    struct Frame {
        int direction;
        unsigned long duration;
        bool dilated;
    };
    static const int FRAME_COUNT = 14;
    Frame timeline[FRAME_COUNT];
    
    void processBlink();
    void updateVeinPulse();
    unsigned long getRandomDuration(unsigned long baseDuration);
    void drawDirection(int matrix, int direction, bool dilated, uint8_t veinPulse);
    void drawEye(int matrix, int pupilOffsetX, int pupilOffsetY, bool dilated, uint8_t veinPulse);
    void drawEyelid(int matrix, int lidLevel);
};

// ============== THANKSGIVING ANIMATIONS ==============

class ThanksgivingScrollText {
public:
    ThanksgivingScrollText(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    int scrollPos;
    unsigned long lastUpdate;
};

class FallingLeavesAnimation {
public:
    FallingLeavesAnimation(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    struct Leaf {
        float x;
        float y;
        float speed;
        CRGB color;
    };
    static const int MAX_LEAVES = 15;
    Leaf leaves[MAX_LEAVES];
    unsigned long lastUpdate;
};

// ============== NEW YEAR ANIMATIONS ==============

class NewYearCountdown {
public:
    NewYearCountdown(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    unsigned long lastUpdate;
    bool isNewYear;
    void drawCountdown(long days, int hours, int minutes, int seconds);
    void drawDigit(int matrix, int x_start, int y_start, int digit, CRGB color);
};

class NewYearScrollText {
public:
    NewYearScrollText(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    int scrollPos;
    unsigned long lastUpdate;
};

class FireworksAnimation {
public:
    FireworksAnimation(MatrixDisplay* display);
    void begin();
    void update();
    
private:
    MatrixDisplay* disp;
    struct Firework {
        int x;
        int y;
        int radius;
        CRGB color;
        bool active;
        unsigned long startTime;
    };
    static const int MAX_FIREWORKS = 5;
    Firework fireworks[MAX_FIREWORKS];
    unsigned long lastUpdate;
    void launchFirework();
};

class ColorSpinningStarAnimation {
public:
    ColorSpinningStarAnimation(MatrixDisplay* display);
    void begin();
    void update();
    
private:
    MatrixDisplay* disp;
    float rotation;
    uint8_t hue;
    unsigned long lastUpdate;
};

// ============== OHIO STATE ANIMATIONS ==============

class OSUScrollText {
public:
    OSUScrollText(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    int scrollPos;
    unsigned long lastUpdate;
};

class OHIOLetterAnimation {
public:
    OHIOLetterAnimation(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    int currentLetter;
    unsigned long lastLetterTime;
    bool showLetter;
    void drawLetter(char letter, int matrix);
};

class OSUColorWaveAnimation {
public:
    OSUColorWaveAnimation(MatrixDisplay* display);
    void begin();
    void update();
private:
    MatrixDisplay* disp;
    int wavePosition;
    unsigned long lastUpdate;
};

#endif // HOLIDAY_ANIMATIONS_H