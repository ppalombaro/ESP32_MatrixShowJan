# ESP32 MatrixShow V16.4.13 — Architecture & Module Reference

Companion to [README.md](README.md). Reflects branch `main-v16.4`, which compiles
clean (`pio run -e esp32dev` → SUCCESS).

---

## Runtime flow

### `setup()` (`src/main.cpp`)
1. `Serial.begin(115200)`; boot banner uses `FW_VERSION` (`Config.h`).
2. `display.begin()` — `FastLED.addLeds` for Matrix 0 (GPIO 16) and Matrix 1
   (GPIO 17) into one `leds[TOTAL_LEDS]` buffer; restores brightness from NVS
   (`show-config` / `brightness`).
3. `content.begin(&display)` — `readCustomStorage()` parses the flash blob at
   `0x290000`, builds `contentRegistry`, `registerProceduralAnimations()`, then
   `loadScheduleFromNVS()` + `loadEligibleFromNVS()`.
4. `WiFi.mode(WIFI_STA); WiFi.setHostname(HOSTNAME); WiFi.begin(WIFI_SSID, WIFI_PASSWORD)`
   — up to 20×500 ms. On success: `configTzTime("EST5EDT,M3.2.0/2,M11.1.0/2",
   "pool.ntp.org", "time.nist.gov")` starts SNTP + the local timezone (DST
   automatic). There is no `NTPClient` — `time(nullptr)` / `getLocalTime()` are
   used everywhere.
5. `themeManager.begin(&display, &content)`
6. `web.begin(...)` — `WebServer` on port 80, page + `/api/*` routes.

No mDNS. No FreeRTOS web task. No `esp_task_wdt`.

### `loop()`
```cpp
web.handle();            // WebServer::handleClient
themeManager.update();   // thin (no-op today)
content.update();        // schedule gate + random rotation + playback advance
delay(10);
```

**Nothing in the render path blocks.** `renderContent()` parses/sets up playback
state and returns; `ContentManager::update()` advances it every tick, so
`web.handle()` keeps running throughout playback.

---

## Modules

### `Config.h` / `Config.cpp`
- `FW_VERSION` — single firmware version string (banner + web nav).
- Wi-Fi/hostname: `#include "Secrets.h"` (git-ignored; copy from
  `Secrets.h.example`). Build fails with a clear `#error` if `Secrets.h` is
  absent.
- Feature flags: `ENABLE_MEGAMATRIX false`, `ENABLE_MEGATREE false`.
- Matrix 0 = source of truth: `MATRIX0_ROWS 25`, `MATRIX0_COLS 20`,
  `PIN_MATRIX0 16`. Matrix 1 aliases the dims, `PIN_MATRIX1 17`. Legacy aliases:
  `ROWS`/`COLS`/`MATRIX_LEDS` → Matrix 0; `PIN_LEFT = PIN_MATRIX1`,
  `PIN_RIGHT = PIN_MATRIX0`. `TOTAL_LEDS` = 1000.
- `LED_TYPE WS2811`, `COLOR_ORDER RGB`, `DEFAULT_BRIGHTNESS 20`.
- Run modes: `RUN_MODE_MANUAL 0`, `RUN_MODE_SCHEDULE 1`.
- Themes: `THEME_OFF 0`, `THEME_CHRISTMAS 1`, `THEME_HALLOWEEN 2`,
  `THEME_THANKSGIVING 3`, `THEME_NEWYEAR 4`, `THEME_OSU 5`.
- NVS namespace `PREFS_NAMESPACE` = `"show-config"`. Keys in use:
  `brightness`, `run_mode`, `sch_sh`/`sch_sm`/`sch_eh`/`sch_em` (schedule
  window), `rand_excl` (newline-joined excluded content keys).
- Theme palettes `CHRISTMAS_COLOR_1..3` … `OSU_COLOR_1..3`. Custom named colours
  (`Peach`, `Sienna`, …) defined in `Config.cpp`.

