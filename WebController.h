#pragma once

#include <Arduino.h>
#include <WebServer.h>

class ThemeManager;
class ContentManager;

class WebController {
public:
    WebController();

    void begin(ThemeManager* themeMgr, ContentManager* contentMgr);
    void handle();

private:
    WebServer server{80};

    ThemeManager* themeMgr = nullptr;
    ContentManager* contentMgr = nullptr;

    void registerRoutes();
};
