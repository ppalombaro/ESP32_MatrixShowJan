#include "WebActions.h"
#include "ThemeManager.h"
#include <WebServer.h>

extern WebServer server;

WebActions::WebActions(ThemeManager* tm)
: themeMgr(tm) {}

void WebActions::attach() {

    server.on("/theme/off", HTTP_GET, [this]() {
        themeMgr->setTheme(0);
        server.send(200, "text/plain", "theme off");
    });

    server.on("/theme/test", HTTP_GET, [this]() {
        themeMgr->setTheme(1);
        server.send(200, "text/plain", "theme test");
    });
}