### `MatrixDisplay.*`
Owns `CRGB leds[TOTAL_LEDS]`. Row-major **serpentine**, physical LED 0
bottom-left; `xyToIndex()` flips Y so callers use a top-left origin.
`getIndex(matrix,x,y)` adds the `matrix*MATRIX_LEDS` base, bounds-checked. API:
`setPixel`, `getPixel`, `clear`, `clearMatrix`, `fadeAll`, `drawCircle`,
`getMatrixRows/Cols(m)`, `setBrightness`.

### `ContentManager.*` — the core
`ContentItem`: `{ uint16_t id; String name, theme; ContentType type; String path;
unsigned long durationMs; String matrix0Scene, matrix1Scene, matrix2Scene; }`.
`FileEntry`: `{ String path; uint32_t offset, size; }`.
`PlaybackFrame`: `{ String sceneRef, matrix0Scene, matrix1Scene; unsigned long durationMs; }`.

**`begin()` → `readCustomStorage()`** — reads `DATA_PARTITION_OFFSET 0x290000`.
First 4 bytes = file count (rejected if 0 or > 500). Per file:
`[2B pathLen][path][4B contentLen][content]`, then pad to the next 512-byte
boundary. Each file → a `FileEntry`. Registration by path:

| Path pattern | Registered as |
|---|---|
| `scenes/<theme>/*.json` | `CONTENT_SCENE` (reads `durationMs`, `matrix0/1/2Scene`) |
| `*_timeline.json` / `animation_timeline.json` | `CONTENT_ANIMATION` (`durationMs` or sum of frames) |
| `scroll/<theme>/*.json` | `CONTENT_SCROLL` |
| `countdown/<theme>/*.json` | `CONTENT_COUNTDOWN` |
| `test/*.json` | `CONTENT_TEST` |

`registerProceduralAnimations()` adds `Chase`, `Snowfall`, `Snowfall Gentle`,
`Snowfall Heavy`, `Sparkling Stars` (theme `christmas`) and `Color Wave` (theme
`osu`) — all have implementations in the `Animations` namespace.

`resolveScenePath(timelinePath, sceneName)` — resolves a frame's relative `scene`
name against the timeline's directory, adds `.json` if missing.

Query API: `getContent()`, `getContentById(id)`, `getContentByTheme(theme)`,
`getDiscoveredThemes()`, `getStatus()`.

### Non-blocking playback engine

`renderContent(id)`:
1. `themeManager.setTheme(ThemeManager::themeNameToId(item->theme))` — so
   Scroll/Countdown get the right palette.
2. resets playback state, then per type:

| type | `renderContent` sets up | `updatePlayback()` each tick |
|---|---|---|
| `CONTENT_SCENE` | `clear()` + `drawSceneFile(0,matrix0Scene)` + `drawSceneFile(1,matrix1Scene)` + `show()` | nothing (static, held) |
| `CONTENT_ANIMATION` | parse timeline → `playFrames`, `playLoop = doc["loop"]`, draw frame 0 | when `millis()-playFrameStart ≥ frame.durationMs`: advance (`playLoop` wraps, else stop and hold last frame), `drawPlayFrame()` |
| `CONTENT_SCROLL` | `scroll->loadFromJSON(path)` + `scroll->begin()` | `scroll->update()` |
| `CONTENT_COUNTDOWN` | `countdown->loadFromJSON(path)` + `countdown->begin()` | `countdown->update()` |
| `CONTENT_PROCEDURAL` | record `name` | `runProceduralTick(name)` → one `Animations::*` call; each is internally rate-limited |
| `CONTENT_TEST` | `clear()` + red fill `x<COLS, y<ROWS` + `show()` | nothing (static, held) |

`stopPlayback()` clears state + blanks the display (called by `/api/clear` and
the scheduler when it leaves the window).

`drawSceneFile(matrix, sceneRef, basePath)` — `resolveScenePath` → `findFile`
(exact path, then bare-name `/<name>.json` suffix) → `esp_flash_read` →
`DynamicJsonDocument(49152)` → for each `pixels[]` entry
`disp->setPixel(matrix, centerX + x, centerY + y, CRGB(r,g,b))`.

