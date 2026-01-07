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
    Dir dir = FFat.openDir(rootPath);
    while (dir.next()) {
        String path = dir.fileName();
        if (path.endsWith(".json")) {
            SceneEntry entry;
            entry.id = _nextId++;
            entry.displayName = path.substring(path.lastIndexOf('/')+1);
            entry.source = SceneSourceType::FFAT_JSON;
            entry.filePath = path;
            // infer theme from folder name
            if (path.indexOf("halloween") >= 0) entry.theme = ThemeId::HALLOWEEN;
            else if (path.indexOf("newyear") >= 0) entry.theme = ThemeId::NEWYEAR;
            else entry.theme = ThemeId::NONE;

            _scenes.push_back(entry);
        }
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
