#pragma once
#include <Arduino.h>
#include <vector>
#include "HolidayAnims.h"
#include "Animations.h"  // Ensure Animation is known

// Forward declaration of SceneEntry
struct SceneEntry {
    int id;
    String displayName;
};

class ContentManager {
public:
    ContentManager() = default;

    // ---------------------------
    // Singleton accessor for Scheduler
    // ---------------------------
    static ContentManager& instance() {
        static ContentManager _instance;
        return _instance;
    }

    // Existing public interface
    std::vector<SceneEntry> getAllScenes() { return _scenes; }
    void loadDynamicAnimations(HolidayAnims* holidayAnims);

private:
    std::vector<SceneEntry> _scenes;
};
