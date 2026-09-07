# Content Storage Guide — V16.4.13

> **Naming note:** kept as `SPIFFS_UPLOAD_GUIDE` for continuity. V16.4 uses
> **neither SPIFFS nor FFat as a filesystem.** Content is a flat, custom,
> length-prefixed **blob** flashed raw to the `ffat` partition and parsed by
> `ContentManager::readCustomStorage()` with `esp_flash_read()`.

Firmware and content are flashed **separately**. Runtime settings (brightness,
schedule window, run mode, random exclusions) live in **NVS**, not in any content
file.

---

## Pipeline

```
data_in/            hand-authored / Studio-exported JSON (pretty-printed, tracked in git)
   │  minify_json.py
   ▼
data/               staging: minified copies (temporary; deleted after packing)
   │  generate_manifest.py   -> ffat_manifest.txt   (path,length,sha256)
   │  build_simple_storage.py
   ▼
ffat.bin            917,504-byte image (fixed = ffat partition size)
   │  esptool.py write_flash 0x290000
   ▼
ffat partition @ 0x290000
   │  esp_flash_read()
   ▼
ContentManager.contentRegistry     (in RAM, built at boot)
```

Windows one-shot: **`json_datain_2_FFAT.bat`** (`--dry-run` stops after the
manifest). Manual steps: [InstallationGuide.md §4](InstallationGuide.md).

---

## Blob format (`tools/FFAT/build_simple_storage.py`)

```
[4 bytes LE]  file count
per file:
  [2 bytes LE]  path length  (UTF-8 bytes, 1..255)
  [K bytes]     path          (forward slashes, relative to data_in/)
  [4 bytes LE]  content length
  [M bytes]     content       (minified JSON)
  [0-pad]       to the next 512-byte boundary
... zero-filled to FFAT_SIZE (917504)
```

`readCustomStorage()` rejects a file count of 0 or > 500, and a path length of 0
or > 255. Cursor advance: `addr += contentLen; addr += (512 - addr % 512) % 512;`.

---

## Flash partition — [`partitions.csv`](../partitions.csv)

```
ffat   data   fat   0x290000   0xE0000     (917,504 bytes)
```

- Offset **`0x290000`** matches `#define DATA_PARTITION_OFFSET 0x290000` in
  `src/ContentManager.cpp` and `write_flash 0x290000` in the `.bat`. Keep all
  three in sync.
- `app0` (0x180000) and `app1` (0x100000) are **unequal** — OTA needs equal
  slots, so OTA is not usable without repartitioning first.
- The `fat` subtype label is cosmetic; nothing mounts it.

---

## `data_in/` layout

```
data_in/
├── schedule.json          (legacy - NOT read by V16.4; see below)
├── scenes/<theme>/<name>.json                  -> CONTENT_SCENE
├── animations/<theme>/<name>/
│   ├── animation_timeline.json                 -> CONTENT_ANIMATION
│   └── <frame files>.json                      (Scene schema)
├── scroll/<theme>/<name>.json                  -> CONTENT_SCROLL
├── countdown/<theme>/<name>.json               -> CONTENT_COUNTDOWN
└── test/<name>.json  (or scenes/test/, animations/test/)   -> CONTENT_TEST
```

**Theme** = the second path segment. **Display name** = the filename without
`.json` (or the animation folder name for timelines). Registration rules:

| Path pattern | Type |
|---|---|
| `scenes/**/*.json` | `CONTENT_SCENE` |
| `**/*_timeline.json`, `**/animation_timeline.json` | `CONTENT_ANIMATION` |
| `scroll/**/*.json` | `CONTENT_SCROLL` |
| `countdown/**/*.json` | `CONTENT_COUNTDOWN` |
| `test/**/*.json` | `CONTENT_TEST` |

Procedural animations (Chase, Snowfall ×3, Sparkling Stars, Color Wave) are
**code-registered**, not files.

**`data_in/schedule.json` is not read in V16.4.** The daily on/off window, run
mode and the random-mode eligible-content mask are configured on the `/times` and
`/schedule` pages and persisted to NVS (`show-config`). The file is harmless
(it matches no registration pattern) but has no effect — leave it or delete it.

---

## JSON schemas

### Scene — `scenes/<theme>/<name>.json`
```json
{
  "name": "halloween_pumpkin", "theme": "halloween",
  "centerX": 10, "centerY": 12, "durationMs": 5000,
  "matrix0Scene": "scenes/halloween/halloween_pumpkin.json",
  "matrix1Scene": "scenes/halloween/halloween_pumpkin.json",
  "pixels": [
    { "x": 0, "y": -5, "r": 255, "g": 128, "b": 0 },
    { "x": -1, "y": -2, "r": 0, "g": 0, "b": 0 }
  ]
}
```
- `pixels[].x/y` are offsets from `centerX/centerY`. Windows are 20 wide × 25
  tall → usable roughly `x ∈ [-10, 9]`, `y ∈ [-12, 12]`. Out-of-range pixels are
  silently dropped.
- `durationMs` — how long the rotation holds this item (default 5000).
- `matrix0Scene` → **right** window, `matrix1Scene` → **left** window.
  `matrix1Scene` defaults to `matrix0Scene`; `matrix0Scene` defaults to the
  file's own path. Use these to show a *pair* as one content item.
