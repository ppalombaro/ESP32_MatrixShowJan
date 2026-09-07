# Installation & Build Guide — V16.4.13

For branch **`main-v16.4`**. Compiles clean: `pio run -e esp32dev` → `[SUCCESS]`
(Flash ~59% / 931 KB of 1.5 MB, RAM ~17%).

Read [README.md](README.md) first — every content type renders, playback is
non-blocking, and the daily scheduler + random eligible-content mask are live.

---

## 1. Toolchain

| Tool | Notes |
|---|---|
| VS Code + **PlatformIO IDE** extension | or PlatformIO Core CLI |
| ESP32 platform | `platform = espressif32@6.12.0` (pinned) — auto-installed on first build (~500 MB) |
| FastLED `^3.6.0` | resolves to 3.10.x |
| ArduinoJson `^6.21.3` | **6.x only — not 7.x** |
| Python 3 + `esptool` | for the data-flash script; esptool ships with the ESP32 platform at `~/.platformio/packages/tool-esptoolpy/esptool.py` |
| CP210x USB driver | for the DevKitC serial port |

No `NTPClient` any more — time comes from the ESP32 SNTP client started by
`configTzTime()` in `main.cpp`.

---

## 2. Configure

### 2a. Wi-Fi credentials (required — the build fails without this)
```bash
cp src/Secrets.h.example src/Secrets.h
```
Edit `src/Secrets.h`:
```c
#define WIFI_SSID     "your-2.4GHz-ssid"
#define WIFI_PASSWORD "your-password"
#define HOSTNAME      "palombaro-matrix"
```
`src/Secrets.h` is git-ignored. `Config.h` `#include`s it and stops with a clear
`#error` if it is missing.

### 2b. `platformio.ini`
```ini
[env:esp32dev]
platform = espressif32@6.12.0
board = esp32dev
framework = arduino
monitor_speed = 115200
monitor_port = COM3
upload_port = COM3
upload_speed = 921600
board_build.partitions = partitions.csv
build_flags =
    -DCORE_DEBUG_LEVEL=3
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue
lib_deps =
    fastled/FastLED@^3.6.0
    bblanchon/ArduinoJson@^6.21.3
```
Adjust:
- `monitor_port` / `upload_port` — your actual COM port (Device Manager → Ports →
  "Silicon Labs CP210x"); `/dev/ttyUSB0` on Linux.
- `-DBOARD_HAS_PSRAM` / `-mfix-esp32-psram-cache-issue` — kept for now. **Remove
  both** if your module is a plain WROOM-32 (no PSRAM); keep them for a WROVER.
- `CORE_DEBUG_LEVEL=3` is verbose — drop to `2` for production.

### 2c. `src/Config.h`
- Confirm `PIN_MATRIX0 16`, `PIN_MATRIX1 17`, `MATRIX0_ROWS/COLS 25/20`.
- `DEFAULT_BRIGHTNESS 20` — keep low on the bench (1000 WS2811 px at full white is a big current draw).
- Timezone: `main.cpp` uses the POSIX string `EST5EDT,M3.2.0/2,M11.1.0/2`
  (US Eastern, DST automatic). Change `TZ_STRING` for another zone.

---

## 3. Build & flash firmware

```bash
pio run -e esp32dev            # compile (first run is slow - full framework build)
pio run -e esp32dev -t upload  # flash over USB
pio device monitor            # serial @ 115200
```
VS Code: **Ctrl+Alt+B** build, **Ctrl+Alt+U** upload+monitor, **Ctrl+Alt+S** monitor.

Expected boot log:
```
ESP32 Matrix Show V16.4.13
[SETUP] Display initialized
[ContentManager] Reading custom flash storage...
[ContentManager] Files in storage: <N>
[Schedule] Loaded: mode=MANUAL 17:0-22:0
[SETUP] Connecting to WiFi...
[SETUP] WiFi connected: 192.168.x.x
[SETUP] SNTP started (TZ EST5EDT,M3.2.0/2,M11.1.0/2)
[SETUP] System ready!
```
If `Files in storage:` is `0` / absurd, or `Custom storage read FAILED` — the
data blob isn't flashed. Do step 4.

---

## 4. Pack & flash the content blob

Content is a **custom indexed blob** (not a filesystem) at flash offset
**`0x290000`**. Source files live in `data_in/`.

### Windows
```bat
json_datain_2_FFAT.bat             REM minify -> manifest -> build blob -> flash 0x290000 -> clean
json_datain_2_FFAT.bat --dry-run   REM minify + manifest only
```
Edit the `set` lines at the top for your COM port.

