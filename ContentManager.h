#pragma once

#include <Arduino.h>
#include <vector>
#include <FFat.h>
#include "Logger.h"

// Forward declarations
class SceneEntry; // optional, already struct
enum class ThemeId;
enum class SceneSourceType;

enum class SceneSourceType {
    FFAT_JSON,
    CODE_SCENE
};

enum class ThemeId {
    NONE,
    HALLOWEEN,
    NEWYEAR,
    OSU
};

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
    static ContentManager& instance();

    void begin();

    // Discovery / registration
    void discoverFFATScenes(const String& rootPath);
    void registerCodeScene(const String& displayName, ThemeId theme, void (*callback)());

    // Accessors
    const SceneEntry* getSceneById(uint16_t id) const;
    const SceneEntry* getSceneByIndex(size_t index) const;
    std::vector<const SceneEntry*> getAllScenes() const;
    std::vector<const SceneEntry*> getScenesForTheme(ThemeId theme) const;

private:
    ContentManager();
    ~ContentManager();

    std::vector<SceneEntry> _scenes;
    uint16_t _nextId;

    ContentManager(const ContentManager&) = delete;
    ContentManager& operator=(const ContentManager&) = delete;
};
