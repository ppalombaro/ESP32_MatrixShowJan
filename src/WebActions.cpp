/* WebActions.cpp
   API endpoints for the web interface
   VERSION: V16.4.13-2026-09-07 - schedule/eligible/status endpoints, ASCII responses
*/

#include "WebActions.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "Config.h"
#include <WebServer.h>
#include <Preferences.h>

static Preferences prefs;

WebActions::WebActions(ContentManager* cm, ThemeManager* tm, MatrixDisplay* disp, WebServer* srv)
: contentMgr(cm), themeMgr(tm), display(disp), server(srv) {}

static String jsonEsc(const String& s) {
    String o;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c == '"' || c == '\\') { o += '\\'; o += c; }
        else if (c == '\n') o += "\\n";
        else o += c;
    }
    return o;
}

void WebActions::attach() {

    // ---- Content rendering ------------------------------------------------
    server->on("/api/render", HTTP_GET, [this]() {
        if (!server->hasArg("id")) {
            server->send(400, "text/plain", "Missing 'id' parameter");
            return;
        }
        uint16_t id = server->arg("id").toInt();
        bool success = contentMgr->renderContent(id);
        const ContentItem* item = contentMgr->getContentById(id);
        String response = success
            ? "OK: Rendering: " + (item ? item->name : String("Unknown"))
            : "ERROR: Content not found";
        server->send(success ? 200 : 404, "text/plain", response);
    });

    server->on("/api/clear", HTTP_GET, [this]() {
        if (contentMgr) contentMgr->stopPlayback();
        if (display) { display->clear(); display->show(); }
        Logger::instance().log("[WebActions] Display cleared");
        server->send(200, "text/plain", "OK: Display cleared");
    });

    server->on("/api/test", HTTP_GET, [this]() {
        if (contentMgr) contentMgr->stopPlayback();
        if (display) {
            display->clear();
            for (int m = 0; m < 2; m++)
                for (int x = 0; x < COLS; x++)
                    for (int y = 0; y < ROWS; y++)
                        display->setPixel(m, x, y, CRGB::Red);
            display->show();
        }
        Logger::instance().log("[WebActions] Test pattern");
        server->send(200, "text/plain", "OK: test pattern");
    });

    // ---- Playback status -------------------------------------------------
    server->on("/api/status", HTTP_GET, [this]() {
        ContentManager::PlaybackStatus s = contentMgr->getStatus();
        String j = "{";
        j += "\"id\":" + String(s.id) + ",";
        j += "\"name\":\"" + jsonEsc(s.name) + "\",";
        j += "\"type\":\"" + jsonEsc(s.type) + "\",";
        j += "\"theme\":\"" + jsonEsc(s.theme) + "\",";
        j += "\"scheduleActive\":" + String(s.scheduleActive ? "true" : "false");
        j += "}";
        server->send(200, "application/json", j);
    });

    // ---- Random mode ---------------------------------------------------
    server->on("/api/random/enable", HTTP_GET, [this]() {
        contentMgr->enableRandomMode(true);
        server->send(200, "text/plain", "OK: Random mode ENABLED");
    });
    server->on("/api/random/disable", HTTP_GET, [this]() {
        contentMgr->enableRandomMode(false);
        server->send(200, "text/plain", "OK: Random mode DISABLED");
    });
    server->on("/api/random/interval", HTTP_GET, [this]() {
        if (!server->hasArg("ms")) { server->send(400, "text/plain", "Missing 'ms' parameter"); return; }
        unsigned long intervalMs = server->arg("ms").toInt();
        contentMgr->setRandomInterval(intervalMs);
        server->send(200, "text/plain", "OK: Interval set to " + String(intervalMs) + " ms");
    });
    server->on("/api/random/filter", HTTP_GET, [this]() {
        if (!server->hasArg("theme")) { server->send(400, "text/plain", "Missing 'theme' parameter"); return; }
        String theme = server->arg("theme");
        contentMgr->setRandomThemeFilter(theme);
        server->send(200, "text/plain", theme.length() > 0
            ? "OK: Filter set to: " + theme
            : "OK: Filter cleared (ALL THEMES)");
    });

    // ---- Eligible-content mask (V16.4.13) -------------------------------
    server->on("/api/random/eligible", HTTP_GET, [this]() {
        String csv = server->hasArg("exclude") ? server->arg("exclude") : String("");
        contentMgr->setExcludedByIds(csv);
        server->send(200, "text/plain", "OK: exclusions saved");
    });
    server->on("/api/random/eligible/get", HTTP_GET, [this]() {
        auto keys = contentMgr->getExcludedKeys();
        String j = "[";
        bool first = true;
        for (const auto& item : contentMgr->getContent()) {
            if (!contentMgr->isEligible(item) && item.type != CONTENT_TEST) {
                if (!first) j += ",";
                j += String(item.id);
                first = false;
            }
        }
        j += "]";
        server->send(200, "application/json", j);
    });

    // ---- Scheduler / run mode (V16.4.13) ------------------------------
    server->on("/api/scheduler/enable", HTTP_GET, [this]() {
        contentMgr->enableScheduler(true);
        server->send(200, "text/plain", "OK: Schedule mode ENABLED");
    });
    server->on("/api/scheduler/disable", HTTP_GET, [this]() {
        contentMgr->enableScheduler(false);
        server->send(200, "text/plain", "OK: Manual mode (schedule OFF)");
    });
    server->on("/api/schedule/times", HTTP_GET, [this]() {
        if (!server->hasArg("start") || !server->hasArg("end")) {
            server->send(400, "text/plain", "Missing 'start' or 'end' (HH:MM)");
            return;
        }
        int sh, sm, eh, em;
        if (sscanf(server->arg("start").c_str(), "%d:%d", &sh, &sm) != 2 ||
            sscanf(server->arg("end").c_str(),   "%d:%d", &eh, &em) != 2 ||
            sh < 0 || sh > 23 || eh < 0 || eh > 23 ||
            sm < 0 || sm > 59 || em < 0 || em > 59) {
            server->send(400, "text/plain", "ERROR: bad time (expect HH:MM 00:00-23:59)");
            return;
        }
        contentMgr->setScheduleWindow(sh, sm, eh, em);
        server->send(200, "text/plain", "OK: window " + server->arg("start") + " - " + server->arg("end"));
    });
    server->on("/api/schedule/get", HTTP_GET, [this]() {
        uint8_t sh, sm, eh, em;
        contentMgr->getScheduleWindow(sh, sm, eh, em);
        String j = "{";
        j += "\"startH\":" + String(sh) + ",\"startM\":" + String(sm) + ",";
        j += "\"endH\":" + String(eh) + ",\"endM\":" + String(em) + ",";
        j += "\"runMode\":" + String(contentMgr->getRunMode()) + ",";
        j += "\"active\":" + String(contentMgr->isScheduleActive() ? "true" : "false");
        j += "}";
        server->send(200, "application/json", j);
    });

    // ---- Brightness ----------------------------------------------------
    server->on("/api/brightness", HTTP_GET, [this]() {
        if (!server->hasArg("value")) { server->send(400, "text/plain", "Missing 'value' parameter"); return; }
        int v = server->arg("value").toInt();
        if (v < 1) v = 1;
        if (v > 255) v = 255;
        if (display) display->setBrightness((uint8_t)v);
        saveBrightness((uint8_t)v);
        Logger::instance().log("[WebActions] Brightness set to " + String(v));
        server->send(200, "text/plain", String(v));
    });
    server->on("/api/brightness/get", HTTP_GET, [this]() {
        server->send(200, "text/plain", String(loadBrightness()));
    });

    // ---- Logs / theme ------------------------------------------------
    server->on("/api/logs/clear", HTTP_GET, [this]() {
        Logger::instance().clear();
        server->send(200, "text/plain", "OK: Logs cleared");
    });
    server->on("/api/theme/set", HTTP_GET, [this]() {
        if (!server->hasArg("id")) { server->send(400, "text/plain", "Missing 'id' parameter"); return; }
        uint8_t themeId = server->arg("id").toInt();
        themeMgr->setTheme(themeId);
        server->send(200, "text/plain", "OK: Theme set to " + String(themeId));
    });
}

void WebActions::saveBrightness(uint8_t brightness) {
    prefs.begin(PREFS_NAMESPACE, false);
    prefs.putUChar(BRIGHTNESS_KEY, brightness);
    prefs.end();
}

uint8_t WebActions::loadBrightness() {
    prefs.begin(PREFS_NAMESPACE, true);
    uint8_t brightness = prefs.getUChar(BRIGHTNESS_KEY, DEFAULT_BRIGHTNESS);
    prefs.end();
    return brightness;
}
