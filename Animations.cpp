/* Animations.cpp
   Implementation of animated effects
   VERSION: V12.0 - Removed LineAnimation
   V12.0-2025-12-19T10:45:00Z - REMOVED LineAnimation class implementation
   VERSION: V11.2.1 - Copied TestScenes implementation, fixed scroll matrix order
   
   V12.0-2025-12-19T10:45:00Z - Removed LineAnimation class - DEFECT 1 FIX: LineAnimation now uses exact TestScenes implementation
   V11.2.1-2025-12-18T15:10:00Z - DEFECT 2 FIX: Scroll matrix order fixed (1=right first, 0=left second)
   V11.2-2025-12-18T14:00:00Z - DEFECT 1 FIX: LineAnimation now calls update() in begin() for immediate display
   V11.2-2025-12-18T14:10:00Z - DEFECT 2 FIX: Scroll text enters from RIGHT, scrolls to LEFT (right matrix first)
   V11.0.1-2025-12-18T13:00:00Z - CRITICAL FIX: Use timeClient for synchronized time (was time(nullptr))
   V11.0-2025-12-18T12:38:00Z - DEFECT 1 FIX: Corrected countdown target timestamp (1766638800)
   V11.0-2025-12-18T12:10:00Z - DEFECT 2 FIX: Replaced LineAnimation with TestScenes color bands
   V11.0-2025-12-18T12:20:00Z - DEFECT 3 FIX: Ensured scroll text synchronization
   // V10.11-2024-12-14  - FIX Line annimation execution and where lables for countdown are drawn.
   V10.6.4-2025-12-15T00:30:00Z - CRITICAL FIX: Line animation boundary hang (position >= to >)
   V10.6.4-2025-12-15T00:30:00Z - FIX: Countdown date calculation (Dec 15, 2025 not 2024)
   V10.6.4-2025-12-15T00:30:00Z - FIX: Scroll text synchronization across dual matrices
   // V10.6-20241214-2315 - Added Christmas Day flashing behavior to countdown
   // V10.1-2025-12-14T16:00:00Z -
   Version 9.0 Re-added original Spinning snowflake and sparkling start
   
   VERSION: V6.8.4 - Fixed fadeAll causing invisible animations
   
   V6.8.4-2025-12-12T17:45:00Z - CRITICAL FIX: Changed fadeAll(240) to clear() in ChaseAnimation
   V6.8.4-2025-12-12T17:45:00Z - fadeAll was making display nearly invisible (only random blinks)
   V6.3.0 - Fixed SnowWhite redefinition conflict with Config.h
   V6.1-2025-12-11T22:00:00Z-BUG001 - Fixed countdown display (boxes, labels, positioning)
   V6.1-2025-12-11T22:00:00Z-BUG002 - Fixed chase animation diagonal movement
   
   Previous versions:
   V4.0 - 2025-12-10T10:55:00Z - Implemented two-matrix scoreboard countdown
   V3.3 - 2025-12-09T22:45:00Z - Fixed snow direction, accumulation, etc.
*/

#include "Animations.h"
#include <time.h>
#include <cstring>
#include <Arduino.h>
#include <pgmspace.h>  // V15.0.2-2026-01-03T18:00:00Z - Required for PROGMEM and pgm_read_byte
#include <NTPClient.h>  // V11.0.1-2025-12-18T13:01:00Z - CRITICAL: Include for timeClient access

// V11.0.1-2025-12-18T13:01:00Z - CRITICAL: Declare external NTP client for accurate time
extern NTPClient timeClient;

// V6.3.0-2025-12-12T14:30:00Z - FIX: Renamed from SnowWhite to avoid conflict with extern in Config.h
#define SNOW_COLOR CRGB(220, 240, 255)

