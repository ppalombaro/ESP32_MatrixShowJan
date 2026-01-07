#pragma once
#include <Arduino.h>
#include "ContentManager.h"
#include "SceneData.h"
#include "Logger.h"

#define SCHEDULER_VERSION "15.4FIX"

class Scheduler {
public:
    static Scheduler& instance();

    void begin();

    // Set schedule (sticky)
    void setSchedule(uint8_t hour, uint8_t minute);
    void getSchedule(uint8_t& hour, uint8_t& minute) const;

    // Main loop tick — call frequently in loop()
    void update();

private:
    Scheduler();
    ~Scheduler();

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    uint8_t _schedHour;
    uint8_t _schedMinute;

    // Prevent multiple triggers per minute
    uint8_t _lastRunHour;
    uint8_t _lastRunMinute;

    void runScheduledScene();
};