### Scheduler (V16.4.13)

`uint8_t runMode` — `RUN_MODE_MANUAL` (random/manual runs 24/7) or
`RUN_MODE_SCHEDULE` (a daily on/off window gates everything). `enableScheduler(bool)`
/ `isSchedulerEnabled()` are thin wrappers over `runMode`.

- Window: `schStartH/M`, `schEndH/M` (u8). `setScheduleWindow(...)` and
  `setRunMode(...)` persist to NVS (`saveScheduleToNVS`); `loadScheduleFromNVS()`
  runs at boot (defaults 17:00–22:00, MANUAL).
- `computeInWindow()`: `nowLocal(struct tm&)` (wraps `getLocalTime(&t, 50)`) →
  if the clock hasn't synced yet, returns **true** (display stays ON) and logs
  once. Otherwise compares minutes-of-day; `start == end` = always on;
  `start < end` = normal; `start > end` = **wraps past midnight**.
- Enforcement is at the top of `update()`: in SCHEDULE mode, outside the window
  → `stopPlayback()` once, set `scheduleBlanked`, `return`. Re-entering the
  window → clear `scheduleBlanked`, reset the random timer, pick a random item
  if random mode is on.
- `isScheduleActive()` (exposed via `/api/status`) reflects the current gate.

### Eligible-content mask (V16.4.13)

Random rotation skips `CONTENT_TEST` **and** any item the user excluded.
Persistence is keyed on a stable string, not the discovery-order id:
`contentKey(item)` = `"<type>|<theme>|<name>"`. The **excluded** set is stored as
a newline-joined string in NVS (`rand_excl`) — "nothing saved" = "all eligible".
`setExcludedByIds("3,7,12")` resolves ids → keys and persists. `isEligible(item)`
is checked in `selectRandomContent()`. A renamed content file silently re-enters
rotation (fails open).

### `ThemeManager.*`
`begin(display, content)`, `update()` (no-op), `setTheme(uint8_t)`,
`getCurrentTheme()`, `renderContent(uint16_t)` (delegates to ContentManager),
`getColor1/2/3()` (switch on the `THEME_*` constants → the `*_COLOR_*` macros
from `Config.h`; `THEME_OFF`/unknown → dim white), and
`static themeNameToId(const String&)` (`"osu"` → `THEME_OSU`, etc).

### `Animations.*`
Procedural effects as a **namespace** (`chase`, `snowfall`, `snowfallGentle`,
`snowfallHeavy`, `sparklingStars`, `colorWave`), each taking `MatrixDisplay*` and
each internally rate-limited (`if (millis()-lastUpdate < N) return;`) so it is
safe to call once per `loop()` tick. `colorWave` is a horizontal `CHSV` rainbow
sweep. `Animations.h` also declares an unused `Animation` base class.

### `Scroll.*`
`Scroll(MatrixDisplay*, ThemeManager*)`; `loadFromJSON(path)` reads `text` /
`speed` straight from the flash blob (own flash-walk, exact-path match),
uppercases the text (`FONT_5X7` is ASCII 32–90). `update()` scrolls one step per
`speed` ms across both 20-wide windows, cycling theme colours each pass.
Constructed in `ContentManager::begin()`; driven by `updatePlayback()`.

### `Countdown.*`
`Countdown(MatrixDisplay*, ThemeManager*)`; `loadFromJSON(path)` reads
`targetDate` (`"YYYY-MM-DD HH:MM:SS"` via `parseHumanDate()`, or an epoch int).
`update()` (1 Hz) draws D/H on the left window, M/S on the right (`DIGIT_3X5`
font); header = theme colour 1, box = 2, digits = 3; flashes `00` past the
target. `parseHumanDate` uses `mktime` with `tm_isdst = -1` — since the TZ is set
(`configTzTime`), the fields are interpreted as **local wall-clock**, DST-aware,
and compared against `time(nullptr)`. If the clock hasn't synced, `update()`
returns without drawing.

