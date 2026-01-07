#include "Scheduler.h"
#include "ContentManager.h"

void Scheduler::setSchedule(uint8_t hour, uint8_t minute) {
    _schedHour = hour;
    _schedMinute = minute;
}

void Scheduler::runScheduledScene() {
    const auto& scenes = ContentManager::instance().getAllScenes();
    if (scenes.empty()) return;

    const SceneEntry* scene = &scenes[0];
    if (!scene) return;

    Serial.printf("Running scheduled scene: %s (%d)\n",
                  scene->displayName.c_str(), scene->id);

    // Keep any existing logic for triggering the scene here
}
