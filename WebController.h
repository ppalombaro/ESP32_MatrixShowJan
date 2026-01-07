#pragma once

#include <Arduino.h>
#include <WiFi.h>

// Forward declarations
class ThemeManager;
class ContentManager;

class WebController {
public:
    WebController();

    void begin(uint16_t port = 80);
    void begin(ThemeManager* themes, ContentManager* content);

    void handle();

private:
    ThemeManager* _themes = nullptr;
    ContentManager* _content = nullptr;
    uint16_t _port = 80;
};
