/* ContentManager.h
   Auto-discovery system for scenes, animations, and schedules
   VERSION: V15.1.0-2026-01-04T10:00:00Z - Initial content discovery system
   
   V15.1.0-2026-01-04T10:00:00Z - Created auto-discovery architecture
*/

#ifndef CONTENT_MANAGER_H
#define CONTENT_MANAGER_H

#include <Arduino.h>
#include <FFat.h>
#include <ArduinoJson.h>
#include <vector>

// V15.1.0-2026-01-04T10:00:00Z - Content item descriptor
struct ContentItem {
    String filename;      // e.g. "tree.json"
    String displayName;   // e.g. "Christmas Tree"
    String category;      // "scene", "animation", "test"
    String theme;         // "christmas", "halloween", etc.
    int index;            // Auto-assigned position in list
};

// V15.1.0-2026-01-04T10:00:00Z - Schedule configuration
struct ScheduleConfig {
    uint8_t startHour;
    uint8_t startMinute;
    uint8_t endHour;
    uint8_t endMinute;
    String mode;          // "random", "sequential", "single"
    bool enabled;
    std::vector<String> allowedContent;  // Filenames of enabled content
};

class ContentManager {
public:
    ContentManager();
    
    bool begin();  // V15.1.0-2026-01-04T10:00:00Z - Initialize FFat and scan content
    void refresh(); // V15.1.0-2026-01-04T10:00:00Z - Rescan content directory
    
    // V15.1.0-2026-01-04T10:00:00Z - Content queries
    std::vector<ContentItem> getScenes(String theme = "");
    std::vector<ContentItem> getAnimations(String theme = "");
    std::vector<ContentItem> getTests();
    ContentItem* getContentByIndex(String category, int index);
    ContentItem* getContentByFilename(String filename);
    
    // V15.1.0-2026-01-04T10:00:00Z - Schedule management
    bool loadSchedule(ScheduleConfig& config);
    bool saveSchedule(const ScheduleConfig& config);
    
    // V15.1.0-2026-01-04T10:00:00Z - Random content selection
    std::vector<ContentItem> getEnabledContent();
    
    int getTotalScenes() { return scenes.size(); }
    int getTotalAnimations() { return animations.size(); }
    int getTotalTests() { return tests.size(); }
    
private:
    std::vector<ContentItem> scenes;      // V15.1.0-2026-01-04T10:00:00Z
    std::vector<ContentItem> animations;  // V15.1.0-2026-01-04T10:00:00Z
    std::vector<ContentItem> tests;       // V15.1.0-2026-01-04T10:00:00Z
    
    void scanDirectory(const char* path, String category);  // V15.1.0-2026-01-04T10:00:00Z
    String extractDisplayName(const char* filename);        // V15.1.0-2026-01-04T10:00:00Z
    String extractTheme(const char* filename);              // V15.1.0-2026-01-04T10:00:00Z
};

#endif
