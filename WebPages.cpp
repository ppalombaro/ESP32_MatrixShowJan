#include "WebPages.h"

static WebPages* _instance = nullptr;

WebPages& WebPages::instance() {
    if (!_instance) _instance = new WebPages();
    return *_instance;
}

WebPages::WebPages()
: _brightnessPercent(20)
, _schedHour(0)
, _schedMinute(0)
{
}

WebPages::~WebPages() {}

void WebPages::begin() {
    // Could initialize settings from persistent storage here
}

String WebPages::buildMainPage() {
    String html;

    html += "<!DOCTYPE html><html><head><title>ESP32 Matrix Show</title></head><body>";

    // Preview section
    html += "<h2>Preview Scene</h2>";
    html += buildPreviewDropdown();
    html += "<button onclick=\"previewScene()\">Run Preview</button>";

    // Brightness section
    html += "<h2>Brightness</h2>";
    html += buildBrightnessSlider();

    // Schedule section
    html += "<h2>Schedule</h2>";
    html += buildScheduleSection();

    html += "<script>"
            "function previewScene(){"
            " var select = document.getElementById('previewSelect');"
            " var id = select.value;"
            " fetch('/preview?id='+id);"
            "}"
            "</script>";

    html += "</body></html>";

    return html;
}

// ------------------------------------------------------------
// Build Preview Dropdown using ContentManager
// ------------------------------------------------------------
String WebPages::buildPreviewDropdown() {
    String html = "<select id='previewSelect'>";

    auto scenes = ContentManager::instance().getAllScenes();
    for (const auto* s : scenes) {
        html += "<option value='" + String(s->id) + "'>" +
                String(s->id) + " – " +
                s->displayName + "</option>";
    }

    html += "</select>";
    return html;
}

// ------------------------------------------------------------
// Build Brightness Slider (sticky)
// ------------------------------------------------------------
String WebPages::buildBrightnessSlider() {
    String html;
    html += "<input type='range' min='0' max='100' value='" +
            String(_brightnessPercent) +
            "' oninput='updateBrightness(this.value)' id='brightnessSlider'>";
    html += "<span id='brightnessValue'>" + String(_brightnessPercent) + "%</span>";
    html += "<script>"
            "function updateBrightness(val){"
            " document.getElementById('brightnessValue').innerText = val+'%';"
            " fetch('/brightness?percent='+val);"
            "}"
            "</script>";
    return html;
}

// ------------------------------------------------------------
// Build Schedule Section (sticky, always visible)
// ------------------------------------------------------------
String WebPages::buildScheduleSection() {
    String html;
    html += "<label>Hour: <input type='number' id='schedHour' min='0' max='23' value='" +
            String(_schedHour) + "'></label>";
    html += "<label>Minute: <input type='number' id='schedMinute' min='0' max='59' value='" +
            String(_schedMinute) + "'></label>";
    html += "<button onclick='setSchedule()'>Set Schedule</button>";

    html += "<script>"
            "function setSchedule(){"
            " var h = document.getElementById('schedHour').value;"
            " var m = document.getElementById('schedMinute').value;"
            " fetch('/schedule/set?hour='+h+'&minute='+m);"
            "}"
            "</script>";

    return html;
}
