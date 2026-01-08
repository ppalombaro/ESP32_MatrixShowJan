#pragma once
#include <Arduino.h>

class ThemeManager;

class WebActions {
public:
    explicit WebActions(ThemeManager* tm);

    void attach();

private:
    ThemeManager* themeMgr;
};
