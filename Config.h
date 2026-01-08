/* Config.h
   Hardware configuration and global settings
       Matrix 0 = Right Window  Matrix - PIN 16
       Matix 1  = Left Window Matrix (center) - PIN 17
       Matix 2  = Gargae Door - Mega Matrix = PIN 18
       Mega Tree = Pin 10


   VERSION: V15.0.2-2026-01-03T15:00:00Z - Added countdown preference keys

   VERSION: V15.0.1-2026-01-03T11:00:00Z - Feature flags, music removal, megamatrix support
   
   V15.0.1-2026-01-03T11:00:00Z - Added feature enable/disable flags
   V15.0.1-2026-01-03T11:00:00Z - Removed all music/waveform references
   V15.0.1-2026-01-03T11:00:00Z - Added configurable megamatrix (Matrix 3)
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>
 
// ============WiFi Configuration - EDIT THESE
#define WIFI_SSID "palombaro"
#define WIFI_PASSWORD "boobug00"
#define HOSTNAME "palombaro-matrix"
//--=====================================

// ========== FEATURE ENABLE/DISABLE FLAGS ==========
// V15.0.1-2026-01-03T11:00:00Z - Control which features are compiled/enabled

#define ENABLE_CHRISTMAS_THEME true
#define ENABLE_HALLOWEEN_THEME true
#define ENABLE_THANKSGIVING_THEME true
#define ENABLE_NEWYEAR_THEME true
#define ENABLE_OSU_THEME true
#define ENABLE_TEST_PATTERNS true
#define ENABLE_MEGATREE false  // V15.0.1-2026-01-03T11:00:00Z - Matrix 3 (future)
#define ENABLE_MEGAMATRIX false  // V15.0.1-2026-01-03T11:00:00Z - Matrix 3 (future)

// ========== HARDWARE CONFIGURATION ==========

// Matrix 1 Configuration (Far Right - existing)
#define MATRIX1_ROWS 25
#define MATRIX1_COLS 20
#define MATRIX1_LEDS (MATRIX1_ROWS * MATRIX1_COLS)  // 500
#define PIN_MATRIX1 16  // V15.0.1-2026-01-03T11:00:00Z - Far right matrix

// Matrix 2 Configuration (Middle - existing, was "left")
#define MATRIX2_ROWS 25
#define MATRIX2_COLS 20
#define MATRIX2_LEDS (MATRIX2_ROWS * MATRIX2_COLS)  // 500
#define PIN_MATRIX2 17  // V15.0.1-2026-01-03T11:00:00Z - Middle matrix

// V15.0.1-2026-01-03T11:00:00Z - Matrix 3 Configuration (Far Left - future megamatrix)
// This will be a LARGER matrix - configure size when installed -  
#define MATRIX3_ROWS 40         // CHANGE TO YOUR SIZE (think strands)
#define MATRIX3_COLS 50         // CHANGE TO YOUR SIZE (think bulbs per strand)
#define MATRIX3_LEDS (MATRIX3_ROWS * MATRIX3_COLS)  // 1000 (example)
#define PIN_MATRIX3 18           // CHANGE TO YOUR GPIO

// Legacy compatibility names (for existing code)
#define ROWS MATRIX1_ROWS
#define COLS MATRIX1_COLS
#define MATRIX_LEDS MATRIX1_LEDS
#define PIN_LEFT PIN_MATRIX2    // Middle matrix (existing "left")
#define PIN_RIGHT PIN_MATRIX1   // Far right matrix (existing "right")

// V15.0.1-2026-01-03T11:00:00Z - Total LED count calculation
#if ENABLE_MEGAMATRIX
  #define TOTAL_LEDS (MATRIX1_LEDS + MATRIX2_LEDS + MATRIX3_LEDS)
#else
  #define TOTAL_LEDS (MATRIX1_LEDS + MATRIX2_LEDS)  // 1000 (existing setup)
#endif

// V15.0.1-2026-01-03T11:00:00Z - Megatree Configuration (separate from matrices)
#define PIN_MEGATREE 19
#define MEGATREE_LEDS 500       // CHANGE TO YOUR LED COUNT - asumes a box of 500)
#define MEGATREE_HEIGHT 100      // Height in inches for wave calculations (assume strands of 50 with 2 inch spacing)
#define MEGATREE_BRANCHES 10      // Number of branches for test pattern  10 strands of 50

#define COLOR_ORDER RGB
#define LED_TYPE WS2811

// Display Settings
#define DEFAULT_BRIGHTNESS 20

// V15.0.1-2026-01-03T11:00:00Z - Display intervals (removed MUSIC_INTERVAL)
#define STATIC_SCENE_INTERVAL 5000    // 5 seconds for static scenes
#define ANIMATION_INTERVAL 8000       // 8 seconds for regular animations
#define SCENE_INTERVAL 5000           // Legacy - kept for compatibility

// WiFi Configuration - EDIT THESE
#define WIFI_SSID "palombaro"
#define WIFI_PASS "boobug00"
#define HOSTNAME "esp32-matrix"

// --- Run Mode Definitions ---
#define RUN_MODE_MANUAL 0       
#define RUN_MODE_SCHEDULE 1     

// --- Theme Definitions ---
// V15.0.1-2026-01-03T11:00:00Z - Multi-holiday themes
#define THEME_OFF 0
#define THEME_CHRISTMAS_STATIC 1
#define THEME_CHRISTMAS_ANIMATED 2
#define THEME_RANDOM_MAGIC 3
#define THEME_HALLOWEEN_STATIC 10
#define THEME_HALLOWEEN_ANIMATED 11
#define THEME_THANKSGIVING_STATIC 20
#define THEME_THANKSGIVING_ANIMATED 21
#define THEME_NEWYEAR_STATIC 30
#define THEME_NEWYEAR_ANIMATED 31
#define THEME_OSU_STATIC 40
#define THEME_OSU_ANIMATED 41
#define THEME_TEST_MODE 5 

// --- Scheduler Globals ---
extern uint8_t global_run_mode;
extern uint8_t global_show_start_hour;
extern uint8_t global_show_end_hour;

// --- Preference Keys for Persistent Storage ---
#define PREFS_NAMESPACE "show-config"
#define RUN_MODE_KEY "run_mode"
#define START_HOUR_KEY "start_hr"
#define END_HOUR_KEY "end_hr"
#define BRIGHTNESS_KEY "brightness"
 // --- Random Show Configuration Keys ---
#define RANDOM_SCENES_MASK_KEY "rand_scns"
#define RANDOM_ANIMS_MASK_KEY "rand_anims"
#define RANDOM_TESTS_MASK_KEY "rand_tests"

// V15.0.2-2026-01-03T15:00:00Z - Countdown Date Configuration Keys
#define COUNTDOWN_XMAS_YEAR_KEY "xmas_year"
#define COUNTDOWN_XMAS_MONTH_KEY "xmas_month"
#define COUNTDOWN_XMAS_DAY_KEY "xmas_day"
#define COUNTDOWN_XMAS_HOUR_KEY "xmas_hour"
#define COUNTDOWN_NEWYEAR_YEAR_KEY "ny_year"
#define COUNTDOWN_NEWYEAR_MONTH_KEY "ny_month"
#define COUNTDOWN_NEWYEAR_DAY_KEY "ny_day"
#define COUNTDOWN_NEWYEAR_HOUR_KEY "ny_hour"

// V15.0.1-2026-01-03T11:00:00Z - Updated animation counts (removed 6 music animations)
// Christmas: 6 Static Scenes
#define DEFAULT_SCENES_MASK 0x3F // Binary 0011 1111

// V15.0.1-2026-01-03T11:00:00Z - Total Animations: 14 (5 Snowfall + 5 Other + 4 Geometry)
// Removed: 6 music animations
// Binary: 0011 1111 1111 1111 = 0x3FFF (14 bits)
#define DEFAULT_ANIMS_MASK 0x3FFF

// Test Patterns: 3 patterns (Color Rows, Color Columns, Matrix ID)
#define DEFAULT_TESTS_MASK 0x07 // Binary 0000 0111

// ========== THEME COLOR DEFINITIONS ==========
// V15.0.1-2026-01-03T11:00:00Z - Customizable theme colors

// Christmas Colors
#define CHRISTMAS_COLOR_1 CRGB(255, 0, 0)    // Red
#define CHRISTMAS_COLOR_2 CRGB(0, 255, 0)    // Green
#define CHRISTMAS_COLOR_3 CRGB(255, 255, 255) // White

// Halloween Colors
#define HALLOWEEN_COLOR_1 CRGB(255, 128, 0)  // Orange
#define HALLOWEEN_COLOR_2 CRGB(128, 0, 128)  // Purple
#define HALLOWEEN_COLOR_3 CRGB(0, 255, 0)    // Lime Green

// Thanksgiving Colors
#define THANKSGIVING_COLOR_1 CRGB(255, 140, 0)   // Dark Orange
#define THANKSGIVING_COLOR_2 CRGB(218, 165, 32)  // Goldenrod
#define THANKSGIVING_COLOR_3 CRGB(139, 69, 19)   // Brown

// New Year Colors
#define NEWYEAR_COLOR_1 CRGB(255, 215, 0)    // Gold
#define NEWYEAR_COLOR_2 CRGB(192, 192, 192)  // Silver
#define NEWYEAR_COLOR_3 CRGB(255, 255, 255)  // White

// Ohio State Colors
#define OSU_COLOR_1 CRGB(187, 0, 0)      // Scarlet Red
#define OSU_COLOR_2 CRGB(102, 102, 102)  // Gray
#define OSU_COLOR_3 CRGB(255, 255, 255)  // White

// --- Custom Colors (defined in Config.cpp) ---
extern const CRGB Peach;
extern const CRGB Sienna;
extern const CRGB RudolfBrown;
extern const CRGB RudolfTan;
extern const CRGB SnowWhite;
extern const CRGB CoolBlue;

#endif