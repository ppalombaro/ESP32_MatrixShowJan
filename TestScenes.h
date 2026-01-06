/* TestScenes.h
   Diagnostic test patterns for troubleshooting display issues
   VERSION: V15.0.1-2026-01-03T12:05:00Z - Matrix ID and megatree tests
   
   V15.0.1-2026-01-03T12:05:00Z - Added drawMatrixID() for hardware verification
   V15.0.1-2026-01-03T12:05:00Z - Added updateMegatreeTest() for branch verification
   V15.0.1-2026-01-03T12:05:00Z - CORRECTED: Matrix 3 is megamatrix (not megatree)
*/

#ifndef TEST_SCENES_H
#define TEST_SCENES_H

#include "MatrixDisplay.h"

class TestScenes {
public:
  TestScenes(MatrixDisplay* display);
  
  // Original test patterns (1-6)
  void drawColorRows(int matrix);
  void drawColorColumns(int matrix);
  void drawAllWhite(int matrix);
  void drawAllRed(int matrix);
  void drawCornerTest(int matrix);
  void drawGridTest(int matrix);
  
  // V15.0.1-2026-01-03T12:05:00Z - NEW: Matrix identification test (pattern 7)
  // Displays large number (1, 2, or 3) on specified matrix
  // Matrix 1 = far right (GPIO 19), Matrix 2 = middle (GPIO 18), Matrix 3 = far left megamatrix (GPIO 5)
  void drawMatrixID(int matrix);
  
  // V15.0.1-2026-01-03T12:05:00Z - NEW: Megatree branch test
  // Each branch different color (Redâ†’Yellowâ†’Greenâ†’Blueâ†’Purpleâ†’Red)
  // Blinks from top to bottom - call repeatedly to animate
  void updateMegatreeTest();
  
  CRGB getTestColor(int index);  // Public helper

private:
  MatrixDisplay* disp;
  
  // Moving test pattern state
  int scrollOffset = 0; 
  unsigned long lastScrollTime = 0;
  void updateScrollState();
  
  // V15.0.1-2026-01-03T12:05:00Z - Megatree test animation state
  unsigned long lastMegatreeUpdate = 0;
  int megatreeBlinkState = 0;  // 0 = off, 1 = on
  int megatreeRow = 0;          // Current row blinking (0 = top)
  
  // V15.0.1-2026-01-03T12:05:00Z - Helper to draw large numbers on matrix
  void drawLargeNumber(int matrix, int number);
};

#endif
