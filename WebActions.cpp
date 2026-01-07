#include "WebActions.h"
#include "Config.h"   // <-- REQUIRED for THEME_OFF

void WebActions::setMode(uint8_t mode) {
    themeMgr->setTheme(mode);
}

void WebActions::setScene(int index) {
    themeMgr->setSpecificScene(index);
}

void WebActions::setAnimation(int index) {
    themeMgr->setSpecificAnimation(index);
}

void WebActions::setTest(int index) {
    themeMgr->setSpecificTest(index);
}

void WebActions::setBrightness(uint8_t value) {
    themeMgr->setBrightness(value);
}

void WebActions::clearDisplay() {
    themeMgr->setTheme(THEME_OFF);
}
