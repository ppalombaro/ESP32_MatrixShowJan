/* WebController.h
   Web server and HTTP interface
   VERSION: V16.1.3-2026-01-09T05:35:00Z - Fixed WebActions lifecycle
*/

#pragma once

#include <Arduino.h>
#include <WebServer.h>

class ContentManager;
class ThemeManager;
class MatrixDisplay;
class WebActions;

class WebController {
public:
    WebController();

    void begin(ContentManager* contentMgr, ThemeManager* themeMgr, MatrixDisplay* disp);
    void handle();

private:
    WebServer server{80};
    ContentManager* content = nullptr;
    ThemeManager* themes = nullptr;
    MatrixDisplay* display = nullptr;
    WebActions* actions = nullptr;  // V16.1.3-2026-01-09T05:35:00Z
    bool uploadAuthOk = false;      // V16.4.14 - gates the current /update file upload

    // V16.4.15 - web-based upload of the data partition (scenes/animations/etc,
    // ffat.bin), alongside the existing /update firmware route. Update.h only
    // targets the app partition, so this writes the raw "ffat" data partition
    // directly via esp_partition_*.
    bool dataUploadAuthOk = false;
    const void* dataPartition = nullptr;  // const esp_partition_t*, opaque here to avoid the include in the header
    size_t dataWriteOffset = 0;
    bool dataUploadFailed = false;

    void setupRoutes();
    void setupOtaRoute();           // V16.4.14 - web-based firmware upload, independent of ContentManager state
    void setupDataOtaRoute();       // V16.4.15 - web-based data-partition upload (ffat.bin)
};