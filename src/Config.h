/* Config.h
   Hardware configuration and global settings
   VERSION: V16.1.2-2026-01-08T15:00:00Z - Content auto-discovery architecture

   Matrix 0 = Right Window Matrix - PIN 16 (ACTIVE)
   Matrix 1 = Left Window Matrix  - PIN 17 (ACTIVE)
   Matrix 2 = Mega Matrix         - PIN 18 (DISABLED - future)
   Mega Tree = PIN 19             (DISABLED - future)
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>

// ================= WIFI CONFIGURATION =================
#define WIFI_SSID     "palombaro"
#define WIFI_PASSWORD "boobug00"
#define HOSTNAME      "palombaro-matrix"

// ================= FEATURE FLAGS ======================
#define ENABLE_MEGAMATRIX false   // Matrix 2 disabled (future)
#define ENABLE_MEGATREE   false   // Mega tree disabled (future)

// ================= HARDWARE CONFIG ====================
//
// IMPORTANT RULE:
// Matrix 0 is the *single source of truth*
// All other matrices alias or extend from it
//

// -------- Matrix 0 (Right Window - ACTIVE) -------------
#define MATRIX0_ROWS 25
#define MATRIX0_COLS 20
#define MATRIX0_LEDS (MATRIX0_ROWS * MATRIX0_COLS)  // 500
#define PIN_MATRIX0 16

// -------- Matrix 1 (Left Window - ACTIVE) --------------
// Identical physical dimensions to Matrix 0
#define MATRIX1_ROWS MATRIX0_ROWS
#define MATRIX1_COLS MATRIX0_COLS
#define MATRIX1_LEDS MATRIX0_LEDS
#define PIN_MATRIX1 17

// -------- Matrix 2 (Mega Matrix - DISABLED) ------------
#define MATRIX2_ROWS 40
#define MATRIX2_COLS 50
#define MATRIX2_LEDS (MATRIX2_ROWS * MATRIX2_COLS)  // 2000
#define PIN_MATRIX2 18

// -------- Mega Tree (DISABLED) -------------------------
#define PIN_MEGATREE 19
#define MEGATREE_BRANCHES 20
#define MEGATREE_LEDS_PER_BRANCH 50
#define MEGATREE_LEDS (MEGATREE_BRANCHES * MEGATREE_LEDS_PER_BRANCH)

// ================= LEGACY COMPATIBILITY ================
// These are aliases ONLY — never redefine dimensions above
#define ROWS        MATRIX0_ROWS
#define COLS        MATRIX0_COLS
#define MATRIX_LEDS MATRIX0_LEDS
#define PIN_LEFT    PIN_MATRIX1
#define PIN_RIGHT   PIN_MATRIX0

// ================= TOTAL LED COUNT =====================
#if ENABLE_MEGAMATRIX
  #define TOTAL_LEDS (MATRIX0_LEDS + MATRIX1_LEDS + MATRIX2_LEDS)
#else
  #define TOTAL_LEDS (MATRIX0_LEDS + MATRIX1_LEDS)  // 1000 (current)
#endif

// ================= FASTLED SETTINGS ====================
#define COLOR_ORDER RGB
#define LED_TYPE    WS2811
#define DEFAULT_BRIGHTNESS 20

// ================= DISPLAY TIMING ======================
#define STATIC_SCENE_INTERVAL 5000
#define ANIMATION_INTERVAL    8000
#define SCROLL_INTERVAL       10000
#define SCENE_INTERVAL        STATIC_SCENE_INTERVAL  // legacy alias

// ================= RUN MODES ===========================
#define RUN_MODE_MANUAL   0
#define RUN_MODE_SCHEDULE 1

// ================= THEMES ==============================
#define THEME_OFF           0
#define THEME_CHRISTMAS     1
#define THEME_HALLOWEEN     2
#define THEME_THANKSGIVING  3
#define THEME_NEWYEAR       4
#define THEME_OSU           5
#define THEME_RANDOM_MAGIC  10
#define THEME_TEST_MODE     99

// ================= CONTENT TYPES =======================
#define CONTENT_TYPE_SCENE       1
#define CONTENT_TYPE_ANIMATION   2
#define CONTENT_TYPE_SCROLL      3
#define CONTENT_TYPE_PROCEDURAL  4
#define CONTENT_TYPE_TEST        5

// ================= PREFERENCES KEYS ====================
#define PREFS_NAMESPACE        "show-config"
#define RUN_MODE_KEY           "run_mode"
#define START_HOUR_KEY         "start_hr"
#define END_HOUR_KEY           "end_hr"
#define BRIGHTNESS_KEY         "brightness"
#define RANDOM_SCENES_MASK_KEY "rand_scns"
#define RANDOM_ANIMS_MASK_KEY  "rand_anims"
#define RANDOM_TESTS_MASK_KEY  "rand_tests"

// ================= THEME COLORS ========================
// Christmas
#define CHRISTMAS_COLOR_1 CRGB(255,   0,   0)
#define CHRISTMAS_COLOR_2 CRGB(  0, 255,   0)
#define CHRISTMAS_COLOR_3 CRGB(255, 255, 255)

// Halloween
#define HALLOWEEN_COLOR_1 CRGB(255, 128,   0)
#define HALLOWEEN_COLOR_2 CRGB(128,   0, 128)
#define HALLOWEEN_COLOR_3 CRGB(  0, 255,   0)

// Thanksgiving
#define THANKSGIVING_COLOR_1 CRGB(255, 140,   0)
#define THANKSGIVING_COLOR_2 CRGB(218, 165,  32)
#define THANKSGIVING_COLOR_3 CRGB(139,  69,  19)

// New Year
#define NEWYEAR_COLOR_1 CRGB(255, 215,   0)
#define NEWYEAR_COLOR_2 CRGB(192, 192, 192)
#define NEWYEAR_COLOR_3 CRGB(255, 255, 255)

// Ohio State
#define OSU_COLOR_1 CRGB(187,   0,   0)
#define OSU_COLOR_2 CRGB(102, 102, 102)
#define OSU_COLOR_3 CRGB(255, 255, 255)

// ================= CUSTOM COLORS =======================
extern const CRGB Peach;
extern const CRGB Sienna;
extern const CRGB RudolfBrown;
extern const CRGB RudolfTan;
extern const CRGB SnowWhite;
extern const CRGB CoolBlue;

#endif // CONFIG_H
