#pragma once

#include <Arduino.h>
#include <vector>
#include "ThemeManager.h"
#include "ContentManager.h"

class WebActions {
public:
    WebActions(ContentManager* cm, ThemeManager* tm)
        : contentMgr(cm), themeMgr(tm) {}

    void setMode(uint8_t mode);
    void setScene(int index);
    void setAnimation(int index);
    void setTest(int index);
    void setBrightness(uint8_t value);
    void clearDisplay();

private:
    ContentManager* contentMgr;
    ThemeManager* themeMgr;
};