### `WebController.*`
`WebServer server{80}`. `begin(content, themes, display)` creates a `WebActions`,
`setupRoutes()`, `actions->attach()`. Page routes (`HTTP_GET`): `/` (→ `/control`),
`/control`, `/schedule`, `/times`, `/logs`, `/discovery`.

### `WebActions.*` — `/api/*` (all `HTTP_GET`)

| Endpoint | Action |
|---|---|
| `/api/render?id=<n>` | `renderContent(n)` |
| `/api/clear` | `stopPlayback()` + blank |
| `/api/test` | `stopPlayback()` + solid red fill |
| `/api/status` | JSON `{id,name,type,theme,scheduleActive}` — "now playing" |
| `/api/random/enable` · `/disable` | random rotation on/off |
| `/api/random/interval?ms=<n>` | rotation interval |
| `/api/random/filter?theme=<t>` | restrict rotation to a theme |
| `/api/random/eligible?exclude=<ids>` | set the excluded-content list (persisted) |
| `/api/random/eligible/get` | JSON array of excluded ids |
| `/api/scheduler/enable` · `/disable` | run mode SCHEDULE / MANUAL |
| `/api/schedule/times?start=HH:MM&end=HH:MM` | set + persist the daily window |
| `/api/schedule/get` | JSON `{startH,startM,endH,endM,runMode,active}` |
| `/api/brightness?value=<1-255>` · `/api/brightness/get` | brightness (NVS `show-config`) |
| `/api/theme/set?id=<n>` | `ThemeManager::setTheme` (mostly for testing) |
| `/api/logs/clear` | clear the log buffer |

### `WebPages.*`
HTML string templates. `/control` shows brightness, Clear / Test buttons, a
polled "Now playing" line, and per-theme grids of Preview buttons. `/schedule`
has the random-mode controls + the eligible-content checkbox list (wired to
`/api/random/eligible`). `/times` shows the run mode + a MANUAL/SCHEDULE toggle
and the daily window inputs (wired to `/api/schedule/times`).

### `Logger.h`
`Logger::instance()` singleton; `log(String)` → `Serial` + an in-memory ring
buffer surfaced on `/logs`.

---

## Storage format (custom "simple storage")

`tools/FFAT/build_simple_storage.py` packs a directory into:

```
[4 bytes LE] file count
repeat per file:
  [2 bytes] path length (UTF-8, ≤ 255)
  [N bytes] path, forward-slash separated, relative to data_in/
  [4 bytes] content length
  [M bytes] content
  [pad]     zero-fill to the next 512-byte boundary
```

Image size fixed at `FFAT_SIZE = 917504`, zero-padded. `generate_manifest.py`
writes `ffat_manifest.txt` (`path,length,sha256`). Flashed raw with
`esptool.py ... write_flash 0x290000 <image>`. **Not** a FAT/SPIFFS filesystem —
`readCustomStorage()` walks the bytes directly.

---

## JSON schemas (`data_in/`)

