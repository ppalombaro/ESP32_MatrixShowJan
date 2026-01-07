#include "HolidayAnims.h"
#include "ContentManager.h"

// ------------------------------------------------------------
// FORWARD DECLARATIONS OF CODE-BASED ANIMATIONS
// ------------------------------------------------------------
// These should already exist elsewhere in your project.
// If not, stub them safely until implemented.

static void animRainbowChase();
static void animColorPulse();
static void animTestPattern();

// ------------------------------------------------------------
// REGISTRATION
// ------------------------------------------------------------
void HolidayAnims::registerScenes() {

    ContentManager& cm = ContentManager::instance();

    // ---- TEST / DEBUG ----
    cm.registerCodeScene(
        "Test – Rainbow Chase",
        ThemeId::TEST,
        animRainbowChase
    );

    cm.registerCodeScene(
        "Test – Color Pulse",
        ThemeId::TEST,
        animColorPulse
    );

    cm.registerCodeScene(
        "Test – Pattern Grid",
        ThemeId::TEST,
        animTestPattern
    );

    // ---- FALLBACK / MISC ----
    // These can be used when FFAT space is exhausted
    cm.registerCodeScene(
        "Misc – Solid Color Pulse",
        ThemeId::MISC,
        animColorPulse
    );
}

// ------------------------------------------------------------
// CODE-BASED ANIMATION IMPLEMENTATIONS
// ------------------------------------------------------------

static void animRainbowChase() {
    Serial.println("[HolidayAnims] Running Rainbow Chase (CODE)");
    // TODO: call your existing animation engine here
}

static void animColorPulse() {
    Serial.println("[HolidayAnims] Running Color Pulse (CODE)");
    // TODO: implement pulse logic
}

static void animTestPattern() {
    Serial.println("[HolidayAnims] Running Test Pattern (CODE)");
    // TODO: implement test pattern
}
