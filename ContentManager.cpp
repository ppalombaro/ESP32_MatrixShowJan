#include "ContentManager.h"

static ContentManager* _instance = nullptr;

ContentManager& ContentManager::instance() {
    if (!_instance) _instance = new ContentManager();
    return *_instance;
}

ContentManager::ContentManager()
: _nextId(0) {}

ContentManager::~ContentManager() {}

void ContentManager::begin() {
    _scenes.clear();
    _nextId = 0;
}

void ContentManager::discoverFFATScenes(const String& rootPath) {
        File root = FFat.open(rootPath);
    if (!root || !root.isDirectory()) {
        Logger::instance().logf("Invalid FFAT path: %s", rootPath.c_str());
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String name = file.name();
            // process JSON scene file here
        }
        file = root.openNextFile();
    }
}

void ContentManager::registerCodeScene(const String& displayName, ThemeId theme, void (*callback)()) {
    SceneEntry entry;
    entry.id = _nextId++;
    entry.displayName = displayName;
    entry.source = SceneSourceType::CODE_SCENE;
    entry.filePath = "";
    entry.theme = theme;
    entry.runCallback = callback;
    _scenes.push_back(entry);
}

const SceneEntry* ContentManager::getSceneById(uint16_t id) const {
    for (auto& s : _scenes) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

const SceneEntry* ContentManager::getSceneByIndex(size_t index) const {
    if (index >= _scenes.size()) return nullptr;
    return &_scenes[index];
}

std::vector<const SceneEntry*> ContentManager::getAllScenes() const {
    std::vector<const SceneEntry*> out;
    for (auto& s : _scenes) out.push_back(&s);
    return out;
}

std::vector<const SceneEntry*> ContentManager::getScenesForTheme(ThemeId theme) const {
    std::vector<const SceneEntry*> out;
    for (auto& s : _scenes) {
        if (s.theme == theme || theme == ThemeId::NONE) out.push_back(&s);
    }
    return out;
}