### Scene — `scenes/<theme>/<name>.json`
```json
{
  "name": "christmas_tree", "theme": "christmas",
  "centerX": 10, "centerY": 12, "durationMs": 5000,
  "matrix0Scene": "scenes/christmas/christmas_tree.json",
  "matrix1Scene": "scenes/christmas/christmas_tree.json",
  "pixels": [ { "x": 1, "y": -11, "r": 253, "g": 255, "b": 112 } ]
}
```
`pixels[].x/y` are offsets from `centerX/centerY`. `matrix0Scene` → right window,
`matrix1Scene` → left window (defaults: `matrix1Scene` mirrors `matrix0Scene`,
`matrix0Scene` is the file's own path). `drawSceneFile()` renders these.

### Animation — `animations/<theme>/<name>/`
`animation_timeline.json` (or `<name>_timeline.json`) + per-frame scene files:
```json
{
  "animationId": "spooky_eyes", "theme": "halloween", "loop": true,
  "frames": [
    { "scene": "frame_00", "durationMs": 100 },
    { "matrix0Scene": "eye_right", "matrix1Scene": "eye_left", "durationMs": 500 }
  ]
}
```
- A frame carries `scene` (same art both windows) **or**
  `matrix0Scene`/`matrix1Scene` (per-window art). Names resolve against the
  timeline folder (`resolveScenePath`).
- `loop: true` repeats the timeline; without it, playback stops on and holds the
  last frame.
- Total duration = `durationMs` if set, else the sum of `frames[].durationMs`.

### Scroll — `scroll/<theme>/<name>.json`
```json
{ "text": "GO BUCKS", "speed": 50 }
```
`text` uppercased on load; `speed` = ms per 1-pixel step.

### Countdown — `countdown/<theme>/<name>.json`
```json
{ "targetDate": "2026-11-28 12:00:00", "comment": "OSU vs Michigan", "durationMs": 5000 }
```
`targetDate` = local wall-clock string or an epoch int.

### Test — `test/*.json` or `scenes/test/*.json`
Registered as `CONTENT_TEST`; the renderer ignores the file and does a red fill.

---

## Known gaps / cleanup backlog

| Item | Where | Note |
|---|---|---|
| ~~Scene / animation / scroll / countdown rendering~~ | `ContentManager` | ✅ V16.4.11–12 — all types render |
| ~~Blocking `delay()` in the render path~~ | `renderContent` | ✅ V16.4.12 — non-blocking `updatePlayback()` |
| ~~`loop: true` ignored~~ / ~~`CONTENT_TEST` bounds~~ / ~~"Color Wave" no impl~~ | `renderContent` / `Animations` | ✅ V16.4.11–12 |
| ~~Scroll/Countdown assumed 25-wide windows~~ | `Scroll.cpp`, `Countdown.cpp` | ✅ V16.4.12 — use `COLS`/`ROWS` |
| ~~Theme colours off-by-one, never set~~ | `ThemeManager` | ✅ V16.4.13 — `THEME_*` constants + `themeNameToId`, set in `renderContent` |
| ~~Scheduler time window not implemented~~ | `ContentManager` | ✅ V16.4.13 — persisted window, midnight-wrap, enforced in `update()` |
| ~~Eligible-content mask not implemented~~ | `ContentManager` | ✅ V16.4.13 — persisted, keyed on `type\|theme\|name` |
| ~~Hard-coded TZ offset / NTPClient~~ | `main.cpp` | ✅ V16.4.13 — `configTzTime` with `EST5EDT` (DST automatic), NTPClient removed |
| ~~Dead `Scheduler.*` / `SceneData.*` / stub files~~ | `src/` | ✅ V16.4.13 — deleted |
| ~~Wi-Fi credentials hard-coded~~ | `Config.h` | ✅ V16.4.13 — moved to git-ignored `Secrets.h` (rotate the router password; still in pre-V16.4.13 history) |
| ~~Brightness NVS namespace split~~ | `WebActions` / `MatrixDisplay` | ✅ V16.4.13 — unified to `show-config` (old `matrixshow` value orphaned) |
| ~~Web response mojibake~~ | `WebActions` / `WebPages` | ✅ V16.4.13 — plain ASCII |
| Scroll font uppercase-only | `Scroll.cpp` `FONT_5X7` | ASCII 32–90; add lower-case glyphs if wanted |
| No mDNS | `WebController` | `.local` won't resolve — IP only |
| No watchdog | `main.cpp` | risky for an unattended seasonal display |
| `-DBOARD_HAS_PSRAM` + cache-issue flag | `platformio.ini` | left as-is (board PSRAM status unconfirmed); drop if WROOM-32 |
| `app0` (1.5 MB) ≠ `app1` (1 MB) | `partitions.csv` | OTA needs equal slots — repartition first |
| For a stable eligible-content mask, don't rename `data_in/` files | authoring | rename = exclusion silently lost |