### Manual / cross-platform
```bash
python tools/FFAT/minify_json.py         data_in/ data/
python tools/FFAT/generate_manifest.py   data/ ffat_manifest.txt
python tools/FFAT/build_simple_storage.py data/ ffat.bin 917504 --manifest ffat_manifest.txt
python ~/.platformio/packages/tool-esptoolpy/esptool.py \
    --chip esp32 --port COM3 --baud 921600 write_flash 0x290000 ffat.bin
rm -rf data/
```
`ffat.bin` at the repo root is a previously-built blob — only flash it directly
if it matches the current `data_in/`.

See [SPIFFS_UPLOAD_GUIDE.md](SPIFFS_UPLOAD_GUIDE.md) for the format + schemas.

---

## 5. First-boot checklist

- [ ] Serial banner shows `V16.4.13`; web nav shows `V16.4.13`.
- [ ] Serial: `Files in storage: <N>` matches your `data_in/` count.
- [ ] Serial: `WiFi connected: <IP>` then `SNTP started`.
- [ ] `http://<IP>/` → the control page loads; `/discovery` lists content.
- [ ] `http://<IP>/api/test` → both windows light **solid red**.
- [ ] `/api/render?id=<a scene id>` → the scene draws (try `beat_michigan`).
- [ ] `/api/render?id=<an animation id>` → the timeline plays; `loop:true` repeats; the web UI stays responsive.
- [ ] `/api/render?id=<a scroll id>` → text scrolls, colour changes each pass, colours match the theme (OSU = scarlet/grey).
- [ ] `/api/render?id=<the osu countdown id>` → D/H left window, M/S right, counting down (blank for a few seconds until the clock syncs).
- [ ] `/api/render?id=<Color Wave>` → rainbow sweep.
- [ ] `/control` → "Now playing" line updates every 2 s; "Idle" after Clear.
- [ ] `/api/clear` → display blanks **and stays blank**.
- [ ] `/api/brightness?value=40` then reboot → brightness persists.
- [ ] `/schedule` → uncheck an item, Save; enable random → it never appears; survives reboot.
- [ ] `/times` → SCHEDULE mode + a window excluding now → display blanks; a window including now → resumes.

`.local` (mDNS) does **not** work — use the IP.

---

## Troubleshooting

| Symptom | Cause / fix |
|---|---|
| Build stops with `Missing src/Secrets.h` | Do step 2a. |
| First build takes 10-15 min | Normal — full framework compile (longer after a `platform` version change). Later builds are seconds. |
| `deserializeJson` / `StaticJsonDocument` errors | ArduinoJson 7.x — pin `@^6.21.3`. |
| Upload "Failed to connect" | Hold **BOOT** during upload; check cable; close any serial monitor. |
| `Custom storage read FAILED` / `Invalid file count` | Blob not flashed or wrong offset — must be `0x290000`. Re-run step 4. |
| Content shows nothing | Serial for `Scene not found` / `parse error`. Confirm the blob is flashed and `/discovery` lists it. |
| Scene colours look off | `COLOR_ORDER` is `RGB` in `Config.h`; try `GRB`. |
| Scene shifted / clipped | `centerX/centerY` + offset outside 0..19 / 0..24; out-of-range pixels are dropped. |
| Scroll shows blanks / wrong glyphs | Font is ASCII 32-90, uppercased on load. |
| Countdown time wrong | `targetDate` is local wall-clock; `TZ_STRING` in `main.cpp` is US Eastern. Change it for your zone. |
| Countdown blank | Clock not synced yet — it draws once SNTP returns. |
| Display dark, `/discovery` fine | Check `/times` — you may be in SCHEDULE mode outside the window. |
| Themed colours all look like Christmas | Pre-V16.4.13 bug — update firmware. |
| Text mirrored / torn | Serpentine assumptions vs wiring — check `MatrixDisplay::xyToIndex()`. |
| Brownout on bright output | PSU can't supply peak current — lower brightness, inject power. |
| PSRAM warnings | Remove the two PSRAM `build_flags` if the board has no PSRAM. |

---

## Flash map — [`partitions.csv`](../partitions.csv)

| Region | Offset | Size | Contents |
|---|---|---|---|
| `nvs` | 0x9000 | 0x5000 | `Preferences` (`show-config`): brightness, schedule window, run mode, random exclusions |
| `otadata` | 0xe000 | 0x2000 | OTA bookkeeping |
| `app0` | 0x10000 | 0x180000 | firmware slot A (~931 KB used of 1.5 MB) |
| `app1` | 0x190000 | 0x100000 | firmware slot B — **unequal to app0, OTA not usable as-is** |
| `ffat` | 0x290000 | 0xE0000 | custom content blob (917,504 B) |

`pio run -t erase` wipes NVS (brightness / schedule / exclusions back to
defaults); the content blob is a separate flash write and is unaffected.
