/* TestScenes.cpp
   Implementation of diagnostic test patterns
   VERSION: V15.0.1-2026-01-03T11:05:00Z - Added matrix ID and megatree tests
   
   V15.0.1-2026-01-03T11:05:00Z - CORRECTED: Matrix 3 is megamatrix, not megatree
   V15.0.1-2026-01-03T11:05:00Z - Added drawMatrixID() for all 3 matrices
   V15.0.1-2026-01-03T11:05:00Z - Added updateMegatreeTest() for branch verification
   V12.5.3-2025-12-19T16:01:00Z - Fixed scrollOffset integer overflow bug
*/

#include "TestScenes.h"

TestScenes::TestScenes(MatrixDisplay* display) : disp(display), megatreeRow(0), megatreeBlinkState(0), lastMegatreeUpdate(0) {}

// Helper function to cycle through test colors
CRGB TestScenes::getTestColor(int index) {
  CRGB colors[] = {
    CRGB::Red,
    CRGB::Yellow,
    CRGB::Green,
    CRGB::Blue,
    CRGB(148, 0, 211),  // Violet
    CRGB::White
  };
  return colors[index % 6];
}

// V15.0.1-2026-01-03T11:05:00Z - Update scroll state with overflow protection
void TestScenes::updateScrollState() {
    unsigned long currentTime = millis();
    if (currentTime - lastScrollTime >= 100) {
        scrollOffset++;
        scrollOffset = scrollOffset % 24;  // Wrap at pattern cycle
        lastScrollTime = currentTime;
    }
}

// Test 1: Horizontal color bars (rows) - moves up/down
void TestScenes::drawColorRows(int matrix) {
  disp->clearMatrix(matrix);
  updateScrollState();
  
  for (int y = 0; y < ROWS; y++) {
    CRGB color = getTestColor((y + scrollOffset) / 4);
    for (int x = 0; x < COLS; x++) {
      disp->setPixel(matrix, x, y, color);
    }
  }
}

// Test 2: Vertical color bars (columns) - moves right/left
void TestScenes::drawColorColumns(int matrix) {
  disp->clearMatrix(matrix);
  updateScrollState();

  for (int x = 0; x < COLS; x++) {
    CRGB color = getTestColor((x + scrollOffset) / 3);
    for (int y = 0; y < ROWS; y++) {
      disp->setPixel(matrix, x, y, color);
    }
  }
}

// Test 3: Draw all white
void TestScenes::drawAllWhite(int matrix) {
  disp->clearMatrix(matrix);
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      disp->setPixel(matrix, x, y, CRGB::White);
    }
  }
}

// Test 4: Draw all red
void TestScenes::drawAllRed(int matrix) {
  disp->clearMatrix(matrix);
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      disp->setPixel(matrix, x, y, CRGB::Red);
    }
  }
}

// Test 5: Corner test
void TestScenes::drawCornerTest(int matrix) {
  disp->clearMatrix(matrix);
  
  // Top-left: RED
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      disp->setPixel(matrix, i, j, CRGB::Red);
    }
  }
  
  // Top-right: GREEN
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      disp->setPixel(matrix, COLS - 1 - i, j, CRGB::Green);
    }
  }

  // Bottom-left: BLUE
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      disp->setPixel(matrix, i, ROWS - 1 - j, CRGB::Blue);
    }
  }
  
  // Bottom-right: YELLOW
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      disp->setPixel(matrix, COLS - 1 - i, ROWS - 1 - j, CRGB::Yellow);
    }
  }
  
  // Center: WHITE (cross pattern)
  int cx = COLS / 2;
  int cy = ROWS / 2;
  for (int i = -3; i <= 3; i++) {
    disp->setPixel(matrix, cx + i, cy, CRGB::White);
  }
  for (int j = -3; j <= 3; j++) {
    disp->setPixel(matrix, cx, cy + j, CRGB::White);
  }
}

// Test 6: Grid pattern (every 5th pixel)
void TestScenes::drawGridTest(int matrix) {
  disp->clearMatrix(matrix);
  
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      if (x % 5 == 0 || y % 5 == 0) {
        disp->setPixel(matrix, x, y, CRGB::White);
      }
    }
  }
  
  // Add colored marker at bottom-left corner for orientation
  disp->setPixel(matrix, 0, 0, CRGB::Red);
  disp->setPixel(matrix, 1, 0, CRGB::Red);
  disp->setPixel(matrix, 0, 1, CRGB::Red);
}

// V15.0.1-2026-01-03T11:05:00Z - Test 7: Matrix Identification
// Displays large number (1, 2, or 3) to identify physical matrix
// Matrix 1 = Far Right (existing right matrix)
// Matrix 2 = Middle (existing left matrix)
// Matrix 3 = Far Left (future megamatrix)
void TestScenes::drawMatrixID(int matrix) {
  disp->clearMatrix(matrix);
  
  // Matrix numbering (left to right when viewing from front):
  // matrix=0 (GPIO 18) â†’ Display "2" (middle position)
  // matrix=1 (GPIO 19) â†’ Display "1" (far right position)
  // matrix=2 (GPIO 5)  â†’ Display "3" (far left position - future megamatrix)
  
  int displayNumber;
  if (matrix == 0) {
    displayNumber = 2;  // Middle matrix
  } else if (matrix == 1) {
    displayNumber = 1;  // Far right matrix
  } else {
    displayNumber = 3;  // Far left megamatrix (future)
  }
  
  drawLargeNumber(matrix, displayNumber);
}

