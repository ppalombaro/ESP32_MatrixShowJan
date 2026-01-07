# ESP32 MatrixShow 15.4FIX

**Version:** 15.4FIX  
**ESP32 Core:** 2.0.17

## Overview
ESP32 MatrixShow 15.4FIX is a fully optimized system for managing, previewing, and scheduling LED matrix animations and scenes, supporting both FFAT JSON files and code-based animations.

### Key Features
- Dynamic scene discovery for FFAT and code-based scenes
- Theme-aware filtering via ThemeManager
- Preview using global numeric IDs (UAT ready)
- Brightness slider with sticky default 20%
- Schedule visible and sticky; respects both hour + minute
- Memory-leak-free across HolidayAnims, WebController, SceneData, Logger
- Timeline animations supported
- Fail-safe handling for missing FFAT files

## Folder Structure
```
ESP32_MatrixShow_15.4FIX/
├─ src/
│  ├─ ContentManager.h
│  ├─ ContentManager.cpp
│  ├─ ThemeManager.h
│  ├─ ThemeManager.cpp
│  ├─ HolidayAnims.h
│  ├─ SceneData.h
│  ├─ SceneData.cpp
│  ├─ Logger.h
│  ├─ Logger.cpp
│  ├─ WebController.h
│  ├─ WebController.cpp
│  ├─ WebPages.h
│  ├─ WebPages.cpp
│  ├─ Scheduler.h
│  ├─ Scheduler.cpp
├─ data/
│  ├─ animations/   # FFAT JSON animation files
│  ├─ scenes/       # FFAT JSON scene files
└─ README.md
```

## Setup Instructions
1. Flash your ESP32 with core 2.0.17.
2. Copy the `data` folder into FFAT (or SPIFFS) using your Arduino/ESP32 tool.
3. Build and upload all `src` files.
4. Initialize the system in `setup()` as follows:
```cpp
void setup() {
    Serial.begin(115200);
    FFat.begin();

    ThemeManager::instance().begin();
    ContentManager::instance().begin();

    HolidayAnims::registerScenes();       // register code-based scenes
    ContentManager::instance().discoverFFATScenes("/data"); // discover FFAT scenes

    WebPages::instance().begin();
    WebController::instance().begin(80);

    Scheduler::instance().begin();
}
```
5. Main loop:
```cpp
void loop() {
    WebController::instance().handleClient();
    Scheduler::instance().update();
    delay(10);
}
```

## UI / Web Interface
- **Preview Scene:** Dropdown uses numeric IDs for UAT testing. Select and run any scene.
- **Brightness:** Slider starts at 20% and is sticky across sessions.
- **Schedule:** Hour and minute always visible, sticky; triggers scenes once per minute.

## System Diagram
```
        ┌────────────────────┐
        │      WebController │
        │  (HTTP Routes)     │
        └─────────┬─────────┘
                  │
      ┌───────────┴────────────┐
      │       WebPages          │
      │  (UI / Preview / Slider)│
      └───────────┬────────────┘
                  │
       ┌──────────┴──────────┐
       │   ContentManager     │
       │  (Registry / Loader) │
       └───────────┬─────────┘
                   │
      ┌────────────┴────────────┐
      │ ThemeManager (filter)   │
      │ FFAT & Code Scene Themes│
      └───────────┬────────────┘
                  │
   ┌──────────────┴──────────────┐
   │ SceneData / AnimationLoader │
   │ (Timeline frames / play)   │
   └───────────┬────────────────┘
               │
      ┌────────┴───────────┐
      │  MatrixDisplay      │
      │ (Hardware Renderer) │
      └────────────────────┘
```

## Notes
- **ContentManager** is the single source of truth for all scenes.
- **ThemeManager** provides filtered lists without owning scenes.
- **SceneData** safely loads FFAT JSON timelines.
- **Logger** handles all console output safely with dynamic buffer.
- **Scheduler** triggers FFAT and code scenes at the scheduled hour and minute.
- **HolidayAnims** registers code-based animations only; no memory ownership.

## Versioning
- **15.4FIX:** Complete memory-safe, dynamic discovery, preview, brightness, schedule, and timeline fixes.

