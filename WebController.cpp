#include "WebController.h"

static WebController* _instance = nullptr;

WebController& WebController::instance() {
    if (!_instance) {
        _instance = new WebController();
    }
    return *_instance;
}

WebController::WebController()
: _server(nullptr)
, _brightnessPercent(20) // default 20%
, _schedHour(0)
, _schedMinute(0)
{
}

WebController::~WebController() {
    if (_server) {
        _server->stop();
        delete _server;
        _server = nullptr;
    }
}

void WebController::begin(uint16_t port) {
    if (_server) delete _server;

    _server = new WebServer(port);

    // Route handlers
    _server->on("/preview", [this]() {
        if (_server->hasArg("id")) {
            uint16_t id = _server->arg("id").toInt();
            handlePreview(id);
        } else {
            Logger::instance().log("[WebController] /preview missing id");
            _server->send(400, "text/plain", "Missing scene ID");
        }
    });

    _server->on("/brightness", [this]() {
        if (_server->hasArg("percent")) {
            uint8_t percent = _server->arg("percent").toInt();
            handleSetBrightness(percent);
            _server->send(200, "text/plain", "OK");
        } else {
            _server->send(400, "text/plain", "Missing percent");
        }
    });

    _server->on("/schedule/get", [this]() {
        handleGetSchedule();
    });

    _server->on("/schedule/set", [this]() {
        if (_server->hasArg("hour") && _server->hasArg("minute")) {
            uint8_t hour   = _server->arg("hour").toInt();
            uint8_t minute = _server->arg("minute").toInt();
            handleSetSchedule(hour, minute);
            _server->send(200, "text/plain", "OK");
        } else {
            _server->send(400, "text/plain", "Missing hour/minute");
        }
    });

    _server->begin();
    Logger::instance().logf("[WebController] Listening on port %u", port);
}

void WebController::handleClient() {
    if (_server) _server->handleClient();
}

// ------------------------------------------------------------
// Route implementations
// ------------------------------------------------------------
void WebController::handlePreview(uint16_t sceneId) {
    const SceneEntry* scene = ContentManager::instance().getSceneById(sceneId);
    if (!scene) {
        Logger::instance().logf("[WebController] Preview failed: Scene ID %u not found", sceneId);
        if (_server) _server->send(404, "text/plain", "Scene not found");
        return;
    }

    Logger::instance().logf("[WebController] Preview scene: %s (#%u)", scene->displayName.c_str(), scene->id);

    SceneData sd(scene);
    if (scene->source == SceneSourceType::FFAT_JSON) {
        if (!sd.load()) {
            if (_server) _server->send(500, "text/plain", "Failed to load scene");
            return;
        }
    }

    sd.play();

    if (_server) _server->send(200, "text/plain", "Preview OK");
}

void WebController::handleSetBrightness(uint8_t percent) {
    _brightnessPercent = constrain(percent, 0, 100);
    Logger::instance().logf("[WebController] Brightness set to %u%%", _brightnessPercent);

    // TODO: propagate to display engine
}

void WebController::handleGetSchedule() {
    if (_server) {
        String resp = String(_schedHour) + ":" + String(_schedMinute);
        _server->send(200, "text/plain", resp);
    }
}

void WebController::handleSetSchedule(uint8_t hour, uint8_t minute) {
    _schedHour   = hour;
    _schedMinute = minute;
    Logger::instance().logf("[WebController] Schedule set to %02u:%02u", _schedHour, _schedMinute);
}
