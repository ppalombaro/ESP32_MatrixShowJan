# Theme Content Placeholders

This directory is organized by holiday theme. Add your JSON scene files here.

## Supported Themes:
- **christmas** - Christmas holiday content
- **halloween** - Halloween/spooky content  
- **thanksgiving** - Thanksgiving content
- **newyear** - New Year's celebration content
- **osu** - Ohio State University themed content

## How to Add Content:

1. Create or convert scene to JSON format
2. Save in appropriate theme folder
3. Upload SPIFFS data to ESP32
4. Scene appears automatically in web UI

## File Naming:

Files can be named anything, but descriptive names help:
- ✅ `tree.json` (in christmas folder)
- ✅ `santa_face.json`
- ✅ `snowman.json`
- ❌ `scene1.json` (not descriptive)

## Creating New Themes:

To add a new theme:
1. Create new folder: `data/scenes/yourtheme/`
2. Update `ContentManager.cpp` themes array
3. Add content to new folder

---

See `DATA_FOLDER_STRUCTURE.md` for complete documentation.
