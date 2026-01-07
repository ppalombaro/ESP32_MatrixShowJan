#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "ContentManager.h"
#include "Logger.h"
#include <FFat.h>

#define SCENEDATA_VERSION "15.4FIX"

struct FrameData {
    String filePath;       // FFAT JSON frame
    JsonDocument* jsonDoc; // pointer to loaded JSON (nullptr if not loaded)
};

class SceneData {
public:
    SceneData(const SceneEntry* scene);
    ~SceneData();

    // Load scene timeline JSON files (frames)
    bool load();

    // Play the loaded frames using your animation engine
    bool play();

    // Accessors
    const String& getName() const;
    size_t frameCount() const;
    const FrameData* getFrame(size_t index) const;

private:
    const SceneEntry* _scene;
    std::vector<FrameData> _frames;

    bool loadFrame(const String& path, FrameData& outFrame);

    // JSON buffer pool
    static const size_t JSON_BUFFER_SIZE = 2048; // adjust per frame size
};