// 3x5 Font Map (3 pixels wide, 5 pixels high)
const uint8_t DIGIT_3X5[][5] = {
  {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
  {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
  {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
  {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
  {0b101, 0b101, 0b111, 0b001, 0b001}, // 4 
  {0b111, 0b100, 0b111, 0b001, 0b111}, // 5 
  {0b111, 0b100, 0b111, 0b101, 0b111}, // 6 
  {0b111, 0b001, 0b001, 0b001, 0b001}, // 7
  {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
  {0b111, 0b101, 0b111, 0b001, 0b111}  // 9
};

// V9.2-2025-12-13T09:07:00Z - DEFECT #5 FIX: Simple 5x7 font for scrolling text
// Basic uppercase letters, numbers, and space (ASCII 32-90)
// V15.0.2-2026-01-03T17:00:00Z - Font moved to PROGMEM (saves ~300 bytes RAM)
const uint8_t FONT_5X7[][5] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, // space (32)
  {0x00, 0x00, 0x5F, 0x00, 0x00}, // ! (33)
  {0x00, 0x07, 0x00, 0x07, 0x00}, // " (34)
  {0x14, 0x7F, 0x14, 0x7F, 0x14}, // # (35)
  {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $ (36)
  {0x23, 0x13, 0x08, 0x64, 0x62}, // % (37)
  {0x36, 0x49, 0x55, 0x22, 0x50}, // & (38)
  {0x00, 0x05, 0x03, 0x00, 0x00}, // ' (39)
  {0x00, 0x1C, 0x22, 0x41, 0x00}, // ( (40)
  {0x00, 0x41, 0x22, 0x1C, 0x00}, // ) (41)
  {0x14, 0x08, 0x3E, 0x08, 0x14}, // * (42)
  {0x08, 0x08, 0x3E, 0x08, 0x08}, // + (43)
  {0x00, 0x50, 0x30, 0x00, 0x00}, // , (44)
  {0x08, 0x08, 0x08, 0x08, 0x08}, // - (45)
  {0x00, 0x60, 0x60, 0x00, 0x00}, // . (46)
  {0x20, 0x10, 0x08, 0x04, 0x02}, // / (47)
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0 (48)
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1 (49)
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 (50)
  {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3 (51)
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4 (52)
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 (53)
  {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6 (54)
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 (55)
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 (56)
  {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9 (57)
  {0x00, 0x36, 0x36, 0x00, 0x00}, // : (58)
  {0x00, 0x56, 0x36, 0x00, 0x00}, // ; (59)
  {0x08, 0x14, 0x22, 0x41, 0x00}, // < (60)
  {0x14, 0x14, 0x14, 0x14, 0x14}, // = (61)
  {0x00, 0x41, 0x22, 0x14, 0x08}, // > (62)
  {0x02, 0x01, 0x51, 0x09, 0x06}, // ? (63)
  {0x32, 0x49, 0x79, 0x41, 0x3E}, // @ (64)
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A (65)
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B (66)
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C (67)
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D (68)
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E (69)
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F (70)
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G (71)
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H (72)
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I (73)
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J (74)
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K (75)
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L (76)
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M (77)
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N (78)
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O (79)
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P (80)
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q (81)
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R (82)
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S (83)
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T (84)
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U (85)
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V (86)
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W (87)
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X (88)
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y (89)
  {0x61, 0x51, 0x49, 0x45, 0x43}  // Z (90)
};

// Helper function to initialize flakes
void SnowfallEffect::initFlakes(Snowflake* flakes, int count) {
    for (int i = 0; i < count; i++) {
        flakes[i].x = random(COLS);
        flakes[i].y = random(ROWS);
        flakes[i].speed = random(5, 15) / 10.0f; 
        flakes[i].windSpeed = random(-5, 5) / 10.0f;
        flakes[i].size = 1; 
        flakes[i].shape = 0;
    }
}

// --- SnowfallEffect Implementation ---

SnowfallEffect::SnowfallEffect(MatrixDisplay* display) : disp(display), mode(MODE_BASIC), activeFlakes(MAX_FLAKES_PER_MATRIX), lastTime(0) {
  // V15.0.2-2026-01-03T17:00:00Z - Single shared ground level array
  memset(groundLevel, 0, sizeof(groundLevel));
}

void SnowfallEffect::begin() {
    activeFlakes = 0; 
    lastTime = millis();
    randomSeed(analogRead(0));
    // V15.0.2-2026-01-03T17:00:00Z - Single shared ground level
    for (int i = 0; i < COLS; i++) {
        groundLevel[i] = 0;
    }
}

void SnowfallEffect::setMode(SnowfallMode m) {
    mode = m;
    disp->clear();  // V9.2-2025-12-13T09:05:00Z - DEFECT #3 FIX: Clear before mode change
    disp->show();   // V9.2-2025-12-13T09:05:00Z - Show clear display
    delay(50);      // V9.2-2025-12-13T09:05:00Z - Brief pause for clean transition
    begin();
}

void SnowfallEffect::addFlake(int matrix) {
    // V15.0.2-2026-01-03T17:00:00Z - Use shared flakes array (ignore matrix param)
    if (activeFlakes >= MAX_FLAKES_PER_MATRIX) return; 

    flakes[activeFlakes].x = random(COLS);
    flakes[activeFlakes].y = ROWS - 1; 
    flakes[activeFlakes].speed = random(4, 10) / 10.0;  // V6.9.2 - Slower flakes (0.4-0.9)
    flakes[activeFlakes].windSpeed = (mode == MODE_WIND_SINGLE || mode == MODE_WIND_VARY) 
    ? random(-15, 15) / 10.0  // -1.5 to +1.5 pixels/frame sideways
    : 0.0;
    

    if (mode == MODE_VARY_SIZE) {
        flakes[activeFlakes].size = random(1, 3);
        flakes[activeFlakes].shape = random(0, 2);
    } else {
        flakes[activeFlakes].size = 1;
        flakes[activeFlakes].shape = 0;
    }
    
    activeFlakes++;
}

void SnowfallEffect::drawFlake(const Snowflake& flake, int matrix, CRGB color) {
    int inverted_y = ROWS - 1 - (int)flake.y;
    if (flake.size > 1 && flake.shape == 1) {
        disp->setPixel(matrix, (int)flake.x, inverted_y, color);
    } else {
        disp->setPixel(matrix, (int)flake.x, inverted_y, color);
    }
}

void SnowfallEffect::update() {
    unsigned long now = millis();
    if (now - lastTime < 70) return;  // V6.9.2-2025-12-13T05:15:00Z - Fixed variable name (lastTime not lastUpdate)
    lastTime = now;  // V6.9.2-2025-12-13T05:15:00Z - Also corrected here

    if (activeFlakes < MAX_FLAKES_PER_MATRIX && random(100) < 30) {
        addFlake(0);
        addFlake(1);
    }

    // V9.2-2025-12-13T09:06:00Z - DEFECT #3 FIX: Clear to prevent color bleeding
    disp->clear();

    // V9.2-2025-12-13T09:06:00Z - Use pure white for snow (no other colors)
    CRGB pureWhite = CRGB(255, 255, 255);

    // V15.0.2-2026-01-03T17:00:00Z - Use single shared flakes array
    // Draw same snowfall pattern on both matrices
    for (int i = 0; i < activeFlakes; i++) {
        flakes[i].y -= flakes[i].speed;
        flakes[i].x += flakes[i].windSpeed;

        if (flakes[i].x < 0) flakes[i].x = 0;
        if (flakes[i].x >= COLS) flakes[i].x = COLS - 1;

        int xpos = (int)flakes[i].x;
        if (mode == MODE_ACCUMULATE) {
            if (flakes[i].y <= groundLevel[xpos]) {
                groundLevel[xpos]++;
                if (groundLevel[xpos] >= ROWS) groundLevel[xpos] = ROWS - 1;
                flakes[i].y = ROWS - 1;
                flakes[i].x = random(COLS);
            }
        } else {
            if (flakes[i].y < 0) {
                flakes[i].y = ROWS - 1;
                flakes[i].x = random(COLS);
            }
        }

        // V15.0.2-2026-01-03T17:00:00Z - Draw same flake to both matrices
        drawFlake(flakes[i], 0, pureWhite);
        drawFlake(flakes[i], 1, pureWhite);
    }

    // V15.0.2-2026-01-03T17:00:00Z - Shared ground accumulation for both matrices
    if (mode == MODE_ACCUMULATE) {
        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < groundLevel[x]; y++) {
                disp->setPixel(0, x, ROWS - 1 - y, pureWhite);
                disp->setPixel(1, x, ROWS - 1 - y, pureWhite);
            }
        }
    }

    disp->show();
}

// --- ChaseAnimation Implementation ---
// V6.1-2025-12-11T22:00:00Z-BUG002 - Fixed diagonal chase movement

ChaseAnimation::ChaseAnimation(MatrixDisplay* display, CRGB c, int t, int spd)
    : disp(display), color(c), thickness(t), chaseSpeed(spd), position(0), direction(1), lastUpdate(0) {}

void ChaseAnimation::begin() {
    position = 0;
    direction = 1;
    colorIndex = 0;  // V9.2-2025-12-13T09:01:00Z - Track which color to use
    lastUpdate = millis();
}

void ChaseAnimation::update() {
    unsigned long now = millis();
    if (now - lastUpdate < chaseSpeed) {
        return;
    }
    lastUpdate = now;
    
    // V9.4-2025-12-13T12:01:00Z - DEFECT #2 FIX: Three diagonal lines chasing each other
    disp->clear();
    
    // Color rotation: Red, Green, Blue, White
    CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Cyan, CRGB::White};
    
    // V9.4-2025-12-13T12:01:00Z - Draw THREE lines spaced apart
    int spacing = 8;  // Pixels between each line
    
    // Draw diagonal lines (top-left to bottom-right) on both matrices
    for (int matrix = 0; matrix < 2; matrix++) {
        for (int lineNum = 0; lineNum < 3; lineNum++) {  // V9.4-2025-12-13T12:01:00Z - Three lines
            int linePosition = position + (lineNum * spacing);
            CRGB lineColor = colors[(colorIndex + lineNum) % 4];  // V9.4-2025-12-13T12:01:00Z - Different color per line
            
            for (int offset = -ROWS; offset < COLS; offset++) {
                if (offset == linePosition) {
                    for (int i = 0; i < ROWS; i++) {
                        int x = offset + i;
                        int y = i;
                        if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
                            disp->setPixel(matrix, x, y, lineColor);
                        }
                    }
                }
            }
        }
    }
    
    // Move position and change color when bouncing
    position += direction;
    int maxPos = COLS - 1;
    int minPos = -(ROWS - 1);
    
    if (position >= maxPos) {
        position = maxPos;
        direction = -1;
        colorIndex++;  // Change color on bounce
    } else if (position <= minPos) {
        position = minPos;
        direction = 1;
        colorIndex++;  // Change color on bounce
    }
    
    disp->show();
}

// --- ScrollAnimation Implementation ---
// V12.0.1-2025-12-19T13:30:00Z - Fixed: Removed stray LineAnimation leftover code
// V12.0.3-2025-12-19T13:45:00Z - Added missing constructor and begin() method

ScrollAnimation::ScrollAnimation(MatrixDisplay* display) : disp(display), scrollPos(0), scrollSpeed(33), lastUpdate(0) {
    strcpy(message, "MERRY CHRISTMAS!");
}

void ScrollAnimation::begin() {
    scrollPos = COLS * 2;
    lastUpdate = millis();
}

void ScrollAnimation::setMessage(const char* msg) {
    strncpy(message, msg, sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';
}

void ScrollAnimation::update() {
    unsigned long now = millis();
    if (now - lastUpdate < scrollSpeed) return;
    lastUpdate = now;

    // V9.2-2025-12-13T09:08:00Z - DEFECT #5 FIX: Proper text scrolling with font
    disp->clear();
    
    int charSpacing = 6;  // 5 pixels width + 1 pixel gap
    int totalWidth = strlen(message) * charSpacing;
    
    // Draw each character
    for (size_t i = 0; i < strlen(message); i++) {
        char c = message[i];
        int charPos = scrollPos + (i * charSpacing);
        
        // V10.6.4-2025-12-15T00:30:00Z - CRITICAL FIX: Treat display as continuous 40-pixel wide canvas																										
        // Only draw if character is visible on either matrix
     if (charPos >= -charSpacing && charPos < COLS * 2) {
													
						   
								 
			
								  
						   
										
			 
			
            // Get font data for this character
            int fontIndex = -1;
            if (c >= 32 && c <= 90) {
                fontIndex = c - 32;
            }
            
            if (fontIndex >= 0) {
                // Draw the character using 5x7 font
                for (int col = 0; col < 5; col++) {
                    int globalX = charPos + col;  // V10.6.4: Global X position across both matrices
                    
                    // V10.6.4: Determine which matrix and local x position
                    int matrix = 0;
                    int x = globalX;
                    
                    if (globalX >= COLS) {
                        matrix = 1;
                        x = globalX - COLS;  // Convert to local coordinate
                    }
                    
                    // V10.6.4: Only draw if within valid range for this matrix
                    if (globalX >= 0 && globalX < COLS * 2 && x >= 0 && x < COLS) {

                        // V15.0.2-2026-01-03T17:00:00Z - Read from PROGMEM
                        uint8_t columnData = pgm_read_byte(&FONT_5X7[fontIndex][col]);
                        
                        // Draw vertical column of pixels
                        for (int row = 0; row < 7; row++) {
                            if (columnData & (1 << row)) {
                                int y = 9 + row;  // Center vertically (rows 9-15)
                                if (y < ROWS) {
                                    disp->setPixel(matrix, x, y, CRGB::Red);
                                }
                            }
                        }
                    }
					
	  
				
				  
			
			   
					
				  
				 
			 
			   
				   
		  
		 
		
	   
	  
				 
					 
                }
            }
        }
    }


    scrollPos--;
    if (scrollPos < -totalWidth) {
        scrollPos = COLS * 2;  // Start from right edge of right matrix
    }

    disp->show();
}


// --- MerryChristmasCount Implementation ---
// V6.1-2025-12-11T22:00:00Z-BUG001 - Fixed countdown display

MerryChristmasCount::MerryChristmasCount(MatrixDisplay* display) : disp(display), lastUpdate(0) {}

void MerryChristmasCount::begin() {
    lastUpdate = millis();
}

// V6.1-2025-12-11T22:00:00Z-BUG001 - Helper to draw single 3x5 digit
void MerryChristmasCount::drawDigit(int matrix, int x_start, int y_start, int digit, CRGB color) {
    if (digit < 0 || digit > 9) return;
    
    for (int y = 0; y < 5; y++) {
        uint8_t row_pattern = DIGIT_3X5[digit][y];
        for (int x = 0; x < 3; x++) {
            if (row_pattern & (0b100 >> x)) {
                disp->setPixel(matrix, x_start + x, y_start + y, color);
            }
        }
    }
}

// V6.1-2025-12-11T22:00:00Z-BUG001 - Fixed countdown for 20x25 matrix
void MerryChristmasCount::drawCountdown(long days, int hours, int minutes, int seconds) {
    const int BOX_WIDTH = 9;   // V6.1-BUG001: Wider boxes
    const int BOX_HEIGHT = 10; // V6.1-BUG001: Taller boxes
    const int GAP = 1;
    
    const int DIGIT_W = 3;
    const int DIGIT_H = 5;
    
    const int Y_START = 8; // V6.1-BUG001: Start boxes lower
    
    const int X_LEFT = GAP;
    const int X_RIGHT = COLS / 2 + GAP;
    
    // V6.1-BUG001: Border drawing helper
    auto drawRectBorder = [&](int matrix, int x1, int y1, int x2, int y2, CRGB color) {
        for (int x = x1; x <= x2; x++) {
            disp->setPixel(matrix, x, y1, color);
            disp->setPixel(matrix, x, y2, color);
        }
        for (int y = y1 + 1; y <= y2 - 1; y++) {
            disp->setPixel(matrix, x1, y, color);
            disp->setPixel(matrix, x2, y, color);
        }
    };
    
    // V6.1-BUG001: Label drawing helper (D, H, M, S)
    auto drawLabel = [&](int matrix, int x_center, int y, char label, CRGB color) {
        int x = x_center - 1;
        
        switch(label) {
            case 'D':
                disp->setPixel(matrix, x, y, color);
                disp->setPixel(matrix, x, y+1, color);
                disp->setPixel(matrix, x, y+2, color);
                disp->setPixel(matrix, x, y+3, color);
                disp->setPixel(matrix, x, y+4, color);
                disp->setPixel(matrix, x+1, y, color);
                disp->setPixel(matrix, x+2, y+1, color);
                disp->setPixel(matrix, x+2, y+2, color);
                disp->setPixel(matrix, x+2, y+3, color);
                disp->setPixel(matrix, x+1, y+4, color);
                break;
                
            case 'H':
                disp->setPixel(matrix, x, y, color);
                disp->setPixel(matrix, x, y+1, color);
                disp->setPixel(matrix, x, y+2, color);
                disp->setPixel(matrix, x, y+3, color);
                disp->setPixel(matrix, x, y+4, color);
                disp->setPixel(matrix, x+1, y+2, color);
                disp->setPixel(matrix, x+2, y, color);
                disp->setPixel(matrix, x+2, y+1, color);
                disp->setPixel(matrix, x+2, y+2, color);
                disp->setPixel(matrix, x+2, y+3, color);
                disp->setPixel(matrix, x+2, y+4, color);
                break;
                
            case 'M':
                disp->setPixel(matrix, x, y, color);
                disp->setPixel(matrix, x, y+1, color);
                disp->setPixel(matrix, x, y+2, color);
                disp->setPixel(matrix, x, y+3, color);
                disp->setPixel(matrix, x, y+4, color);
                disp->setPixel(matrix, x+1, y+1, color);
                disp->setPixel(matrix, x+2, y, color);
                disp->setPixel(matrix, x+2, y+1, color);
                disp->setPixel(matrix, x+2, y+2, color);
                disp->setPixel(matrix, x+2, y+3, color);
                disp->setPixel(matrix, x+2, y+4, color);
                break;
                
            case 'S':
                disp->setPixel(matrix, x, y, color);
                disp->setPixel(matrix, x+1, y, color);
                disp->setPixel(matrix, x+2, y, color);
                disp->setPixel(matrix, x, y+1, color);
                disp->setPixel(matrix, x, y+2, color);
                disp->setPixel(matrix, x+1, y+2, color);
                disp->setPixel(matrix, x+2, y+2, color);
                disp->setPixel(matrix, x+2, y+3, color);
                disp->setPixel(matrix, x, y+4, color);
                disp->setPixel(matrix, x+1, y+4, color);
                disp->setPixel(matrix, x+2, y+4, color);
                break;
        }
    };

    // V10.2-2025-12-14T16:10:00Z - FIX2: Complete box drawing with labels ABOVE boxes
    auto drawBox = [&](int matrix, int x, int y, char label, long value) {
        drawRectBorder(matrix, x, y, x + BOX_WIDTH - 1, y + BOX_HEIGHT - 1, CRGB::Red);
        
        // V10.11-2024-12-14T23:02:00Z - FIX: Draw label 4 rows above box (was y-2, now y-4)
        int label_x = x + (BOX_WIDTH / 2);
        int label_y = y - 6;  // V10.11: Moved from -2 to -6 for clear separation   12/18
        drawLabel(matrix, label_x, label_y, label, CRGB::Green);
        
        // Hardcode digits_y to absolute row 10
        int digits_y = 10;
        int digit1_x = x + 1;
        int digit2_x = x + 1 + DIGIT_W + 1;
        
        long value_clamped = constrain(value, 0, 99);
        int value1 = (value_clamped / 10) % 10;
        int value2 = value_clamped % 10;
        
        drawDigit(matrix, digit1_x, digits_y, value1, CRGB::White);
        drawDigit(matrix, digit2_x, digits_y, value2, CRGB::White);
    };

    // V6.1-BUG001: Draw all four boxes
    drawBox(0, X_LEFT, Y_START, 'M', minutes);
    drawBox(0, X_RIGHT, Y_START, 'S', seconds);
    drawBox(1, X_LEFT, Y_START, 'D', days);
    drawBox(1, X_RIGHT, Y_START, 'H', hours);
}

void MerryChristmasCount::update() {
    unsigned long now = millis();

    static unsigned long lastFlash = 0;
    static bool flashState = false;
    bool isChristmasDay = false;

    /* -------------------------------
       V11.0.1-2025-12-18T13:02:00Z - CRITICAL FIX
       GET CURRENT TIME FROM NTP CLIENT (synchronized UTC time)
       DO NOT USE time(nullptr) - it's not automatically synced by NTPClient
    -------------------------------- */
    timeClient.update();  // V11.0.1: Update NTP time
    time_t currentTime = timeClient.getEpochTime();  // V11.0.1: Use timeClient, not time(nullptr)

    // V11.0.1-2025-12-18T13:02:00Z - Fallback if NTP not synced (epoch time < Jan 2, 1970)
    if (currentTime < 100000) {
        // Use millis as rough fallback (will be inaccurate but won't crash)
        currentTime = (millis() / 1000);
    }

    /* -------------------------------
       CHECK FOR CHRISTMAS DAY (UTC)
       V11.0-2025-12-18T12:00:00Z - Check in UTC to match countdown logic
    -------------------------------- */
    struct tm utcTime;
    gmtime_r(&currentTime, &utcTime);  // V11.0: Use gmtime_r for UTC, not localtime_r

    // V11.0-2025-12-18T12:00:00Z - Check Christmas Day in UTC (Dec 25 at 05:00 UTC = midnight EST)
    if (utcTime.tm_mon == 11 && utcTime.tm_mday == 25 && utcTime.tm_hour >= 5) {
        isChristmasDay = true;
    }

    /* -------------------------------
       TARGET: Dec 25, 2025 05:00:00 UTC (midnight EST)
       V11.0-2025-12-18T12:00:00Z - CORRECTED timestamp
    -------------------------------- */
    const time_t targetTimeUTC = 1766638800;  // V11.0: Fixed from 1766602800 to 1766638800

    long diff = targetTimeUTC - currentTime;
    if (diff < 0) diff = 0;

    long days    = diff / 86400;
    long hours   = (diff % 86400) / 3600;
    long minutes = (diff % 3600) / 60;
    long seconds = diff % 60;

    /* -------------------------------
       CHRISTMAS DAY FLASHING
    -------------------------------- */
    if (isChristmasDay) {
        if (now - lastFlash >= 500) {
            flashState = !flashState;
            lastFlash = now;

            disp->clear();
            if (flashState) {
                drawCountdown(0, 0, 0, 0);
            }
            disp->show();
        }
        return;
    }

    /* -------------------------------
       NORMAL COUNTDOWN
    -------------------------------- */
    if (now - lastUpdate < 1000) return;
    lastUpdate = now;

    disp->clear();
    drawCountdown(days, hours, minutes, seconds);
    disp->show();
}


// ========== SpinningSnowflake ==========
// v2.1: Added complete implementation
SpinningSnowflake::SpinningSnowflake(MatrixDisplay* display) : disp(display), rotation(0), lastUpdate(0) {}

void SpinningSnowflake::begin() {
  rotation = 0;
  lastUpdate = millis();
}

void SpinningSnowflake::update() {
  unsigned long now = millis();
  if (now - lastUpdate > updateDelay) {
    rotation += 0.1f;
    if (rotation >= 6.28f) rotation = 0;
    lastUpdate = now;
  }
  
  disp->clear();
  
  int cx = COLS / 2;
  int cy = ROWS / 2;
  
  // Draw rotating snowflake on both matrices
  for (int matrix = 0; matrix < 2; matrix++) {
    // Center point
    disp->setPixel(matrix, cx, cy, CRGB::White);
    
    // Draw arms with rotation
    for (int i = 0; i < 6; i++) {
      float angle = rotation + (i * 1.047f); // 60 degrees apart
      for (int r = 1; r <= 8; r++) {
        int x = cx + (int)(r * cos(angle));
        int y = cy + (int)(r * sin(angle));
        if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
          disp->setPixel(matrix, x, y, CRGB::White);
        }
      }
    }
  }
  
  disp->show();
}

// ========== SparklingStar ==========
// v2.1: Added complete implementation
SparklingStar::SparklingStar(MatrixDisplay* display) : disp(display), lastUpdate(0) {}

void SparklingStar::begin() {
  lastUpdate = millis();
}

void SparklingStar::update() {
  unsigned long now = millis();
  if (now - lastUpdate > updateDelay) {
    lastUpdate = now;
  }
  
  disp->clear();
  
  // Draw a star shape with random sparkles
  int cx = COLS / 2;
  int cy = ROWS / 2;
  
  for (int matrix = 0; matrix < 2; matrix++) {
    // Main star shape
    for (int i = -8; i <= 8; i++) {
      // Horizontal line
      if (cx + i >= 0 && cx + i < COLS) {
        disp->setPixel(matrix, cx + i, cy, CRGB::Yellow);
      }
      // Vertical line
      if (cy + i >= 0 && cy + i < ROWS) {
        disp->setPixel(matrix, cx, cy + i, CRGB::Yellow);
      }
    }
    
    // Diagonal lines
    for (int i = -6; i <= 6; i++) {
      if (cx + i >= 0 && cx + i < COLS && cy + i >= 0 && cy + i < ROWS) {
        disp->setPixel(matrix, cx + i, cy + i, CRGB::Yellow);
        disp->setPixel(matrix, cx + i, cy - i, CRGB::Yellow);
      }
    }
    
    // Random sparkles
    for (int i = 0; i < 20; i++) {
      int x = random(COLS);
      int y = random(ROWS);
      if (random(2)) {
        disp->setPixel(matrix, x, y, CRGB::White);
      }
    }
  }
  
  disp->show();
}

// ========== ConcentricRectangles ==========
// V9.1-2025-12-13T08:02:00Z - Added complete implementation
// ========== ConcentricRectangles ==========
// V9.1-2025-12-13T08:02:00Z - Added complete implementation
void ConcentricRectangles::begin() {
  ringCount = min(COLS, ROWS) / 2;
  ConcentricBase::begin();
}

void ConcentricRectangles::drawRing(int matrix, int r, CRGB c) {
  int x1 = r;
  int y1 = r;
  int x2 = COLS - 1 - r;
  int y2 = ROWS - 1 - r;

  for (int x = x1; x <= x2; x++) {
    disp->setPixel(matrix, x, y1, c);
    disp->setPixel(matrix, x, y2, c);
  }

  for (int y = y1; y <= y2; y++) {
    disp->setPixel(matrix, x1, y, c);
    disp->setPixel(matrix, x2, y, c);
  }
}

// ========== ConcentricCircles ==========
// V9.1-2025-12-13T08:03:00Z - Added complete implementation
// ========== ConcentricCircles ==========
// V9.1-2025-12-13T08:03:00Z - Added complete implementation
void ConcentricCircles::begin() {
  ringCount = min(COLS, ROWS) / 2;
  ConcentricBase::begin();
}

void ConcentricCircles::drawRing(int matrix, int r, CRGB c) {
  int cx = COLS / 2;
  int cy = ROWS / 2;
  int radius = r;

  // V9.4-2025-12-13T12:04:00Z - DEFECT #5 FIX: Smaller angle step (4 instead of 8) for complete circle
  for (int angle = 0; angle < 360; angle += 4) {  // V9.4: Changed from 8 to 4 for smoother circle
    float rad = angle * DEG_TO_RAD;
    int x = cx + (int)(cos(rad) * radius);
    int y = cy + (int)(sin(rad) * radius);

    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
      disp->setPixel(matrix, x, y, c);
    }
  }
}

// ========== ConcentricTriangles ==========
// V9.1-2025-12-13T08:04:00Z - Added complete implementation
// ========== ConcentricTriangles ==========
// V9.1-2025-12-13T08:04:00Z - Added complete implementation
void ConcentricTriangles::begin() {
  ringCount = ROWS / 2;
  ConcentricBase::begin();
}

void ConcentricTriangles::drawRing(int matrix, int r, CRGB c) {
  int cx = COLS / 2;
  int top = r;
  int bottom = ROWS - 1 - r;

  // V9.4-2025-12-13T12:03:00Z - Draw triangle sides
  for (int y = top; y <= bottom; y++) {
    int halfWidth = y - top;
    int x1 = cx - halfWidth;
    int x2 = cx + halfWidth;

    if (x1 >= 0 && x1 < COLS) {
      disp->setPixel(matrix, x1, y, c);
    }
    if (x2 >= 0 && x2 < COLS) {
      disp->setPixel(matrix, x2, y, c);
    }
  }
  
  // V9.4-2025-12-13T12:03:00Z - DEFECT #4 FIX: Draw base (horizontal line at bottom)
  int baseWidth = bottom - top;
  for (int x = cx - baseWidth; x <= cx + baseWidth; x++) {
    if (x >= 0 && x < COLS) {
      disp->setPixel(matrix, x, bottom, c);
    }
  }
}

// ========== MorphingGeometry ==========
// V9.1-2025-12-13T08:05:00Z - Added complete implementation
MorphingGeometry::MorphingGeometry(MatrixDisplay* display)
  : disp(display), currentShape(MORPH_RECT),
    ringIndex(0), ringCount(0), direction(1),
    lastUpdate(0), speed(80) {}

void MorphingGeometry::begin() {
  ringIndex = 0;
  direction = 1;
  currentShape = MORPH_RECT;
  setupShape();
  lastUpdate = millis();
}

void MorphingGeometry::setupShape() {
  switch(currentShape) {
    case MORPH_RECT:
      ringCount = min(COLS, ROWS) / 2;
      break;
    case MORPH_CIRCLE:
      ringCount = min(COLS, ROWS) / 2;
      break;
    case MORPH_TRIANGLE:
      ringCount = ROWS / 2;
      break;
  }
}

CRGB MorphingGeometry::ringColor(int ring) {
  switch (ring % 3) {
    case 0: return CRGB::Red;
    case 1: return CRGB::Green;
    default: return CRGB::White;
  }
}

void MorphingGeometry::update() {
  unsigned long now = millis();
  if (now - lastUpdate < speed) return;
  lastUpdate = now;

  disp->clear();

  for (int matrix = 0; matrix < 2; matrix++) {
    switch(currentShape) {
      case MORPH_RECT:
        drawRectangle(matrix, ringIndex, ringColor(ringIndex));
        break;
      case MORPH_CIRCLE:
        drawCircle(matrix, ringIndex, ringColor(ringIndex));
        break;
      case MORPH_TRIANGLE:
        drawTriangle(matrix, ringIndex, ringColor(ringIndex));
        break;
    }
  }

  ringIndex += direction;
  
  // Check if we need to morph to next shape
  if (ringIndex >= ringCount - 1) {
    ringIndex = ringCount - 1;
    direction = -1;
  } else if (ringIndex <= 0) {
    ringIndex = 0;
    direction = 1;
    // Morph to next shape
    currentShape = (MorphShape)((currentShape + 1) % 3);
    setupShape();
  }

  ringIndex = constrain(ringIndex, 0, ringCount - 1);

  disp->show();
}

void MorphingGeometry::drawRectangle(int matrix, int r, CRGB c) {
  int x1 = r;
  int y1 = r;
  int x2 = COLS - 1 - r;
  int y2 = ROWS - 1 - r;

  for (int x = x1; x <= x2; x++) {
    disp->setPixel(matrix, x, y1, c);
    disp->setPixel(matrix, x, y2, c);
  }

  for (int y = y1; y <= y2; y++) {
    disp->setPixel(matrix, x1, y, c);
    disp->setPixel(matrix, x2, y, c);
  }
}

void MorphingGeometry::drawCircle(int matrix, int r, CRGB c) {
  int cx = COLS / 2;
  int cy = ROWS / 2;

  // V9.4-2025-12-13T12:05:00Z - DEFECT #5 FIX: Smaller angle step for complete circle
  for (int angle = 0; angle < 360; angle += 4) {  // V9.4: Changed from 8 to 4
    float rad = angle * DEG_TO_RAD;
    int x = cx + (int)(cos(rad) * r);
    int y = cy + (int)(sin(rad) * r);

    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
      disp->setPixel(matrix, x, y, c);
    }
  }
}

void MorphingGeometry::drawTriangle(int matrix, int r, CRGB c) {
  int cx = COLS / 2;
  int top = r;
  int bottom = ROWS - 1 - r;

  // V9.4-2025-12-13T12:06:00Z - Draw triangle sides
  for (int y = top; y <= bottom; y++) {
    int halfWidth = y - top;
    int x1 = cx - halfWidth;
    int x2 = cx + halfWidth;

    if (x1 >= 0 && x1 < COLS) {
      disp->setPixel(matrix, x1, y, c);
    }
    if (x2 >= 0 && x2 < COLS) {
      disp->setPixel(matrix, x2, y, c);
    }
  }
  
  // V9.4-2025-12-13T12:06:00Z - DEFECT #4 FIX: Add base line
  int baseWidth = bottom - top;
  for (int x = cx - baseWidth; x <= cx + baseWidth; x++) {
    if (x >= 0 && x < COLS) {
      disp->setPixel(matrix, x, bottom, c);
    }
  }
}