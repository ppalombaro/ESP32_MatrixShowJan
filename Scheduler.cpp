#include "Scheduler.h"
#include <TimeLib.h>

static Scheduler* _instance = nullptr;

Scheduler& Scheduler::instance() {
    if (!_instance) _instance = new Scheduler();
    return *_instance;
}

Scheduler::Scheduler()
: _schedHour(0)
, _schedMinute(0)
, _lastRunHour(255)
, _lastRunMinute(255)
{
}

Scheduler::~Scheduler() {}

void Scheduler::begin() {
    _lastRunHour   = 255;
    _lastRunMinute = 255;
}

void Scheduler::setSchedule(uint8_t hour, uint8_t minute) {
    _schedHour   = hour;
    _schedMinute = minute;
    Logger::instance().logf("[Scheduler] Schedule set to %02u:%02u", _schedHour, _schedMinute);
}

void Scheduler::getSchedule(uint8_t& hour, uint8_t& minute) const {
    hour   = _schedHour;
    minute = _schedMinute;
}

// ------------------------------------------------------------
// Call frequently in main loop
// ------------------------------------------------------------
void Scheduler::update() {
    int currentHour   = hour();
    int currentMinute = minute();

    // Only trigger once per minute
    if (currentHour == _schedHour && currentMinute == _schedMinute) {
        if (_lastRunHour != _schedHour || _lastRunMinute != _schedMinute) {
            runScheduledScene();
            _lastRunHour   = _schedHour;
            _lastRunMinute = _schedMinute;
        }
    } else {
        // reset once outside scheduled time
        _lastRunHour   = 255;
        _lastRunMinute = 255;
    }
}

// ------------------------------------------------------------
// Execute scheduled scene
// ------------------------------------------------------------
void Scheduler::runScheduledScene() {
    // For now, pick first scene in registry (can be extended to saved selection)
    const auto* scene = ContentManager::instance().getSceneByIndex(0);
    if (!scene) {
        Logger::instance().log("[Scheduler] No scenes registered for scheduled run");
        return;
    }

    Logger::instance().logf("[Scheduler] Running scheduled scene: %s (#%u)",
                             scene->displayName.c_str(), scene->id);

    SceneData sd(scene);
    if (scene->source == SceneSourceType::FFAT_JSON) {
        if (!sd.load()) {
            Logger::instance().logf("[Scheduler] Failed to load scene: %s", scene->displayName.c_str());
            return;
        }
    }

    sd.play();
}
