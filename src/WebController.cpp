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
#include <WiFi.h>
#include <time.h>
#include "esp_partition.h"

WebController::WebController() : server(80) {}

void WebController::begin(ContentManager* contentMgr, ThemeManager* themeMgr, MatrixDisplay* disp) {
    content = contentMgr;
    themes = themeMgr;
    display = disp;

    setupRoutes();
    setupOtaRoute();
    setupDataOtaRoute();

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
    
    // Plain-text clock/uptime readout: shows what time the ESP thinks it is,
    // since the rotating log can't answer "how long has it been on".
    server.on("/status", HTTP_GET, [this]() {
        unsigned long s = millis() / 1000UL;
        String out = "Uptime: " + String(s / 86400UL) + "d " + String((s / 3600UL) % 24) + "h " +
                     String((s / 60UL) % 60) + "m\n";
        struct tm ti;
        if (getLocalTime(&ti, 0)) {
            char buf[48];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &ti);
            out += "Local time: " + String(buf) + "\n";
        } else {
            out += "Local time: NOT SYNCED (schedule gate fail-open, show stays ON)\n";
        }
        out += "Scheduler: " + String(content->isSchedulerEnabled() ? "enabled" : "disabled") + "\n";
        out += "In window: " + String(content->isScheduleActive() ? "yes" : "no") + "\n";
        out += "WiFi: " + String(WiFi.status() == WL_CONNECTED ? "connected" : "DOWN") + "\n";
        server.send(200, "text/plain", out);
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

// V16.4.15 - Web-based data-partition upload: takes ffat.bin (the exact image
// tools/FFAT/build_simple_storage.py produces) and writes it directly to the
// "ffat" data partition (partitions.csv), byte-for-byte the same as
// `esptool write_flash 0x290000 ffat.bin` over USB. Update.h can't be used
// here - it only targets the app (firmware) partition - so this goes straight
// through esp_partition_erase_range/esp_partition_write.
void WebController::setupDataOtaRoute() {
    server.on("/update-data", HTTP_GET, [this]() {
        server.send(200, "text/html",
            "<!DOCTYPE html><html><head><title>Data Update</title>"
            "<meta name='viewport' content='width=device-width,initial-scale=1'></head><body>"
            "<h2>ESP32 Matrix Show &ndash; Data (scenes/animations) Update</h2>"
            "<p>Upload ffat.bin built by tools/FFAT/build_simple_storage.py. "
            "The display pauses during the flash and the device reboots when done.</p>"
            "<form method='POST' action='/update-data' enctype='multipart/form-data'>"
            "<input type='password' name='pw' placeholder='OTA password' required><br><br>"
            "<input type='file' name='update' accept='.bin' required><br><br>"
            "<input type='submit' value='Upload &amp; Flash Data'>"
            "</form></body></html>");
    });

    server.on("/update-data", HTTP_POST, [this]() {
        if (!dataUploadAuthOk) {
            server.send(403, "text/plain", "Unauthorized - bad or missing OTA password");
            return;
        }
        bool ok = !dataUploadFailed && dataWriteOffset > 0;
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", ok ? "Data update OK - rebooting..." : "Data update FAILED - see /logs");
        if (ok) {
            delay(500);
            ESP.restart();
        }
    }, [this]() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            dataUploadAuthOk = (server.arg("pw") == String(OTA_PASSWORD));
            dataWriteOffset = 0;
            dataUploadFailed = false;
            if (!dataUploadAuthOk) {
                Logger::instance().log("[DataOTA] Rejected - bad password");
                return;
            }
            dataPartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "ffat");
            if (!dataPartition) {
                Logger::instance().log("[DataOTA] ffat partition not found");
                dataUploadFailed = true;
                return;
            }
            Logger::instance().log("[DataOTA] Upload starting: " + upload.filename);
            if (content) content->stopPlayback();
            const esp_partition_t* part = (const esp_partition_t*)dataPartition;
            if (esp_partition_erase_range(part, 0, part->size) != ESP_OK) {
                Logger::instance().log("[DataOTA] Erase failed");
                dataUploadFailed = true;
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (!dataUploadAuthOk || dataUploadFailed || !dataPartition) return;
            const esp_partition_t* part = (const esp_partition_t*)dataPartition;
            if (dataWriteOffset + upload.currentSize > part->size) {
                Logger::instance().log("[DataOTA] Upload larger than ffat partition - aborting");
                dataUploadFailed = true;
                return;
            }
            if (esp_partition_write(part, dataWriteOffset, upload.buf, upload.currentSize) != ESP_OK) {
                Logger::instance().log("[DataOTA] Write failed at offset " + String((uint32_t)dataWriteOffset));
                dataUploadFailed = true;
                return;
            }
            dataWriteOffset += upload.currentSize;
        } else if (upload.status == UPLOAD_FILE_END) {
            if (!dataUploadAuthOk) return;
            if (!dataUploadFailed) {
                Logger::instance().log("[DataOTA] Upload complete: " + String((uint32_t)dataWriteOffset) + " bytes");
            } else {
                Logger::instance().log("[DataOTA] Upload FAILED");
            }
        }
    });
}