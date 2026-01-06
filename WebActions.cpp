/* WebActions.cpp
   Web request handlers - separated from HTML generation
   VERSION: V15.1.0-2026-01-04T10:20:00Z - Split from WebController
   
   V15.1.0-2026-01-04T10:20:00Z - Clean separation of actions from pages
*/

#include "WebActions.h"

extern Logger logger;  // V15.1.0-2026-01-04T10:20:00Z

WebActions::WebActions(ThemeManager* tm, ContentManager* cm) 
    : themeMgr(tm), contentMgr(cm) {
}

void WebActions::setMode(uint8_t theme) {
    // V15.1.0-2026-01-04T10:20:00Z - Set display theme
    themeMgr->setTheme(theme);
    logger.log("Theme set to: " + String(theme));
}

void WebActions::setScene(int index) {
    // V15.1.0-2026-01-04T10:20:00Z - Set specific scene by auto-discovered index
    ContentItem* item = contentMgr->getContentByIndex("scene", index);
    if (item) {
        themeMgr->setSpecificScene(index);
        logger.log("Scene set: " + item->displayName);
    } else {
        logger.log("Invalid scene index: " + String(index));
    }
}

void WebActions::setAnimation(int index) {
    // V15.1.0-2026-01-04T10:20:00Z - Set specific animation
    ContentItem* item = contentMgr->getContentByIndex("animation", index);
    if (item) {
        themeMgr->setSpecificAnimation(index);
        logger.log("Animation set: " + item->displayName);
    } else {
        logger.log("Invalid animation index: " + String(index));
    }
}

void WebActions::setTest(int index) {
    // V15.1.0-2026-01-04T10:20:00Z - Set test pattern
    ContentItem* item = contentMgr->getContentByIndex("test", index);
    if (item) {
        themeMgr->setSpecificTest(index);
        logger.log("Test pattern set: " + item->displayName);
    } else {
        logger.log("Invalid test index: " + String(index));
    }
}

void WebActions::setBrightness(uint8_t value) {
    // V15.1.0-2026-01-04T10:20:00Z - Adjust display brightness
    themeMgr->setBrightness(value);
    logger.log("Brightness set to: " + String(value));
}

void WebActions::clearDisplay() {
    // V15.1.0-2026-01-04T10:20:00Z - Turn off display
    themeMgr->setTheme(THEME_OFF);
    logger.log("Display cleared");
}

void WebActions::saveScheduleConfig(const ScheduleConfig& config) {
    // V15.1.0-2026-01-04T10:20:00Z - Save schedule to FFat
    if (contentMgr->saveSchedule(config)) {
        logger.log("Schedule configuration saved");
    } else {
        logger.log("Failed to save schedule");
    }
}

void WebActions::saveRandomConfig(const std::vector<String>& enabledContent) {
    // V15.1.0-2026-01-04T10:20:00Z - Update allowed content list
    ScheduleConfig config;
    contentMgr->loadSchedule(config);
    
    config.allowedContent = enabledContent;
    
    if (contentMgr->saveSchedule(config)) {
        logger.log("Random configuration saved (" + String(enabledContent.size()) + " items)");
    } else {
        logger.log("Failed to save random config");
    }
}

void WebActions::setChristmasCountdown(uint16_t year, uint8_t month, uint8_t day, 
                                       uint8_t hour, uint8_t minute) {
    // V15.1.0-2026-01-04T10:20:00Z - Configure Christmas countdown target
    themeMgr->setChristmasCountdownDate(year, month, day, hour, minute);
    logger.log("Christmas countdown set: " + String(year) + "-" + String(month) + "-" + String(day));
}

void WebActions::setNewYearCountdown(uint16_t year, uint8_t month, uint8_t day,
                                    uint8_t hour, uint8_t minute) {
    // V15.1.0-2026-01-04T10:20:00Z - Configure New Year countdown target
    themeMgr->setNewYearCountdownDate(year, month, day, hour, minute);
    logger.log("New Year countdown set: " + String(year) + "-" + String(month) + "-" + String(day));
}

void WebActions::resetCountdowns() {
    // V15.1.0-2026-01-04T10:20:00Z - Reset countdown dates to defaults
    themeMgr->resetCountdownDates();
    logger.log("Countdown dates reset to defaults");
}

ScheduleConfig WebActions::getCurrentSchedule() {
    // V15.1.0-2026-01-04T10:20:00Z - Load current schedule configuration
    ScheduleConfig config;
    contentMgr->loadSchedule(config);
    return config;
}
