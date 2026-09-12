/* ContentManager.cpp
   VERSION: V16.2.1-2026-01-10T18:50:00Z - Implemented renderContent with actual display logic
*/

#include "ContentManager.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "Animations.h"
#include "Scroll.h"
#include "Countdown.h"
#include "ThemeManager.h"
#include "Config.h"
#include <vector>
#include <time.h>
#include <Preferences.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include <ArduinoJson.h>  // V16.3.0-2026-01-10T22:42:00Z

// V16.2.5-2026-01-10T22:19:00Z - External references
extern ThemeManager themeManager;

// V16.4.13 - local wall-clock (TZ set via configTzTime in main.cpp).
// Returns false until SNTP first syncs.
static bool nowLocal(struct tm& out) {
    return getLocalTime(&out, 50);
}

// Custom storage parameters
#define DATA_PARTITION_OFFSET 0x290000
#define DATA_AREA_START 0  // V16.1.3-2026-01-09T05:15:00Z - Simple storage starts at offset 0

ContentManager::ContentManager() {
    Serial.println("DEBUG: ContentManager constructor");
}

void ContentManager::begin(MatrixDisplay* display) {
    Logger::instance().log("DEBUG: ContentManager.begin() ENTRY");
    
    disp = display;
    contentRegistry.clear();
    discoveredThemes.clear();

    // V16.4.12 - Non-blocking scroll / countdown players (read straight from the blob)
    if (!scroll)    scroll    = new Scroll(disp, &themeManager);
    if (!countdown) countdown = new Countdown(disp, &themeManager);

    Logger::instance().log("[ContentManager] Reading custom flash storage...");

    // Read file index from flash
    if (!readCustomStorage()) {
        Logger::instance().log("[ContentManager] Custom storage read FAILED");
        // Continue with procedural content only
    }

    Logger::instance().log("[ContentManager] Discovered " + String(discoveredThemes.size()) + " themes");

    registerProceduralAnimations();

    // V16.4.13 - restore persisted schedule + eligible-content state
    loadScheduleFromNVS();
    loadEligibleFromNVS();

    Logger::instance().log("[ContentManager] Total content: " + String(contentRegistry.size()));
}

