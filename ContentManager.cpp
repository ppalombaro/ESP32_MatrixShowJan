#include "ContentManager.h"
#include "Logger.h"
#include "FS.h"
#include "SPIFFS.h"

ContentManager::ContentManager() {}

void ContentManager::begin() {
    if (!SPIFFS.begin(true)) {
        Logger::instance().log("SPIFFS mount failed!");
    }
}

void ContentManager::update() {
    // Placeholder for dynamic content refresh if needed
}

// Populate both static and dynamic animations
void ContentManager::populateAnimations(HolidayAnims* ha) {
    if (!ha) return;

    loadStaticAnimations(ha);
    loadDynamicAnimations(ha);

    ha->beginAll(); // Initialize all animations after registering
}

// Register coded static animations here
void ContentManager::loadStaticAnimations(HolidayAnims* ha) {
    // Example static animations
    // ha->addAnimation(new SomeStaticAnimation());
}

// Scan SPIFFS for dynamic JSON animations and register them
void ContentManager::loadDynamicAnimations(HolidayAnims* ha) {
    File root = SPIFFS.open("/scenes");
    if (!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while (file) {
        String filename = file.name();
        if (filename.endsWith(".json")) {
            // Create a dynamic animation object
            Animation* anim = new Animation();
            anim->loadFromFile(filename); // make sure your Animation class has this
            ha->addAnimation(anim);
        }
        file = root.openNextFile();
    }
}
