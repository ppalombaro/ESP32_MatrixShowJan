#include "ContentManager.h"

ContentManager& ContentManager::instance() {
    static ContentManager inst;
    return inst;
}

ContentManager::ContentManager() : _nextId(0) {}

ContentManager::~ContentManager() {}

void ContentManager::begin() {
    // initialize content system
}

void ContentManager::discoverFFATScenes(const String& rootPath) {
    // implementation
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

const SceneEntry* ContentManager::getSceneById(uint16_t id) const {
    for (auto &scene : _scenes) {
        if (scene.id == id) return &scene;
    }
    return nullptr;
}

const SceneEntry* ContentManager::getSceneByIndex(size_t index) const {
    if (index < _scenes.size()) return &_scenes[index];
    return nullptr;
}

std::vector<const SceneEntry*> ContentManager::getAllScenes() const {
    std::vector<const SceneEntry*> result;
    for (auto &scene : _scenes) result.push_back(&scene);
    return result;
}

std::vector<const SceneEntry*> ContentManager::getScenesForTheme(ThemeId theme) const {
    std::vector<const SceneEntry*> result;
    for (auto &scene : _scenes) if (scene.theme == theme) result.push_back(&scene);
    return result;
}

void ContentManager::update() {
    // optional tick logic for .ino loop
}