- `drawSceneFile()` renders `pixels[]` with `CRGB(r,g,b)`.

### Animation — `animations/<theme>/<name>/`
`animation_timeline.json`:
```json
{
  "animationId": "spooky_eyes", "theme": "halloween",
  "loop": true, "durationMs": 0,
  "frames": [
    { "scene": "eyes_center", "durationMs": 500 },
    { "scene": "eyes_left",   "durationMs": 300 }
  ]
}
```
- `frames[].scene` resolves against the timeline folder (`resolveScenePath`);
  `.json` appended if missing. A value containing `/` is used as-is.
- Item duration = `durationMs` if non-zero, else the sum of `frames[].durationMs`
  (default 100 ms/frame), else 5000.
- Frame files use the Scene schema.
- **Non-blocking:** `renderContent` parses `frames[]` into RAM; `update()`
  advances one frame when `millis() - frameStart ≥ durationMs`.
- `loop: true` repeats the timeline; without it, playback stops on and holds the
  last frame.

**Per-frame, per-window frames (dual-panel content).** A frame may carry
`matrix0Scene` + `matrix1Scene` instead of `scene`:
```json
{ "matrix1Scene": "letter_O", "matrix0Scene": "letter_H", "durationMs": 1000 }
```
`matrix1` = **left** window (GPIO 17), `matrix0` = **right** window (GPIO 16).
Used by `animations/osu/ohio_chant`, `animations/halloween/lightning_flash`,
`animations/halloween/split_screen`. Component scenes live inside the animation
folder (bare name). An empty `""` or a `blank.json` (empty `pixels`) = dark
panel. `renderContent` reads `matrix0Scene`/`matrix1Scene`, falling back to
`scene` for both windows.

### Scroll — `scroll/<theme>/<name>.json`
```json
{ "text": "GO BUCKS", "speed": 50 }
```
- `text` uppercased on load (`FONT_5X7` is ASCII 32–90).
- `speed` = ms per 1-pixel step (default 50). Scrolls right-to-left across both
  20-wide windows; colour cycles through the theme palette each pass.
- Theme is set from the content item's folder (`scroll/<theme>/…`).

### Countdown — `countdown/<theme>/<name>.json`
```json
{ "targetDate": "2026-11-28 12:00:00", "comment": "OSU vs Michigan", "durationMs": 5000 }
```
- `targetDate` — `"YYYY-MM-DD HH:MM:SS"` interpreted as **local wall-clock**
  (DST-aware; `main.cpp` sets US Eastern via `configTzTime`), or a Unix epoch
  integer.
- Left window = Days / Hours, right window = Minutes / Seconds (`DIGIT_3X5`
  font). Header = theme colour 1, box = 2, digits = 3. Flashes `00` past target.
- Blank until the system clock syncs after boot.

### Test — `test/*.json`
Registered as `CONTENT_TEST`; the renderer ignores the file content and does a
solid red fill on both windows (held until replaced). Same as `/api/test`.

---

## Authoring workflow

1. Open `tools/LED_Content_Studio_V16_4_10.html` in a browser. Design at
   **20 wide × 25 tall**, export JSON.
2. Save into the right folder (`data_in/scenes/<theme>/…`, etc).
3. Re-pack + flash: `json_datain_2_FFAT.bat` (or the manual steps).
4. Reboot. `ContentManager` re-scans on boot — **no firmware rebuild**. Serial
   prints `Files in storage: <N>`; `http://<IP>/discovery` lists everything.
5. Commit new files under `data_in/` (the `data/` staging folder is transient,
   git-ignored).

### Helper scripts (`tools/FFAT/`)
| Script | Purpose |
|---|---|
| `minify_json.py` | strip whitespace, `data_in/` → `data/`; hard-fails if the total exceeds 917,504 B |
| `generate_manifest.py` | write `ffat_manifest.txt` (`path,length,sha256`) |
| `build_simple_storage.py` | pack `data/` → `ffat.bin` (arg 3 = max size) — **this is the packer the `.bat` uses** |
| `build_ffat_custom.py` | unused alternate FAT16 builder — not part of the pipeline |

---

## Size budget

- Partition 917,504 bytes; the blob is zero-padded to that size.
- Per-file overhead: 6-byte header + up to 511 bytes of 512-byte padding — many
  tiny files (single letters, short frame sets) cost more than their bytes.
- `minify_json.py` **hard-fails (`exit 1`)** if the minified total exceeds
  917,504 bytes; warns at 90%.
- Frame-heavy animations dominate (`newyear/fireworks` 30 frames, etc). Each
  frame is a full scene.
- `json_datain_2_FFAT.bat --dry-run` reports the exact total and % used.

### Saving space
1. **Animation ⇒ no separate static scene.** Point a content item at an
   animation frame file if you need a still.
2. **Thin frames.** Halve a smooth walk cycle at 2× per-frame `durationMs`.
3. **Reuse identical frames.** A 2-state loop needs 2 files, not 8.
4. **Drop redundant variants** (one crawl direction, not both).
