#include "Scheduler.h"

void Scheduler::setSchedule(uint8_t hour, uint8_t minute) {
    _schedHour = hour;
    _schedMinute = minute;
    Logger::instance().logf("[Scheduler] Schedule set to %02u:%02u", _schedHour, _schedMinute);
}

void Scheduler::runScheduledScene() {
    // Example: pick first scene in ContentManager for demo
    const auto& scenes = ContentManager::instance().getAllScenes();
    if (scenes.empty()) {
        Logger::instance().log("[Scheduler] No scenes available to run.");
        return;
    }

    const SceneEntry* scene = &scenes[0];
    Logger::instance().logf("[Scheduler] Running scheduled scene: %s (#%u)",
                             scene->displayName.c_str(), scene->id);

    if (scene->runCallback) {
        scene->runCallback();
    } else {
        Logger::instance().logf("[Scheduler] No callback defined for scene: %s", scene->displayName.c_str());
    }
}
