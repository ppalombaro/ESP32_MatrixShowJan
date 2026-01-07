#include "SceneData.h"
#include <FFat.h>

SceneData::SceneData(const String& filePath)
    : _filePath(filePath) {}

bool SceneData::load() {
    if (_filePath.length() == 0) {
        Logger::instance().logf("[SceneData] Scene has empty file path");
        return false;
    }

    File file = FFat.open(_filePath, "r");
    if (!file) {
        Logger::instance().logf("[SceneData] Scene file missing: %s", _filePath.c_str());
        return false;
    }

    const size_t bufferSize = file.size() + 1;
    std::unique_ptr<char[]> buf(new char[bufferSize]);
    file.readBytes(buf.get(), bufferSize);
    file.close();

    DynamicJsonDocument doc(4096);
    auto err = deserializeJson(doc, buf.get());
    if (err) {
        Logger::instance().logf("[SceneData] JSON parse error: %s", err.c_str());
        return false;
    }

    if (!doc.containsKey("frames")) {
        Logger::instance().logf("[SceneData] Missing frames array in %s", _filePath.c_str());
        return false;
    }

    _frames.clear();
    for (auto&& frame : doc["frames"].as<JsonArray>()) {
        FrameData f;
        f.durationMs = frame["duration"];
        f.content = frame["content"].as<String>();
        _frames.push_back(f);
    }

    Logger::instance().logf("[SceneData] Loaded %u frames for %s", (unsigned)_frames.size(), _filePath.c_str());
    return true;
}
