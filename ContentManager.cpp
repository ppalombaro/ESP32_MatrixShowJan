#include "ContentManager.h"

void ContentManager::begin() {
    Logger::instance().log("[ContentManager] Initialization complete.");
}

void ContentManager::discoverFFATScenes(const String& rootPath) {
    File dir = FFat.open(rootPath);
    if (!dir || !dir.isDirectory()) return;

    File file = dir.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            SceneEntry entry;
            entry.id = _nextId++;
            entry.filePath = String(file.name());
            entry.displayName = file.name();
            entry.source = SceneSourceType::FFAT_JSON;
            entry.theme = ThemeId::NONE;
            _scenes.push_back(entry);
        }
        file = dir.openNextFile();
    }
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

const SceneEntry* ContentManager::getSceneById(uint16_t id) const {
    for (auto& scene : _scenes) {
        if (scene.id == id) return &scene;
    }
    return nullptr;
}

std::vector<const SceneEntry*> ContentManager::getScenesForTheme(ThemeId theme) const {
    std::vector<const SceneEntry*> result;
    for (auto& scene : _scenes) {
        if (scene.theme == theme) result.push_back(&scene);
    }
    return result;
}
