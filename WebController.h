#pragma once

#include <Arduino.h>

class ThemeManager;
class ContentManager;

class WebController {
public:
    WebController();

    // Original API
    void begin(uint16_t port = 80);

    // V15.4FIX.1 overload for current .ino usage
    void begin(ThemeManager* themes, ContentManager* content);

    void handle();  // call in loop for request processing

private:
    ThemeManager* _themes = nullptr;
    ContentManager* _content = nullptr;

    uint16_t _port = 80;

    // any other private members from original class remain
};
