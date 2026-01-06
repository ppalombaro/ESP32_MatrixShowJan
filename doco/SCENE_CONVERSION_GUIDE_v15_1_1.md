# V15.1.1 Scene Conversion Guide

## What Changed

Scenes can now be stored as JSON files in SPIFFS and auto-discovered. The system falls back to hardcoded scenes if JSON files don't exist.

## File Structure

```
data/
├── scenes/
│   ├── christmas_tree.json      ✓ Converted
│   ├── santa_claus.json          ✓ Converted
│   ├── gift.json                 → Convert with script
│   ├── yellow_gift.json          → Convert with script
│   ├── frosty_snowman.json       → Convert with script
│   ├── giant_snowflake.json      → Convert with script
│   ├── halloween_pumpkin.json    → Convert with script
│   └── [other scenes...]
```

## How to Use

### Option 1: Upload Existing JSON Files (Immediate)

1. Create `data/scenes/` folder in Arduino sketch directory
2. Copy `christmas_tree.json` and `santa_claus.json` to this folder
3. Upload via Tools → ESP32 Sketch Data Upload
4. These two scenes now load from SPIFFS, others use hardcoded fallback

### Option 2: Convert All Scenes (Complete Migration)

Run the converter script:
```bash
python3 scene_converter.py /path/to/Scenes.cpp
```

This generates JSON files for:
- gift.json
- yellow_gift.json  
- frosty_snowman.json
- giant_snowflake.json
- halloween_pumpkin.json
- halloween_bat.json
- halloween_ghost.json
- thanksgiving_turkey.json
- newyear_firework.json

Upload all to `data/scenes/` folder.

## Adding New Scenes

### Method 1: Create JSON Manually
```json
{
  "sceneId": "my_scene",
  "version": "15.1.1",
  "created": "2026-01-04",
  "description": "My custom scene",
  "theme": "christmas",
  "centerX": 10,
  "centerY": 12,
  "pixels": [
    {"x": 0, "y": 0, "r": 255, "g": 0, "b": 0},
    {"x": 1, "y": 0, "r": 0, "g": 255, "b": 0}
  ]
}
```

### Method 2: Use LED Content Studio
The HTML tool can export scenes in this exact JSON format.

## Code Integration

### ThemeManager Changes Needed

Replace scene rendering code:
```cpp
// OLD:
staticScenes->drawChristmasTree(matrix);

// NEW:
SceneData sceneData(disp);
if (!sceneData.renderSceneFromFile("christmas_tree.json", matrix)) {
    // Fallback to hardcoded
    sceneData.renderHardcodedScene("christmas_tree", matrix);
}
```

### ContentManager Integration

ContentManager already scans `/scenes/` directory and builds the scene list automatically. No changes needed there.

## Benefits

✓ Drop new scene JSON → reboot → appears in UI  
✓ Edit scenes without recompiling firmware  
✓ Share scene packs as ZIP files  
✓ Gradual migration (fallback to hardcoded scenes)  
✓ Same JSON format as animations (consistent)  

## Migration Path

**Phase 1** (Now): Use SceneData with 2 JSON scenes  
**Phase 2**: Convert remaining scenes with Python script  
**Phase 3**: Remove hardcoded scene functions once all converted  
**Phase 4**: Create new scenes with LED Content Studio