bool ContentManager::readCustomStorage() {
    // V16.4.4-2026-01-11T22:05:00Z - Use Logger for web visibility
    uint32_t flash_addr = DATA_PARTITION_OFFSET + DATA_AREA_START;
    
    Logger::instance().log("[ContentManager] Reading flash at 0x" + String(flash_addr, HEX));
    
    // Read file count
    uint32_t file_count = 0;
    esp_err_t err = esp_flash_read(NULL, &file_count, flash_addr, 4);
    
    Logger::instance().log("[ContentManager] Flash read returned: " + String(err));
    Logger::instance().log("[ContentManager] File count read: " + String(file_count) + " (0x" + String(file_count, HEX) + ")");
    
    if (err != ESP_OK) {
        Logger::instance().log("ERROR: Cannot read file count from flash (error " + String(err) + ")");
        return false;
    }
    
    Logger::instance().log("[ContentManager] Files in storage: " + String(file_count));
    
    if (file_count == 0 || file_count > 500) {
        Logger::instance().log("ERROR: Invalid file count " + String(file_count) + " (0x" + String(file_count, HEX) + ")");
        return false;
    }
    
    flash_addr += 4;  // Skip file count
    
    Logger::instance().log("[ContentManager] Starting file parsing loop...");
    
    // Read each file entry
    for (uint32_t i = 0; i < file_count; i++) {
        // Read path length (2 bytes)
        uint16_t path_len = 0;
        esp_err_t err = esp_flash_read(NULL, &path_len, flash_addr, 2);
        if (err != ESP_OK) {
            Logger::instance().log("ERROR: Failed to read path_len at file " + String(i) + ", error " + String(err));
            break;
        }
        flash_addr += 2;
        
        if (path_len == 0 || path_len > 255) {
            Logger::instance().log("ERROR: Invalid path_len " + String(path_len) + " at file " + String(i));
            break;
        }
        
        // Read path
        char path_buf[256];
        err = esp_flash_read(NULL, path_buf, flash_addr, path_len);
        if (err != ESP_OK) {
            Logger::instance().log("ERROR: Failed to read path at file " + String(i) + ", error " + String(err));
            break;
        }
        flash_addr += path_len;
        path_buf[path_len] = '\0';
        String path = String(path_buf);
        
        // Read content length (4 bytes)
        uint32_t content_len = 0;
        err = esp_flash_read(NULL, &content_len, flash_addr, 4);
        if (err != ESP_OK) {
            Logger::instance().log("ERROR: Failed to read content_len at file " + String(i) + ", error " + String(err));
            break;
        }
        flash_addr += 4;
        
        // Store file info (we'll read content on-demand later)
        FileEntry entry;
        entry.path = path;
        entry.offset = flash_addr;
        entry.size = content_len;
        fileEntries.push_back(entry);
        
        Logger::instance().log("  Found: " + path + " (" + String(content_len) + " bytes)");
        
        // Extract theme from path
        if (path.startsWith("scenes/") || path.startsWith("animations/")) {
            int slash1 = path.indexOf('/');
            int slash2 = path.indexOf('/', slash1 + 1);
            if (slash2 > 0) {
                String theme = path.substring(slash1 + 1, slash2);
                
                bool found = false;
                for (const auto& t : discoveredThemes) {
                    if (t == theme) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    discoveredThemes.push_back(theme);
                    Logger::instance().log("[ContentManager] Theme: " + theme);
                }
            }
        }
        
        // Register content based on type
        if (path.startsWith("scenes/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:52:00Z - Save offset/size before reading
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:41:00Z - Read JSON to get duration and matrix assignments
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(1024);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;  // Default 5 seconds
                    String m0 = doc["matrix0Scene"] | path;
                    String m1 = doc["matrix1Scene"] | m0;  // Default mirror
                    String m2 = doc["matrix2Scene"] | String("");
                    addContent(filename, theme, CONTENT_SCENE, path, duration, m0, m1, m2);
                } else {
                    addContent(filename, theme, CONTENT_SCENE, path);
                }
                delete[] json_content;  // V16.3.0-2026-01-10T22:55:00Z - Move inside read block
            } else {
                delete[] json_content;
                addContent(filename, theme, CONTENT_SCENE, path);  // Fallback
            }
        }
        else if (path.indexOf("animation_timeline.json") > 0 || path.indexOf("_timeline.json") > 0) {
            int lastSlash = path.lastIndexOf('/');
            String animName = path.substring(0, lastSlash);
            animName = animName.substring(animName.lastIndexOf('/') + 1);
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:52:00Z - Save offset/size before reading
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.4.10-2026-01-12T03:00:00Z - Read timeline JSON for duration
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(8192);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    // V16.4.10 - Calculate duration from sum of frames if not explicitly set
                    unsigned long duration = doc["durationMs"] | 0;
                    if (duration == 0 && doc.containsKey("frames")) {
                        JsonArray frames = doc["frames"];
                        for (JsonObject frame : frames) {
                            duration += frame["durationMs"] | 100;  // Default 100ms per frame
                        }
                    }
                    if (duration == 0) duration = 5000;  // Final fallback
                    
                    String m0 = doc["matrix0Scene"] | path;
                    String m1 = doc["matrix1Scene"] | m0;
                    String m2 = doc["matrix2Scene"] | String("");
                    addContent(animName, theme, CONTENT_ANIMATION, path, duration, m0, m1, m2);
                } else {
                    addContent(animName, theme, CONTENT_ANIMATION, path);
                }
                delete[] json_content;
            } else {
                delete[] json_content;
                addContent(animName, theme, CONTENT_ANIMATION, path);
            }
        }
        // V16.2.0-2026-01-10T18:10:00Z - Add scroll discovery
        else if (path.startsWith("scroll/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:53:00Z - Save offset/size
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:45:00Z - Parse scroll duration
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(1024);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;
                    addContent(filename, theme, CONTENT_SCROLL, path, duration, path, path, "");
                } else {
                    addContent(filename, theme, CONTENT_SCROLL, path);
                }
                delete[] json_content;
            } else {
                delete[] json_content;
                addContent(filename, theme, CONTENT_SCROLL, path);
            }
        }
        // V16.2.0-2026-01-10T18:10:00Z - Add countdown discovery
        else if (path.startsWith("countdown/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:53:00Z - Save offset/size
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:45:00Z - Parse countdown duration
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(1024);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;
                    addContent(filename, theme, CONTENT_COUNTDOWN, path, duration, path, path, "");
                } else {
                    addContent(filename, theme, CONTENT_COUNTDOWN, path);
                }
                delete[] json_content;
            } else {
                delete[] json_content;
                addContent(filename, theme, CONTENT_COUNTDOWN, path);
            }
        }
        // V16.4.8-2026-01-11T22:30:00Z - Add test pattern discovery
        else if (path.startsWith("test/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            
            // V16.4.8 - Test patterns get 5 second default duration
            addContent(filename, "test", CONTENT_TEST, path, 5000, path, path, "");
        }
        
        // V16.4.6-2026-01-11T22:20:00Z - CRITICAL FIX: Move flash_addr advance OUTSIDE all conditionals
        // Skip to next file (align to 512 bytes)
        flash_addr += content_len;
        uint32_t padding = (512 - (flash_addr % 512)) % 512;
        flash_addr += padding;
    }
    
    Logger::instance().log("[ContentManager] Loop complete, found " + String(fileEntries.size()) + " files");
    return fileEntries.size() > 0;
}

