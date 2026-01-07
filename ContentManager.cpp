#include "ContentManager.h"

// Singleton instance
ContentManager& ContentManager::instance() {
    static ContentManager _instance;
    return _instance;
}

ContentManager::ContentManager() : _nextId(0) {
    // optional init logic
}

ContentManager::~ContentManager() {
    // optional cleanup
}

void ContentManager::begin() {
    // existing init logic
}

bool ContentManager::begin(bool verbose) {
    begin();   // call existing init
    return true; // allow usage in if (!content.begin())
}

// -----------------------
// Scene discovery / registration
// -----------------------

void ContentManager::discoverFFATScenes(const String& rootPath) {
    // existing FFAT discovery logic
}

void ContentManager::registerCodeScene(const String& displayName, ThemeId theme, void (*callback)()) {
    SceneEntry entry;
    entry.id = _nextId++;
    entry.displayName = displayName;
    entry.source = SceneSourceType::CODE_SCENE;
    entry.theme = theme;
    entry.runCallback = callback;
    _scenes.push_back(entry);
}

// -----------------------
// Accessors
// -----------------------

const SceneEntry* ContentManager::getSceneById(uint16_t id) const {
    for (const auto& scene : _scenes) {
        if (scene.id == id) return &scene;
    }
    return nullptr;
}

const SceneEntry* ContentManager::getSceneByIndex(size_t index) const {
    if (index >= _scenes.size()) return nullptr;
    return &_scenes[index];
}

std::vector<const SceneEntry*> ContentManager::getAllScenes() const {
    std::vector<const SceneEntry*> result;
    for (const auto& scene : _scenes) result.push_back(&scene);
    return result;
}

std::vector<const SceneEntry*> ContentManager::getScenesForTheme(ThemeId theme) const {
    std::vector<const SceneEntry*> result;
    for (const auto& scene : _scenes) {
        if (scene.theme == theme) result.push_back(&scene);
    }
    return result;
}