// V15.0.1-2026-01-03T11:05:00Z - Helper: Draw large centered number
void TestScenes::drawLargeNumber(int matrix, int number) {
  // Numbers are 15 pixels wide Ã— 20 pixels tall
  // Centered on 25Ã—20 matrix (or larger for megamatrix)
  
  int startX = (COLS - 15) / 2;  // Center horizontally
  int startY = (ROWS - 20) / 2;  // Center vertically
  
  CRGB color = CRGB::White;
  
  // Number patterns [number][row][col]
  bool pattern[3][20][15];
  memset(pattern, 0, sizeof(pattern));
  
  // ===== NUMBER 1 PATTERN =====
  for (int y = 0; y < 20; y++) {
    pattern[0][y][7] = true;  // Vertical center line
    if (y < 3) {
      pattern[0][y][5] = true;  // Top diagonal
      pattern[0][y][6] = true;
    }
    if (y >= 18) {
      for (int x = 3; x < 12; x++) {  // Bottom base
        pattern[0][y][x] = true;
      }
    }
  }
  
  // ===== NUMBER 2 PATTERN =====
  for (int y = 0; y < 20; y++) {
    // Top curve (rows 0-2)
    if (y < 3) {
      for (int x = 3; x < 12; x++) pattern[1][y][x] = true;
    }
    // Right side (rows 3-7)
    if (y >= 3 && y < 8) {
      pattern[1][y][11] = true;
      pattern[1][y][12] = true;
    }
    // Middle diagonal (rows 8-14)
    if (y >= 8 && y < 15) {
      int xPos = 12 - (y - 8);
      pattern[1][y][xPos] = true;
      pattern[1][y][xPos-1] = true;
    }
    // Left bottom (rows 15-17)
    if (y >= 15 && y < 18) {
      pattern[1][y][2] = true;
      pattern[1][y][3] = true;
    }
    // Bottom line (rows 18-19)
    if (y >= 18) {
      for (int x = 2; x < 13; x++) pattern[1][y][x] = true;
    }
  }
  
  // ===== NUMBER 3 PATTERN =====
  for (int y = 0; y < 20; y++) {
    // Top curve (rows 0-2)
    if (y < 3) {
      for (int x = 3; x < 12; x++) pattern[2][y][x] = true;
    }
    // Right side - all rows
    if (y >= 3 && y < 18) {
      pattern[2][y][11] = true;
      pattern[2][y][12] = true;
    }
    // Middle bar (rows 9-10)
    if (y >= 9 && y < 11) {
      for (int x = 5; x < 12; x++) pattern[2][y][x] = true;
    }
    // Bottom curve (rows 18-19)
    if (y >= 18) {
      for (int x = 3; x < 12; x++) pattern[2][y][x] = true;
    }
  }
  
  // Draw the selected number
  if (number >= 1 && number <= 3) {
    for (int y = 0; y < 20; y++) {
      for (int x = 0; x < 15; x++) {
        if (pattern[number-1][y][x]) {
          int drawX = startX + x;
          int drawY = startY + y;
          if (drawX >= 0 && drawX < COLS && drawY >= 0 && drawY < ROWS) {
            disp->setPixel(matrix, drawX, drawY, color);
          }
        }
      }
    }
  }
}

// V15.0.1-2026-01-03T11:05:00Z - Megatree Branch Test Animation
// Tests megatree ONLY (separate from LED matrices)
// Each branch shows different color: Red, Yellow, Green, Blue, Purple, Red (repeat)
// Blinks from top to bottom (one row at a time)
void TestScenes::updateMegatreeTest() {
  #if ENABLE_MEGATREE
  unsigned long now = millis();
  
  // Update every 50ms (20 FPS for smooth animation)
  if (now - lastMegatreeUpdate < 50) return;
  lastMegatreeUpdate = now;
  
  // Get direct access to LED array
  CRGB* leds = disp->getLeds();
  int megatreeStart = TOTAL_LEDS;  // Megatree starts after all matrices
  
  // Define branch colors (6 branches, left to right)
  CRGB branchColors[MEGATREE_BRANCHES] = {
    CRGB::Red,     // Branch 0 (leftmost)
    CRGB::Yellow,  // Branch 1
    CRGB::Green,   // Branch 2
    CRGB::Blue,    // Branch 3
    CRGB::Purple,  // Branch 4
    CRGB::Red      // Branch 5 (rightmost) - repeat red if 6 branches
  };
  
  // Calculate LEDs per branch and per row
  int ledsPerBranch = MEGATREE_LEDS / MEGATREE_BRANCHES;
  int ledsPerRow = ledsPerBranch / MEGATREE_HEIGHT;
  
  // Clear entire megatree
  for (int i = 0; i < MEGATREE_LEDS; i++) {
    leds[megatreeStart + i] = CRGB::Black;
  }
  
  // If blink state is ON, show current row on all branches
  if (megatreeBlinkState == 1) {
    for (int branch = 0; branch < MEGATREE_BRANCHES; branch++) {
      int branchStart = branch * ledsPerBranch;
      int rowStart = branchStart + (megatreeRow * ledsPerRow);
      
      // Light up this row in this branch's color
      for (int i = 0; i < ledsPerRow; i++) {
        int ledIndex = megatreeStart + rowStart + i;
        if (ledIndex < megatreeStart + MEGATREE_LEDS) {
          leds[ledIndex] = branchColors[branch];
        }
      }
    }
  }
  
  // Toggle blink state every 500ms (1 Hz flash)
  static unsigned long lastToggle = 0;
  if (now - lastToggle > 500) {
    lastToggle = now;
    megatreeBlinkState = 1 - megatreeBlinkState;
    
    // When turning off, advance to next row (top to bottom)
    if (megatreeBlinkState == 0) {
      megatreeRow++;
      if (megatreeRow >= MEGATREE_HEIGHT) {
        megatreeRow = 0;  // Loop back to top
      }
    }
  }
  
  // Show the result
  FastLED.show();
  #endif
}