String ContentManager::resolveScenePath(const String& timelinePath, const String& sceneName) {
    // V16.4.10-2026-01-12T03:05:00Z - Resolve relative scene paths
    // If sceneName already has path separators, return as-is (absolute path)
    if (sceneName.indexOf('/') >= 0) {
        return sceneName;
    }
    
    // Extract directory from timeline path
    // Example: "animations/halloween/spooky_eyes/eyes_timeline.json" 
    //       -> "animations/halloween/spooky_eyes/"
    int lastSlash = timelinePath.lastIndexOf('/');
    if (lastSlash > 0) {
        String dir = timelinePath.substring(0, lastSlash + 1);
        // Add .json extension if missing
        String resolved = dir + sceneName;
        if (!resolved.endsWith(".json")) {
            resolved += ".json";
        }
        return resolved;
    }
    
    // Fallback: just add .json if needed
    String resolved = sceneName;
    if (!resolved.endsWith(".json")) {
        resolved += ".json";
    }
    return resolved;
}


const FileEntry* ContentManager::findFile(const String& path) const {
    // V16.4.11 - Exact path match first, then bare-name suffix match.
    for (const auto& e : fileEntries) {
        if (e.path == path) return &e;
    }
    String suffix = path;
    if (!suffix.endsWith(".json")) suffix += ".json";
    if (suffix.indexOf('/') < 0) suffix = "/" + suffix;
    for (const auto& e : fileEntries) {
        if (e.path.endsWith(suffix)) return &e;
    }
    return nullptr;
}

bool ContentManager::drawSceneFile(int matrix, const String& sceneRef, const String& basePath) {
    // V16.4.11 - Resolve a frame/scene reference, read its JSON from flash, and
    // plot its pixels onto the given matrix. Empty ref or empty pixels = dark panel.
    if (sceneRef.length() == 0) return false;

    String resolved = resolveScenePath(basePath, sceneRef);
    const FileEntry* fe = findFile(resolved);
    if (!fe) fe = findFile(sceneRef);
    if (!fe) {
        Logger::instance().log("[ContentManager] Scene not found: " + resolved);
        return false;
    }

    char* buf = new char[fe->size + 1];
    bool ok = false;
    if (esp_flash_read(NULL, buf, fe->offset, fe->size) == ESP_OK) {
        buf[fe->size] = '\0';
        // Sized for a worst-case full-panel scene (~500 pixels * ~80 B/slot).
        DynamicJsonDocument doc(49152);
        if (deserializeJson(doc, buf) == DeserializationError::Ok) {
            int cx = doc["centerX"] | 10;
            int cy = doc["centerY"] | 12;
            JsonArray pixels = doc["pixels"];
            for (JsonObject px : pixels) {
                int x = px["x"] | 0;
                int y = px["y"] | 0;
                uint8_t r = px["r"] | 0;
                uint8_t g = px["g"] | 0;
                uint8_t b = px["b"] | 0;
                disp->setPixel(matrix, cx + x, cy + y, CRGB(r, g, b));
            }
            ok = true;
        } else {
            Logger::instance().log("[ContentManager] Scene JSON parse error: " + resolved);
        }
    }
    delete[] buf;
    return ok;
}

