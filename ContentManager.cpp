#include "ContentManager.h"

ContentManager& ContentManager::instance() {
    static ContentManager _instance;
    return _instance;
}

ContentManager::ContentManager()
    : _nextId(0)
{}

ContentManager::~ContentManager() {}

void ContentManager::begin() {
    // No return value; previous bool mismatch removed
    discoverFFATScenes("/scenes");
}

// Discovery
void ContentManager::discoverFFATScenes(const String& rootPath) {
    // Example: scan FFAT directory for JSON scenes
}

void ContentManager::registerCodeScene(const String& displayName, ThemeId theme, void (*callback)()) {
    SceneEntry entry;
    entry.id = _nextId++;
    entry.displayName = displayName;
    entry.theme = theme;
    entry.source = SceneSourceType::CODE_SCENE;
    entry.runCallback = callback;
    _scenes.push_back(entry);
}

// Accessors
const SceneEntry* ContentManager::getSceneById(uint16_t id) const {
    for (const auto& s : _scenes) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

const SceneEntry* ContentManager::getSceneByIndex(size_t index) const {
    if (index < _scenes.size()) return &_scenes[index];
    return nullptr;
}

std::vector<const SceneEntry*> ContentManager::getAllScenes() const {
    std::vector<const SceneEntry*> out;
    for (const auto& s : _scenes) out.push_back(&s);
    return out;
}

std::vector<const SceneEntry*> ContentManager::getScenesForTheme(ThemeId theme) const {
    std::vector<const SceneEntry*> out;
    for (const auto& s : _scenes) {
        if (s.theme == theme) out.push_back(&s);
    }
    return out;
}
