/* ContentManager.h
   Content discovery and rendering system
   VERSION: V16.2.5-2026-01-10T22:05:00Z - Fixed header to match .cpp implementation
*/

#pragma once

#include <Arduino.h>
#include <vector>

// V16.2.5-2026-01-10T22:05:00Z - Forward declarations
class MatrixDisplay;
class Scroll;      // V16.4.12
class Countdown;   // V16.4.12

// V16.2.0 - Content type enumeration
enum ContentType {
    CONTENT_SCENE,
    CONTENT_ANIMATION,
    CONTENT_SCROLL,
    CONTENT_COUNTDOWN,
    CONTENT_PROCEDURAL,  // V16.2.0 - Procedural animations
    CONTENT_TEST         // Test patterns
};

// V16.2.0 - Content item structure
struct ContentItem {
    uint16_t id;
    String name;
    String theme;
    ContentType type;
    String path;
    unsigned long durationMs;     // V16.3.0-2026-01-10T22:40:00Z - Display duration
    String matrix0Scene;          // V16.3.0-2026-01-10T22:40:00Z - Scene for matrix 0
    String matrix1Scene;          // V16.3.0-2026-01-10T22:40:00Z - Scene for matrix 1 (default: mirror matrix0)
    String matrix2Scene;          // V16.3.0-2026-01-10T22:40:00Z - Scene for matrix 2 (optional)
};

// V16.1.3-2026-01-09T05:00:00Z - File entry structure for flash storage
struct FileEntry {
    String path;
    uint32_t offset;
    uint32_t size;
};

// V16.4.12 - One parsed timeline frame for non-blocking animation playback
struct PlaybackFrame {
    String sceneRef;      // frame["scene"] - same art on both windows (empty if per-window)
    String matrix0Scene;  // frame["matrix0Scene"] - right window
    String matrix1Scene;  // frame["matrix1Scene"] - left window
    unsigned long durationMs;
};

class ContentManager {
public:
    ContentManager();
    
    void begin(MatrixDisplay* display);
    void update();
    
    // Content access
    const std::vector<ContentItem>& getContent() const;
    const ContentItem* getContentById(uint16_t id) const;
    std::vector<ContentItem> getContentByTheme(const String& theme) const;
    const std::vector<String>& getDiscoveredThemes() const;
    
    // Content rendering
    bool renderContent(uint16_t contentId);

    // Run mode / scheduler (V16.4.13)
    void enableScheduler(bool enable);              // legacy alias -> setRunMode
    bool isSchedulerEnabled() const;               // true when runMode == SCHEDULE
    void setRunMode(uint8_t mode);
    uint8_t getRunMode() const;
    void setScheduleWindow(uint8_t sh, uint8_t sm, uint8_t eh, uint8_t em);   // persists
    void getScheduleWindow(uint8_t& sh, uint8_t& sm, uint8_t& eh, uint8_t& em) const;
    bool isScheduleActive() const { return scheduleActive; }

    // Random mode control
    void enableRandomMode(bool enable);
    bool isRandomModeEnabled() const;
    void setRandomInterval(unsigned long intervalMs);
    unsigned long getRandomInterval() const;
    void setRandomThemeFilter(const String& theme);
    String getRandomThemeFilter() const;

    // Eligible-content mask for random mode (V16.4.13)
    void setExcludedByIds(const String& csvIds);    // resolve ids -> stable keys, persist
    std::vector<String> getExcludedKeys() const;
    bool isEligible(const ContentItem& item) const;
    static String contentKey(const ContentItem& item);

    // Playback status for /api/status (V16.4.13)
    struct PlaybackStatus { int32_t id; String name; String type; String theme; bool scheduleActive; };
    PlaybackStatus getStatus() const;

    // V16.4.12 - Stop any active playback and blank the display
    void stopPlayback();

private:
    MatrixDisplay* disp = nullptr;
    Scroll* scroll = nullptr;         // V16.4.12
    Countdown* countdown = nullptr;   // V16.4.12

    std::vector<ContentItem> contentRegistry;
    std::vector<String> discoveredThemes;
    std::vector<FileEntry> fileEntries;

    uint16_t nextContentId = 1;

    // V16.4.12 - Non-blocking playback state
    int32_t activeContentId = -1;
    ContentType activeType = CONTENT_TEST;
    String activePath;                       // timeline path (resolveScenePath base)
    String proceduralName;
    std::vector<PlaybackFrame> playFrames;
    size_t playFrameIdx = 0;
    unsigned long playFrameStart = 0;
    bool playLoop = false;

    void updatePlayback();
    void drawPlayFrame(size_t idx);
    void runProceduralTick(const String& name);
    
    bool randomModeEnabled = false;
    unsigned long randomIntervalMs = 4000;  // V16.4.8-2026-01-11T22:30:00Z - Default 4 seconds
    unsigned long lastRandomChange = 0;
    String randomThemeFilter = "";

    // V16.4.13 - scheduler state (persisted to NVS namespace show-config)
    uint8_t runMode = 0;                    // RUN_MODE_MANUAL
    uint8_t schStartH = 17, schStartM = 0;
    uint8_t schEndH = 22,  schEndM = 0;
    bool scheduleActive = true;
    bool scheduleBlanked = false;
    bool loggedTimeUnsynced = false;
    std::vector<String> excludedKeys;       // stable content keys excluded from random rotation

    bool computeInWindow();
    void loadScheduleFromNVS();
    void saveScheduleToNVS();
    void loadEligibleFromNVS();
    void saveEligibleToNVS();
    void loadRandomFromNVS();
    void saveRandomToNVS();

    // Content registration
    void addContent(const String& name, const String& theme, ContentType type, const String& path, unsigned long duration, const String& m0, const String& m1, const String& m2);
    void addContent(const String& name, const String& theme, ContentType type, const String& path);  // V16.3.0 - Backward compat
    void registerProceduralAnimations();

    // Storage reading
    bool readCustomStorage();
    String extractTheme(const String& path);
    String resolveScenePath(const String& timelinePath, const String& sceneName);  // V16.4.10-2026-01-12T03:05:00Z

    // V16.4.11 - Scene pixel rendering
    const FileEntry* findFile(const String& path) const;
    bool drawSceneFile(int matrix, const String& sceneRef, const String& basePath);
    
    // Random mode
    void updateRandomMode();
    void selectRandomContent();
};