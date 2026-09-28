/* WebController.cpp
   Web server implementation
   VERSION: V16.2.5-2026-01-10T22:18:00Z - Created missing implementation
*/

#include "WebController.h"
#include "WebPages.h"
#include "WebActions.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "Config.h"
#include <Update.h>

WebController::WebController() : server(80) {}

void WebController::begin(ContentManager* contentMgr, ThemeManager* themeMgr, MatrixDisplay* disp) {
    content = contentMgr;
    themes = themeMgr;
    display = disp;

    setupRoutes();
    setupOtaRoute();

    // V16.2.5-2026-01-10T22:18:00Z - Create WebActions
    actions = new WebActions(content, themes, display, &server);
    actions->attach();
    
    server.begin();
    Serial.println("[WebController] Server started on port 80");
}

void WebController::handle() {
    server.handleClient();
}

void WebController::setupRoutes() {
    // V16.2.5-2026-01-10T22:18:00Z - Setup page routes
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildRootPage());
    });
    
    server.on("/control", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildControlPage(content));
    });
    
    server.on("/schedule", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildSchedulePage(content));
    });
    
    server.on("/times", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildTimesPage(content));
    });
    
    server.on("/logs", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildLogsPage());
    });
    
    server.on("/discovery", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildDiscoveryPage(content));
    });
}

// V16.4.14 - Web-based firmware upload, as a second OTA path alongside
// ArduinoOTA. Deliberately minimal (no WebPages/ContentManager string
// building) so it stays reachable even if other pages are misbehaving -
// this route only touches the server itself and Update.h.
void WebController::setupOtaRoute() {
    server.on("/update", HTTP_GET, [this]() {
        server.send(200, "text/html",
            "<!DOCTYPE html><html><head><title>Firmware Update</title>"
            "<meta name='viewport' content='width=device-width,initial-scale=1'></head><body>"
            "<h2>ESP32 Matrix Show &ndash; Firmware Update</h2>"
            "<p>Upload a firmware.bin built for this board (env: esp32dev). "
            "The display pauses during the flash and the device reboots when done.</p>"
            "<form method='POST' action='/update' enctype='multipart/form-data'>"
            "<input type='password' name='pw' placeholder='OTA password' required><br><br>"
            "<input type='file' name='update' accept='.bin' required><br><br>"
            "<input type='submit' value='Upload &amp; Flash'>"
            "</form></body></html>");
    });

    server.on("/update", HTTP_POST, [this]() {
        if (!uploadAuthOk) {
            server.send(403, "text/plain", "Unauthorized - bad or missing OTA password");
            return;
        }
        bool ok = !Update.hasError();
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", ok ? "Update OK - rebooting..." : "Update FAILED - see /logs");
        if (ok) {
            delay(500);
            ESP.restart();
        }
    }, [this]() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            // The password field is declared before the file field in the form
            // above, so it has already been parsed into server.arg() by the
            // time this file part starts.
            uploadAuthOk = (server.arg("pw") == String(OTA_PASSWORD));
            if (!uploadAuthOk) {
                Logger::instance().log("[OTA-Web] Rejected - bad password");
                return;
            }
            Logger::instance().log("[OTA-Web] Upload starting: " + upload.filename);
            if (content) content->stopPlayback();
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
                Logger::instance().log("[OTA-Web] Update.begin() failed");
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (!uploadAuthOk) return;
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (!uploadAuthOk) return;
            if (Update.end(true)) {
                Logger::instance().log("[OTA-Web] Upload complete: " + String(upload.totalSize) + " bytes");
            } else {
                Update.printError(Serial);
                Logger::instance().log("[OTA-Web] Upload FAILED");
            }
        }
    });
}