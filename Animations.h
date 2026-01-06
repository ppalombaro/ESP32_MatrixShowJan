/* Animations.h
   Header file for animated effects
   VERSION: V15.0.1-2026-01-03T12:05:00Z - Added configurable countdown dates
   
   V15.0.1-2026-01-03T12:05:00Z - Added setTargetDate() for web configuration
   V15.0.1-2026-01-03T12:05:00Z - Countdown defaults to current year
   V12.0-2025-12-19T10:05:00Z - Removed LineAnimation class
*/

// Animations.h - V15.2.1-2026-01-04T14:00:00Z
// Add after other includes, before class declarations:

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>
#include "MatrixDisplay.h"
#include <NTPClient.h>

// V15.2.1-2026-01-04T14:00:00Z - Export font for use in HolidayAnimations
extern const uint8_t FONT_5X7[][5];

// Snowfall modes
enum SnowfallMode {
    MODE_BASIC = 0,
    MODE_WIND_SINGLE,
    MODE_WIND_VARY,
    MODE_ACCUMULATE,
    MODE_VARY_SIZE
};



// Snowflake structure
struct Snowflake {
    float x, y;
    float speed;
    float windSpeed;
    int size;
    int shape;
};

// SnowfallEffect class
class SnowfallEffect {
public:
    SnowfallEffect(MatrixDisplay* display);
    void begin();
    void setMode(SnowfallMode m);
    void update();

private:
    static const int MAX_FLAKES_PER_MATRIX = 30;
    MatrixDisplay* disp;
    SnowfallMode mode;
    // V15.0.2-2026-01-03T17:00:00Z - Share flakes array (both matrices show same pattern)
    // Saves 480 bytes RAM (was 2 arrays, now 1)
    Snowflake flakes[MAX_FLAKES_PER_MATRIX];
    int activeFlakes;
    unsigned long lastTime;
    // V15.0.2-2026-01-03T17:00:00Z - Share ground level (both matrices accumulate same)
    // Saves 40 bytes RAM (was 2 arrays, now 1)
    int groundLevel[20];

    void initFlakes(Snowflake* flakes, int count);
    void addFlake(int matrix);
    void drawFlake(const Snowflake& flake, int matrix, CRGB color);
};

// ChaseAnimation class
class ChaseAnimation {
public:
  ChaseAnimation(MatrixDisplay* display, CRGB c = CRGB::Red, int t = 3, int spd = 50);
  void begin();
  void update();
private:
  MatrixDisplay* disp;
  CRGB color;
  int thickness;
  int chaseSpeed;
  int position;
  int direction;
  int colorIndex;
  unsigned long lastUpdate;
};

// ScrollAnimation class
class ScrollAnimation {
public:
  ScrollAnimation(MatrixDisplay* display);
  void begin();
  void setMessage(const char* msg);
  void update();
private:
  MatrixDisplay* disp;
  char message[64];
  int scrollPos;
  int scrollSpeed;
  unsigned long lastUpdate;
};

// MerryChristmasCount class
// V15.0.1-2026-01-03T12:05:00Z - Enhanced with configurable target date
class MerryChristmasCount {
public:
  MerryChristmasCount(MatrixDisplay* display);
  void begin();
  void update();
  
  // V15.0.1-2026-01-03T12:05:00Z - Set custom target date (month, day)
  // Automatically uses current year, rolls to next year if date has passed
  void setTargetDate(int month, int day);
  
  // V15.0.1-2026-01-03T12:05:00Z - Get current target for web display
  void getTargetDate(int& month, int& day);
  
private:
  MatrixDisplay* disp;
  unsigned long lastUpdate;
  
  // V15.0.1-2026-01-03T12:05:00Z - Configurable target date
  int targetMonth = 12;  // Default: December
  int targetDay = 25;    // Default: 25th
  
  void drawCountdown(long days, int hours, int minutes, int seconds);
  void drawDigit(int matrix, int x_start, int y_start, int digit, CRGB color);
  time_t calculateTargetTime();  // V15.0.1-2026-01-03T12:05:00Z
};

// SpinningSnowflake class
class SpinningSnowflake {
public:
  SpinningSnowflake(MatrixDisplay* display);
  void begin();
  void update();
private:
  MatrixDisplay* disp;
  float rotation;
  int updateDelay = 100;
  unsigned long lastUpdate;
};

// SparklingStar class
class SparklingStar {
public:
  SparklingStar(MatrixDisplay* display);
  void begin();
  void update();
private:
  MatrixDisplay* disp;
  int updateDelay = 50;
  unsigned long lastUpdate;
};

// Concentric Geometry Base
class ConcentricBase {
protected:
  MatrixDisplay* disp;
  int ringIndex;
  int ringCount;
  int direction;
  unsigned long lastUpdate;
  int speed;

  CRGB ringColor(int ring) {
    switch (ring % 3) {
      case 0: return CRGB::Red;
      case 1: return CRGB::Green;
      default: return CRGB::White;
    }
  }

public:
  ConcentricBase(MatrixDisplay* d, int spd = 80)
    : disp(d), ringIndex(0), ringCount(0),
      direction(1), lastUpdate(0), speed(spd) {}

  virtual void begin() {
    ringIndex = 0;
    direction = 1;
    lastUpdate = millis();
  }

  virtual void drawRing(int matrix, int ring, CRGB color) = 0;

  void update() {
    unsigned long now = millis();
    if (now - lastUpdate < speed) return;
    lastUpdate = now;

    disp->clear();

    for (int matrix = 0; matrix < 2; matrix++) {
      drawRing(matrix, ringIndex, ringColor(ringIndex));
    }

    ringIndex += direction;
    if (ringIndex >= ringCount - 1 || ringIndex <= 0) {
      direction *= -1;
    }

    ringIndex = constrain(ringIndex, 0, ringCount - 1);

    disp->show();
  }
};

class ConcentricRectangles : public ConcentricBase {
public:
  ConcentricRectangles(MatrixDisplay* d) : ConcentricBase(d) {}
  void begin() override;
  void drawRing(int matrix, int ring, CRGB color) override;
};

class ConcentricCircles : public ConcentricBase {
public:
  ConcentricCircles(MatrixDisplay* d) : ConcentricBase(d) {}
  void begin() override;
  void drawRing(int matrix, int ring, CRGB color) override;
};

class ConcentricTriangles : public ConcentricBase {
public:
  ConcentricTriangles(MatrixDisplay* d) : ConcentricBase(d) {}
  void begin() override;
  void drawRing(int matrix, int ring, CRGB color) override;
};

enum MorphShape {
  MORPH_RECT = 0,
  MORPH_CIRCLE,
  MORPH_TRIANGLE
};

class MorphingGeometry {
public:
  MorphingGeometry(MatrixDisplay* display);
  void begin();
  void update();

private:
  MatrixDisplay* disp;
  MorphShape currentShape;
  int ringIndex;
  int ringCount;
  int direction;
  unsigned long lastUpdate;
  int speed;

  CRGB ringColor(int ring);
  void setupShape();
  void drawRectangle(int matrix, int r, CRGB c);
  void drawCircle(int matrix, int r, CRGB c);
  void drawTriangle(int matrix, int r, CRGB c);
};

#endif // ANIMATIONS_H
