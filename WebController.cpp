#include "WebController.h"
#include "ThemeManager.h"
#include "ContentManager.h"
#include "Logger.h"

WebController::WebController() {}

void WebController::begin(ThemeManager* themeMgr, ContentManager* contentMgr) {
    themes  = themeMgr;
    content = contentMgr;

    setupRoutes();
    server.begin();

    Logger::instance().log("HTTP server listening on port 80");
}

void WebController::handle() {
    server.handleClient();
}

void WebController::setupRoutes() {

    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/plain", "ESP32 Matrix Show Controller OK");
    });

    server.on("/status", HTTP_GET, [this]() {
        server.send(200, "text/plain", "OK");
    });

    server.on("/theme/off", HTTP_GET, [this]() {
        if (themes) themes->setTheme(0);
        server.send(200, "text/plain", "Theme OFF");
    });

    server.on("/theme/test", HTTP_GET, [this]() {
        if (themes) themes->setTheme(1);
        server.send(200, "text/plain", "Theme TEST");
    });
}
