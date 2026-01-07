#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "ContentManager.h"
#include "SceneData.h"
#include "Logger.h"

#define WEBCONTROLLER_VERSION "15.4FIX"

class WebController {
public:
    static WebController& instance();

    void begin(uint16_t port = 80);
    void handleClient();

    // Routes
    void handlePreview(uint16_t sceneId);
    void handleSetBrightness(uint8_t percent);
    void handleGetSchedule();
    void handleSetSchedule(uint8_t hour, uint8_t minute);

private:
    WebController();
    ~WebController();

    WebController(const WebController&) = delete;
    WebController& operator=(const WebController&) = delete;

    WebServer* _server;
    uint8_t _brightnessPercent;

    // Sticky schedule
    uint8_t _schedHour;
    uint8_t _schedMinute;
};
