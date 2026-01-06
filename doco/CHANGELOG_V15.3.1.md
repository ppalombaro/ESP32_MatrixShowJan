# CHANGELOG V15.3.1

## Complete Animation Migration to SPIFFS
### Version 15.3.1 - 2026-01-04T17:00:00Z

### SUMMARY
V15.3.1 completes the content migration by converting ALL frame-based animations to JSON format, following the proven spooky_eyes model.

### CHANGES
- **Bleeding Windows**: 16 frames + timeline (17 files)
- **Falling Leaves**: 20 frames + timeline (21 files)
- **Fireworks**: 30 frames + timeline (31 files)
- **Color Spinning Star**: 24 frames + timeline (25 files)

### CODE REDUCTION
- HolidayAnimations.cpp: ~800 lines removed
- Combined with V15.3.0: ~1,420 total lines eliminated

### NEW FILES
- 94 animation JSON files
- 4 animation timeline files
- Total: 121 JSON files in data/ folder

See README_V15.3.1.md for complete details.
