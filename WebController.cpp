#include "WebController.h"
#include "ThemeManager.h"
#include "ContentManager.h"

WebController::WebController() {}

void WebController::begin(ThemeManager* tMgr, ContentManager* cMgr) {
    themeMgr = tMgr;
    contentMgr = cMgr;

    registerRoutes();

    server.begin();
    Serial.println("HTTP server listening on port 80");
}

void WebController::registerRoutes() {

    // ---------- ROOT TEST ----------
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/plain", "WEB OK");
    });

    // ---------- DISCOVERY TEST ----------
    server.on("/discover", HTTP_GET, [this]() {
        String json =
            "{"
            "\"device\":\"ESP32 Matrix Show\","
            "\"version\":\"15.4\","
            "\"status\":\"online\""
            "}";

        server.send(200, "application/json", json);
    });

    // ---------- NOT FOUND ----------
    server.onNotFound([this]() {
        server.send(404, "text/plain", "NOT FOUND");
    });
}

void WebController::handle() {
    server.handleClient();
}
