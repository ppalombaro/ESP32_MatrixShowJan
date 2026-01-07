#include "SceneData.h"

SceneData::SceneData(const SceneEntry* scene)
: _scene(scene)
{
}

SceneData::~SceneData() {
    // Free JSON documents
    for (auto& f : _frames) {
        if (f.jsonDoc) {
            delete f.jsonDoc;
            f.jsonDoc = nullptr;
        }
    }
    _frames.clear();
}

bool SceneData::load() {
    if (!_scene) return false;

    _frames.clear();

    if (_scene->source == SceneSourceType::CODE_SCENE) {
        // Nothing to load for code-based scenes
        return true;
    }

    if (_scene->filePath.length() == 0) {
        Logger::instance().logf("[SceneData] Scene has empty file path: %s\n",
                                 _scene->displayName.c_str());
        return false;
    }

    // Load timeline JSON
    String timelinePath = _scene->filePath;
    File timelineFile = FFat.open(timelinePath);
    if (!timelineFile) {
        Logger::instance().logf("[SceneData] Timeline file missing: %s\n",
                                 timelinePath.c_str());
        return false;
    }

    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    DeserializationError err = deserializeJson(doc, timelineFile);
    timelineFile.close();
    if (err) {
        Logger::instance().logf("[SceneData] JSON parse error (%s): %s\n",
                                 timelinePath.c_str(),
                                 err.c_str());
        return false;
    }

    // Expecting: frames array
    if (!doc.containsKey("frames") || !doc["frames"].is<JsonArray>()) {
        Logger::instance().logf("[SceneData] Missing frames array in %s\n",
                                 timelinePath.c_str());
        return false;
    }

    JsonArray framesArray = doc["frames"].as<JsonArray>();
    for (JsonObject frameObj : framesArray) {
        String framePath = frameObj["file"] | "";
        if (framePath.length() == 0) continue;

        FrameData frame;
        if (!loadFrame(framePath, frame)) {
            Logger::instance().logf("[SceneData] Failed to load frame: %s\n",
                                     framePath.c_str());
            continue;
        }

        _frames.push_back(frame);
    }

    Logger::instance().logf("[SceneData] Loaded %u frames for %s\n",
                             (uint32_t)_frames.size(),
                             _scene->displayName.c_str());

    return _frames.size() > 0;
}

bool SceneData::loadFrame(const String& path, FrameData& outFrame) {
    File file = FFat.open(path);
    if (!file) {
        outFrame.jsonDoc = nullptr;
        return false;
    }

    DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_BUFFER_SIZE);
    DeserializationError err = deserializeJson(*doc, file);
    file.close();
    if (err) {
        delete doc;
        outFrame.jsonDoc = nullptr;
        return false;
    }

    outFrame.filePath = path;
    outFrame.jsonDoc  = doc;
    return true;
}

bool SceneData::play() {
    if (!_scene) return false;

    switch (_scene->source) {
        case SceneSourceType::CODE_SCENE:
            if (_scene->runCallback) {
                _scene->runCallback();
                return true;
            }
            break;

        case SceneSourceType::FFAT_JSON:
            for (auto& f : _frames) {
                if (!f.jsonDoc) continue;
                // call your animation engine here:
                // e.g., MatrixDisplay::instance().drawFrame(f.jsonDoc);
            }
            return true;
    }
    return false;
}

const String& SceneData::getName() const {
    static String empty;
    return _scene ? _scene->displayName : empty;
}

size_t SceneData::frameCount() const {
    return _frames.size();
}

const FrameData* SceneData::getFrame(size_t index) const {
    if (index >= _frames.size()) return nullptr;
    return &_frames[index];
}
