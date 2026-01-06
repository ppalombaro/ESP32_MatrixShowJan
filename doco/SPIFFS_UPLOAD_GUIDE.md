# V15.1.0 SPIFFS Directory Structure

Upload to ESP32 SPIFFS using Arduino IDE "ESP32 Sketch Data Upload" tool.

## Required Directory Layout

```
data/
├── schedule.json              # Scheduler configuration
├── scenes/
│   ├── christmas_tree.json
│   ├── santa.json
│   └── [other scene files]
├── animations/
│   ├── snowfall.json
│   ├── chase.json
│   └── [other animation files]
└── tests/
    └── [test pattern files]
```

## How to Upload

1. Create `data/` folder in your Arduino sketch directory
2. Copy JSON files into appropriate subdirectories
3. Install "ESP32 Sketch Data Upload" plugin (if not installed)
4. Select Tools → ESP32 Sketch Data Upload
5. Wait for upload to complete

## Initial Upload Files (Minimum)

Required for V15.1.0 to work:
- `schedule.json` (from this directory)
- At least 1 scene JSON in `/scenes/`
- At least 1 animation JSON in `/animations/`

## File Naming Convention

Files are auto-discovered and display names are generated from filenames:
- `christmas_tree.json` → "Christmas Tree"
- `snowfall.json` → "Snowfall"
- `flying_bats.json` → "Flying Bats"

## Theme Auto-Detection

Themes are detected from filename keywords:
- Contains "christmas" or "xmas" → Christmas theme
- Contains "halloween" → Halloween theme  
- Contains "thanksgiving" → Thanksgiving theme
- Contains "newyear" or "nye" → New Year theme
- Contains "osu" or "ohio" → OSU theme
- Otherwise → General theme

## Adding New Content

**Drop-in workflow:**
1. Create new JSON file in appropriate directory
2. Upload via ESP32 Sketch Data Upload (or copy to SPIFFS directly)
3. Reboot ESP32
4. New content appears automatically in web UI

**No code changes required!**
