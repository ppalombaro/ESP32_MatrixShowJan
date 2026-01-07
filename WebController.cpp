#include "WebController.h"
#include "ThemeManager.h"
#include "ContentManager.h"

WebController::WebController() {
    // optional constructor logic
}

// Original port-based begin
void WebController::begin(uint16_t port) {
    _port = port;

    // existing web server initialization logic
}

// V15.4FIX.1 overload to accept themes + content (used in .ino)
void WebController::begin(ThemeManager* themes, ContentManager* content) {
    _themes = themes;
    _content = content;

    // call original begin with default port
    begin(80);
}

void WebController::handle() {
    // existing request handling logic
}