String ContentManager::extractTheme(const String& path) {
    // V16.2.0-2026-01-10T18:10:00Z - Extract theme from paths like "scenes/christmas/tree.json", "scroll/christmas/text.json", "countdown/christmas/newyear.json"
    int slash1 = path.indexOf('/');
    int slash2 = path.indexOf('/', slash1 + 1);
    if (slash2 > 0) {
        return path.substring(slash1 + 1, slash2);
    }
    return "unknown";
}

void ContentManager::update() {
    // V16.4.13 - schedule gate: in SCHEDULE mode, suspend everything outside the
    // daily window and blank the display.
    if (runMode == RUN_MODE_SCHEDULE) {
        bool inWindow = computeInWindow();
        scheduleActive = inWindow;
        if (!inWindow) {
            if (!scheduleBlanked) {
                stopPlayback();
                scheduleBlanked = true;
                Logger::instance().log("[Schedule] Outside window - display OFF");
            }
            return;
        }
        if (scheduleBlanked) {
            scheduleBlanked = false;
            lastRandomChange = millis();
            Logger::instance().log("[Schedule] Inside window - display ON");
            if (randomModeEnabled) selectRandomContent();
        }
    } else {
        scheduleActive = true;
    }

    if (randomModeEnabled) {
        updateRandomMode();
    }
    updatePlayback();
}

const std::vector<ContentItem>& ContentManager::getContent() const {
    return contentRegistry;
}

const ContentItem* ContentManager::getContentById(uint16_t id) const {
    for (const auto& item : contentRegistry) {
        if (item.id == id) return &item;
    }
    return nullptr;
}

