#pragma once
#include <Arduino.h>
#include "Logger.h"
#include "ContentManager.h"

class Scheduler {
public:
    Scheduler() : _schedHour(0), _schedMinute(0) {}

    void setSchedule(uint8_t hour, uint8_t minute);
    void runScheduledScene();

private:
    uint8_t _schedHour;
    uint8_t _schedMinute;
};
