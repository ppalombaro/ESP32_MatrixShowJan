#pragma once
#include <Arduino.h>
#include "ContentManager.h"
#include "ThemeManager.h"
#include "Logger.h"

#define WEBPAGES_VERSION "15.4FIX"

class WebPages {
public:
    static WebPages& instance();

    void begin();

    // Build main HTML page dynamically
    String buildMainPage();

private:
    WebPages();
    ~WebPages();

    WebPages(const WebPages&) = delete;
    WebPages& operator=(const WebPages&) = delete;

    // Helpers
    String buildPreviewDropdown();
    String buildBrightnessSlider();
    String buildScheduleSection();

    uint8_t _brightnessPercent;
    uint8_t _schedHour;
    uint8_t _schedMinute;
};
