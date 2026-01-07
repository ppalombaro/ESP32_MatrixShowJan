#pragma once
#include <Arduino.h>
#include <vector>
#include <FFat.h>
#include "Logger.h"

enum class SceneSourceType { FFAT_JSON, CODE_SCENE };
enum class ThemeId { NONE, HALLOWEEN, NEWYEAR, OSU };

struct SceneEntry {
    uint16_t id;
    String displayName;
    String filePath;
    SceneSourceType source;
    ThemeId theme;
    void (*runCallback)() = nullptr;
};

class ContentManager {
public:
    static ContentManager& instance() {
        static ContentManager _instance;
        return _instance;
    }

    void begin();
    void discoverFFATScenes(const String& rootPath);
    void registerCodeScene(const String& displayName, ThemeId theme, void (*callback)());
    const SceneEntry* getSceneById(uint16_t id) const;
    std::vector<const SceneEntry*> getScenesForTheme(ThemeId theme) const;
    const std::vector<SceneEntry>& getAllScenes() const { return _scenes; }

private:
    ContentManager() : _nextId(0) {}
    ~ContentManager() = default;
    ContentManager(const ContentManager&) = delete;
    ContentManager& operator=(const ContentManager&) = delete;

    std::vector<SceneEntry> _scenes;
    uint16_t _nextId;
};
