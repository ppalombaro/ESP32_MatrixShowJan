# SPIFFS Data Folder Structure Guide

## Directory Layout

```
data/
├── schedule.json              # Scheduler configuration (required)
├── scenes/                    # Static scenes directory
│   ├── christmas/             # Christmas theme scenes
│   │   ├── christmas_tree.json
│   │   ├── santa_claus.json
│   │   ├── gift.json
│   │   ├── yellow_gift.json
│   │   ├── frosty_snowman.json
│   │   └── giant_snowflake.json
│   ├── halloween/             # Halloween theme scenes
│   │   ├── halloween_pumpkin.json
│   │   ├── halloween_bat.json
│   │   ├── halloween_ghost.json
│   │   ├── jason_mask.json
│   │   ├── halloween_skull.json
│   │   └── pirate_flag.json
│   ├── thanksgiving/          # Thanksgiving theme scenes
│   │   └── thanksgiving_turkey.json
│   ├── newyear/               # New Year theme scenes
│   │   └── newyear_firework.json
│   └── osu/                   # Ohio State theme scenes
│       └── osu_logo.json
└── animations/                # Animated content directory
    ├── christmas/             # Christmas animations
    │   ├── snowfall.json
    │   ├── chase.json
    │   ├── scroll.json
    │   └── spinning_snowflake.json
    ├── halloween/             # Halloween animations
    │   ├── flying_bats.json
    │   ├── pirate_ship.json
    │   └── spooky_eyes.json
    ├── thanksgiving/          # Thanksgiving animations
    │   └── falling_leaves.json
    ├── newyear/               # New Year animations
    │   ├── fireworks.json
    │   └── countdown.json
    └── osu/                   # Ohio State animations
        ├── color_wave.json
        └── ohio_letters.json
```

## How It Works

### Theme Auto-Detection

**Priority 1: Directory Path**
- File in `/scenes/christmas/` → automatically tagged as "christmas" theme
- File in `/scenes/halloween/` → automatically tagged as "halloween" theme
- No manual theme tagging needed!

**Priority 2: Filename (fallback)**
- If not in theme folder, detects from filename:
  - Contains "christmas" or "xmas" → christmas
  - Contains "halloween" → halloween
  - Contains "thanksgiving" → thanksgiving
  - Contains "newyear" or "nye" → newyear
  - Contains "osu" or "ohio" → osu

**Priority 3: Backward Compatibility**
- Files in flat `/scenes/` directory still work
- Uses filename-based detection

### ContentManager Scanning

ContentManager scans in this order:
1. `/scenes/christmas/` → `/scenes/halloween/` → etc.
2. `/animations/christmas/` → `/animations/halloween/` → etc.
3. `/scenes/` (root, for backwards compatibility)
4. `/animations/` (root, for backwards compatibility)
5. `/tests/` (test patterns, no theme organization)

## Benefits of Theme Folders

✅ **Organization** - All Christmas content in one place  
✅ **Easy Backup** - ZIP entire theme folder  
✅ **Theme Packs** - Share holiday content as folder  
✅ **Auto-Tagging** - No manual theme configuration  
✅ **Visual Clarity** - See what belongs to which holiday  
✅ **Web UI Filtering** - Filter scenes by theme  

## Adding New Content

### Option 1: Use Theme Folder (Recommended)
```bash
# 1. Create your scene JSON
# 2. Save to appropriate theme folder:
data/scenes/christmas/my_new_tree.json

# 3. Re-upload SPIFFS
# 4. Scene appears with christmas theme automatically!
```

### Option 2: Flat Directory (Backward Compatible)
```bash
# 1. Create scene with theme in filename
data/scenes/christmas_my_scene.json

# 2. System detects "christmas" in filename
# 3. Auto-tags as christmas theme
```

## Upload Instructions

### Arduino IDE Method
1. Copy entire `data/` folder to your sketch directory
2. Tools → ESP32 Sketch Data Upload
3. Wait for completion (30-60 seconds)

### PlatformIO Method
```bash
pio run --target uploadfs
```

### Manual Method (Advanced)
Use ESP32 filesystem uploader tools to manually place files

## File Size Limits

- **Per File**: 5MB max (SPIFFS limit)
- **Total SPIFFS**: Depends on partition (typically 1.5MB)
- **Recommended**: Keep scenes under 50KB each
- **Large Scenes**: Use compression or reduce detail

## Verification

After upload, check Serial Monitor:
```
Content discovered: 6 scenes, 4 animations, 3 tests
Found scene: Christmas Tree (/scenes/christmas/christmas_tree.json)
Found scene: Santa Claus (/scenes/christmas/santa_claus.json)
...
```

## Common Issues

**Scenes not appearing:**
- Verify data/ folder is in sketch directory
- Check SPIFFS upload completed successfully
- Ensure JSON files are valid (use validator)

**Wrong theme detected:**
- Move file to correct theme subfolder
- Or rename file to include theme keyword

**Upload fails:**
- Check partition size (3MB+ recommended)
- Remove unused files to free space
- Verify file paths don't exceed 31 chars

## Example File Paths

✅ **Correct:**
```
/scenes/christmas/tree.json
/scenes/halloween/pumpkin.json
/animations/christmas/snowfall.json
```

❌ **Avoid:**
```
/scenes/christmas-scenes-folder/xmas_tree_v2_final.json  # Too long!
/scenes/tree.json  # No theme context
```

## Migration from Flat Structure

Old structure still works! To migrate:

```bash
# Old (still works):
data/scenes/christmas_tree.json

# New (recommended):
data/scenes/christmas/christmas_tree.json
```

Both are discovered, theme folder takes priority.