std::vector<ContentItem> ContentManager::getContentByTheme(const String& theme) const {
    std::vector<ContentItem> filtered;
    for (const auto& item : contentRegistry) {
        if (item.theme == theme) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

const std::vector<String>& ContentManager::getDiscoveredThemes() const {
    return discoveredThemes;
}

bool ContentManager::renderContent(uint16_t contentId) {
    const ContentItem* item = getContentById(contentId);
    if (!item) return false;

    Logger::instance().log("[ContentManager] Rendering: " + item->name);

    // V16.4.13 - set the theme palette from the item's theme so Scroll/Countdown
    // (which read themeManager.getColorN()) render in the right colours.
    themeManager.setTheme(ThemeManager::themeNameToId(item->theme));

    // V16.4.12 - Non-blocking: set up playback state, draw the first frame,
    // and let update() advance it. Nothing here calls delay().
    playFrames.clear();
    proceduralName = "";
    activePath = item->path;
    activeType = item->type;
    activeContentId = contentId;

    switch (item->type) {
        case CONTENT_SCENE: {
            // matrix0Scene -> RIGHT window (0), matrix1Scene -> LEFT window (1).
            disp->clear();
            bool drewR = drawSceneFile(0, item->matrix0Scene, item->path);
            bool drewL = drawSceneFile(1, item->matrix1Scene, item->path);
            disp->show();
            Logger::instance().log("[ContentManager] Scene rendered: " + item->name);
            return drewR || drewL;
        }

        case CONTENT_ANIMATION: {
            const FileEntry* entry = findFile(item->path);
            if (!entry) {
                Logger::instance().log("[ContentManager] Flash entry not found: " + item->path);
                activeContentId = -1;
                return false;
            }

            char* jsonData = new char[entry->size + 1];
            if (esp_flash_read(NULL, jsonData, entry->offset, entry->size) != ESP_OK) {
                delete[] jsonData;
                activeContentId = -1;
                return false;
            }
            jsonData[entry->size] = '\0';

            DynamicJsonDocument doc(24576);
            DeserializationError perr = deserializeJson(doc, jsonData);
            if (perr) {
                delete[] jsonData;
                Logger::instance().log("[ContentManager] Timeline parse error: " + item->name);
                activeContentId = -1;
                return false;
            }

            // NOTE: deserializeJson(doc, char*) is zero-copy - string values point
            // into jsonData, so copy everything into playFrames before freeing it.
            playLoop = doc["loop"] | false;
            JsonArray frames = doc["frames"];
            for (JsonObject frame : frames) {
                PlaybackFrame pf;
                pf.sceneRef     = frame["scene"] | "";          // String ctor copies the bytes
                pf.matrix0Scene = frame["matrix0Scene"] | "";
                pf.matrix1Scene = frame["matrix1Scene"] | "";
                pf.durationMs   = frame["durationMs"] | 100;
                playFrames.push_back(pf);
            }
            delete[] jsonData;

            if (playFrames.empty()) {
                Logger::instance().log("[ContentManager] Timeline has no frames: " + item->name);
                activeContentId = -1;
                return false;
            }

            playFrameIdx = 0;
            drawPlayFrame(0);
            playFrameStart = millis();
            Logger::instance().log("[ContentManager] Timeline started (" + String(playFrames.size()) +
                                   " frames, loop=" + String(playLoop) + "): " + item->name);
            return true;
        }

        case CONTENT_SCROLL: {
            if (!scroll || !scroll->loadFromJSON(item->path)) {
                activeContentId = -1;
                return false;
            }
            scroll->begin();
            Logger::instance().log("[ContentManager] Scroll started: " + item->name);
            return true;
        }

        case CONTENT_COUNTDOWN: {
            if (!countdown || !countdown->loadFromJSON(item->path)) {
                activeContentId = -1;
                return false;
            }
            countdown->begin();
            Logger::instance().log("[ContentManager] Countdown started: " + item->name);
            return true;
        }

        case CONTENT_PROCEDURAL: {
            // Runs continuously (one non-blocking tick per update()) until replaced.
            proceduralName = item->name;
            return true;
        }

        case CONTENT_TEST: {
            disp->clear();
            for (int m = 0; m < 2; m++) {
                for (int x = 0; x < COLS; x++) {
                    for (int y = 0; y < ROWS; y++) {
                        disp->setPixel(m, x, y, CRGB::Red);
                    }
                }
            }
            disp->show();
            return true;
        }

        default:
            activeContentId = -1;
            return false;
    }
}

// V16.4.12 - Advance whatever is currently playing. Called every loop() tick.
void ContentManager::updatePlayback() {
    if (activeContentId < 0) return;

    switch (activeType) {
        case CONTENT_ANIMATION: {
            if (playFrames.empty()) { activeContentId = -1; return; }
            if (millis() - playFrameStart < playFrames[playFrameIdx].durationMs) return;

            size_t next = playFrameIdx + 1;
            if (next >= playFrames.size()) {
                if (!playLoop) {
                    // One pass done - hold the last frame, stop advancing.
                    Logger::instance().log("[ContentManager] Timeline finished");
                    activeContentId = -1;
                    return;
                }
                next = 0;
            }
            playFrameIdx = next;
            drawPlayFrame(playFrameIdx);
            playFrameStart = millis();
            break;
        }

        case CONTENT_SCROLL:
            if (scroll) scroll->update();
            break;

        case CONTENT_COUNTDOWN:
            if (countdown) countdown->update();
            break;

        case CONTENT_PROCEDURAL:
            runProceduralTick(proceduralName);
            break;

        default:
            break;  // CONTENT_SCENE / CONTENT_TEST are static
    }
}

void ContentManager::drawPlayFrame(size_t idx) {
    if (idx >= playFrames.size()) return;
    const PlaybackFrame& f = playFrames[idx];
    disp->clear();
    if (f.sceneRef.length() > 0) {
        drawSceneFile(0, f.sceneRef, activePath);
        drawSceneFile(1, f.sceneRef, activePath);
    } else {
        drawSceneFile(0, f.matrix0Scene, activePath);
        drawSceneFile(1, f.matrix1Scene, activePath);
    }
    disp->show();
}

void ContentManager::runProceduralTick(const String& name) {
    if (name == "Chase")                 Animations::chase(disp);
    else if (name == "Snowfall")         Animations::snowfall(disp);
    else if (name == "Snowfall Gentle")  Animations::snowfallGentle(disp);
    else if (name == "Snowfall Heavy")   Animations::snowfallHeavy(disp);
    else if (name == "Sparkling Stars")  Animations::sparklingStars(disp);
    else if (name == "Color Wave")       Animations::colorWave(disp);
}

void ContentManager::stopPlayback() {
    activeContentId = -1;
    activeType = CONTENT_TEST;
    proceduralName = "";
    playFrames.clear();
    if (disp) {
        disp->clear();
        disp->show();
    }
}

void ContentManager::addContent(const String& name, const String& theme, ContentType type, const String& path, unsigned long duration, const String& m0, const String& m1, const String& m2) {
    ContentItem item;
    item.id = nextContentId++;
    item.name = name;
    item.theme = theme;
    item.type = type;
    item.path = path;
    item.durationMs = duration;          // V16.3.0-2026-01-10T22:43:00Z
    item.matrix0Scene = m0;              // V16.3.0-2026-01-10T22:43:00Z
    item.matrix1Scene = m1;              // V16.3.0-2026-01-10T22:43:00Z
    item.matrix2Scene = m2;              // V16.3.0-2026-01-10T22:43:00Z
    contentRegistry.push_back(item);
}

// V16.3.0-2026-01-10T22:43:00Z - Overload for backward compatibility (default 5s, mirror)
void ContentManager::addContent(const String& name, const String& theme, ContentType type, const String& path) {
    addContent(name, theme, type, path, 5000, path, path, "");
}

void ContentManager::registerProceduralAnimations() {
    Logger::instance().log("[ContentManager] Registering procedural animations...");
    
    // V16.2.0-2026-01-10T18:12:00Z - Register all procedural animations with correct themes
    addContent("Chase", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Snowfall", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Snowfall Gentle", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Snowfall Heavy", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Sparkling Stars", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Color Wave", "osu", CONTENT_PROCEDURAL, "");
}

// ---------------------------------------------------------------------------
// V16.4.13 - Run mode / daily schedule window
// ---------------------------------------------------------------------------

void ContentManager::enableScheduler(bool enable) {
    setRunMode(enable ? RUN_MODE_SCHEDULE : RUN_MODE_MANUAL);
}

bool ContentManager::isSchedulerEnabled() const {
    return runMode == RUN_MODE_SCHEDULE;
}

void ContentManager::setRunMode(uint8_t mode) {
    runMode = (mode == RUN_MODE_SCHEDULE) ? RUN_MODE_SCHEDULE : RUN_MODE_MANUAL;
    scheduleBlanked = false;
    saveScheduleToNVS();
    Logger::instance().log("[ContentManager] Run mode: " +
                           String(runMode == RUN_MODE_SCHEDULE ? "SCHEDULE" : "MANUAL"));
}

uint8_t ContentManager::getRunMode() const {
    return runMode;
}

void ContentManager::setScheduleWindow(uint8_t sh, uint8_t sm, uint8_t eh, uint8_t em) {
    schStartH = sh % 24; schStartM = sm % 60;
    schEndH   = eh % 24; schEndM   = em % 60;
    scheduleBlanked = false;
    saveScheduleToNVS();
    Logger::instance().log("[Schedule] Window " +
        String(schStartH) + ":" + String(schStartM) + " - " +
        String(schEndH) + ":" + String(schEndM));
}

void ContentManager::getScheduleWindow(uint8_t& sh, uint8_t& sm, uint8_t& eh, uint8_t& em) const {
    sh = schStartH; sm = schStartM; eh = schEndH; em = schEndM;
}

bool ContentManager::computeInWindow() {
    struct tm ti;
    if (!nowLocal(ti)) {
        if (!loggedTimeUnsynced) {
            Logger::instance().log("[Schedule] time not synced - staying ON");
            loggedTimeUnsynced = true;
        }
        return true;
    }
    loggedTimeUnsynced = false;

    int now = ti.tm_hour * 60 + ti.tm_min;
    int s   = schStartH * 60 + schStartM;
    int e   = schEndH   * 60 + schEndM;
    if (s == e) return true;                              // degenerate = always on
    if (s < e)  return (now >= s && now < e);
    return (now >= s || now < e);                         // window wraps past midnight
}

void ContentManager::loadScheduleFromNVS() {
    Preferences p;
    p.begin(PREFS_NAMESPACE, true);
    runMode   = p.getUChar(RUN_MODE_KEY, RUN_MODE_MANUAL);
    schStartH = p.getUChar("sch_sh", 17);
    schStartM = p.getUChar("sch_sm", 0);
    schEndH   = p.getUChar("sch_eh", 22);
    schEndM   = p.getUChar("sch_em", 0);
    p.end();
    if (runMode != RUN_MODE_SCHEDULE) runMode = RUN_MODE_MANUAL;
    Logger::instance().log("[Schedule] Loaded: mode=" +
        String(runMode == RUN_MODE_SCHEDULE ? "SCHEDULE" : "MANUAL") + " " +
        String(schStartH) + ":" + String(schStartM) + "-" +
        String(schEndH) + ":" + String(schEndM));
}

void ContentManager::saveScheduleToNVS() {
    Preferences p;
    p.begin(PREFS_NAMESPACE, false);
    p.putUChar(RUN_MODE_KEY, runMode);
    p.putUChar("sch_sh", schStartH);
    p.putUChar("sch_sm", schStartM);
    p.putUChar("sch_eh", schEndH);
    p.putUChar("sch_em", schEndM);
    p.end();
}

// ---------------------------------------------------------------------------
// V16.4.13 - Eligible-content mask for random mode
// ---------------------------------------------------------------------------

String ContentManager::contentKey(const ContentItem& item) {
    return String((int)item.type) + "|" + item.theme + "|" + item.name;
}

bool ContentManager::isEligible(const ContentItem& item) const {
    if (item.type == CONTENT_TEST) return false;
    if (item.theme == "test") return false;
    String key = contentKey(item);
    for (const auto& k : excludedKeys) {
        if (k == key) return false;
    }
    return true;
}

std::vector<String> ContentManager::getExcludedKeys() const {
    return excludedKeys;
}

void ContentManager::setExcludedByIds(const String& csvIds) {
    excludedKeys.clear();
    int start = 0;
    while (start < (int)csvIds.length()) {
        int comma = csvIds.indexOf(',', start);
        if (comma < 0) comma = csvIds.length();
        String tok = csvIds.substring(start, comma);
        tok.trim();
        if (tok.length() > 0) {
            const ContentItem* it = getContentById((uint16_t)tok.toInt());
            if (it) excludedKeys.push_back(contentKey(*it));
        }
        start = comma + 1;
    }
    saveEligibleToNVS();
    Logger::instance().log("[ContentManager] Excluded " + String(excludedKeys.size()) + " items from random");
}

void ContentManager::loadEligibleFromNVS() {
    Preferences p;
    p.begin(PREFS_NAMESPACE, true);
    String blob = p.getString("rand_excl", "");
    p.end();
    excludedKeys.clear();
    int start = 0;
    while (start < (int)blob.length()) {
        int nl = blob.indexOf('\n', start);
        if (nl < 0) nl = blob.length();
        String line = blob.substring(start, nl);
        if (line.length() > 0) excludedKeys.push_back(line);
        start = nl + 1;
    }
    if (excludedKeys.size() > 0) {
        Logger::instance().log("[ContentManager] Loaded " + String(excludedKeys.size()) + " random exclusions");
    }
}

void ContentManager::saveEligibleToNVS() {
    String blob;
    for (const auto& k : excludedKeys) { blob += k; blob += '\n'; }
    Preferences p;
    p.begin(PREFS_NAMESPACE, false);
    p.putString("rand_excl", blob);
    p.end();
}

// ---------------------------------------------------------------------------
// V16.4.13 - Playback status for /api/status
// ---------------------------------------------------------------------------

ContentManager::PlaybackStatus ContentManager::getStatus() const {
    PlaybackStatus st;
    st.scheduleActive = scheduleActive;
    st.id = activeContentId;
    if (activeContentId < 0) {
        st.name = "idle";
        st.type = "idle";
        st.theme = "";
        return st;
    }
    const ContentItem* it = getContentById((uint16_t)activeContentId);
    if (it) {
        st.name = it->name;
        st.theme = it->theme;
        switch (it->type) {
            case CONTENT_SCENE:      st.type = "scene"; break;
            case CONTENT_ANIMATION:  st.type = "animation"; break;
            case CONTENT_SCROLL:     st.type = "scroll"; break;
            case CONTENT_COUNTDOWN:  st.type = "countdown"; break;
            case CONTENT_PROCEDURAL: st.type = "procedural"; break;
            case CONTENT_TEST:       st.type = "test"; break;
        }
    } else {
        st.name = "?";
        st.type = "?";
    }
    return st;
}

void ContentManager::enableRandomMode(bool enable) {
    randomModeEnabled = enable;
    if (enable) {
        lastRandomChange = millis();
        Logger::instance().log("[ContentManager] Random mode ENABLED");
    } else {
        Logger::instance().log("[ContentManager] Random mode DISABLED");
    }
}

bool ContentManager::isRandomModeEnabled() const {
    return randomModeEnabled;
}

void ContentManager::setRandomInterval(unsigned long intervalMs) {
    randomIntervalMs = intervalMs;
    Logger::instance().log("[ContentManager] Random interval: " + String(intervalMs) + "ms");
}

unsigned long ContentManager::getRandomInterval() const {
    return randomIntervalMs;
}

void ContentManager::setRandomThemeFilter(const String& theme) {
    randomThemeFilter = theme;
    if (theme.length() > 0) {
        Logger::instance().log("[ContentManager] Random filter: " + theme);
    } else {
        Logger::instance().log("[ContentManager] Random filter: ALL THEMES");
    }
}

String ContentManager::getRandomThemeFilter() const {
    return randomThemeFilter;
}

void ContentManager::updateRandomMode() {
    unsigned long now = millis();
    if (now - lastRandomChange >= randomIntervalMs) {
        selectRandomContent();
        lastRandomChange = now;
    }
}

void ContentManager::selectRandomContent() {
    // V16.4.14 - Pick a theme first (never mix themes across matrices), then
    // pick a content TYPE from whatever's actually eligible within that theme
    // (scene / animation / scroll / countdown / procedural), then resolve it:
    // scenes randomize each matrix independently (duplicates allowed); every
    // other type is a single self-contained pick played as a whole, same as
    // it always has been.
    std::vector<String> themes;
    if (randomThemeFilter.length() > 0) {
        themes.push_back(randomThemeFilter);
    } else {
        for (const auto& item : contentRegistry) {
            if (!isEligible(item)) continue;
            bool seen = false;
            for (const auto& t : themes) { if (t == item.theme) { seen = true; break; } }
            if (!seen) themes.push_back(item.theme);
        }
    }
    if (themes.empty()) return;
    String chosenTheme = themes[random(themes.size())];

    std::vector<ContentItem> themePool;
    for (const auto& item : contentRegistry) {
        if (!isEligible(item)) continue;
        if (item.theme == chosenTheme) themePool.push_back(item);
    }
    if (themePool.empty()) return;

    // Bucket the theme's eligible content by type, keeping only non-empty buckets.
    static const ContentType kTypes[] = {
        CONTENT_SCENE, CONTENT_ANIMATION, CONTENT_SCROLL, CONTENT_COUNTDOWN, CONTENT_PROCEDURAL
    };
    std::vector<ContentType> availableTypes;
    for (ContentType t : kTypes) {
        for (const auto& item : themePool) {
            if (item.type == t) { availableTypes.push_back(t); break; }
        }
    }
    if (availableTypes.empty()) return;
    ContentType chosenType = availableTypes[random(availableTypes.size())];

    if (chosenType == CONTENT_SCENE) {
        std::vector<ContentItem> scenePool;
        for (const auto& item : themePool) {
            if (item.type == CONTENT_SCENE) scenePool.push_back(item);
        }

        int idx0 = random(scenePool.size());
        int idx1 = random(scenePool.size());  // independent draw - duplicates allowed
        const ContentItem& s0 = scenePool[idx0];
        const ContentItem& s1 = scenePool[idx1];

        themeManager.setTheme(ThemeManager::themeNameToId(chosenTheme));
        playFrames.clear();
        proceduralName = "";
        activeType = CONTENT_SCENE;
        activePath = s0.path;
        activeContentId = s0.id;

        disp->clear();
        drawSceneFile(0, s0.path, s0.path);
        drawSceneFile(1, s1.path, s1.path);
        disp->show();

        Logger::instance().log("[ContentManager] Random (" + chosenTheme + " scene): " + s0.name + " | " + s1.name);
        return;
    }

    // Animation / scroll / countdown / procedural - single pick, plays as a
    // whole (its own timeline/logic already governs both matrices).
    std::vector<ContentItem> typePool;
    for (const auto& item : themePool) {
        if (item.type == chosenType) typePool.push_back(item);
    }
    int idx = random(typePool.size());
    renderContent(typePool[idx].id);
    Logger::instance().log("[ContentManager] Random (" + chosenTheme + "): " + typePool[idx].name);
}