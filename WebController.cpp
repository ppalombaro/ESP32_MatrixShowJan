#include "WebController.h"
#include "ThemeManager.h"
#include "ContentManager.h"

WebController::WebController() {}

void WebController::begin(uint16_t port) {
    _port = port;
}

void WebController::begin(ThemeManager* themes, ContentManager* content) {
    _themes = themes;
    _content = content;
}

void WebController::handle() {
    // handle web requests
}

