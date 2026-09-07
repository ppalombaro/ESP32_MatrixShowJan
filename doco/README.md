# ESP32 MatrixShow — V16.4.13

Multi-holiday LED-matrix display controller for an ESP32. Drives two 20×25
WS2811 "window" matrices (a third "megamatrix" and a "megatree" are scaffolded
but disabled) and plays holiday scenes, animations, scrolling text and
countdowns, selected by a web UI, a random rotation, or a daily time schedule.

> **Branch:** `main-v16.4`.
> **Build:** `pio run -e esp32dev` → `[SUCCESS]`.
> **Functional status:** boots, joins Wi-Fi, syncs time (DST-aware), serves the
> web UI, discovers content, and **renders every content type** — scenes, frame
> animations (per-window frames, `loop:true`), scrolling text, countdowns and
> procedural animations. Playback is **non-blocking**. The **daily scheduler**
> (on/off window) and the **random-rotation eligible-content mask** are
> implemented and persisted to NVS.
> See [What works](#what-works).

---

## Branch history — why this is the good one

The GitHub repo (`ppalombaro/ESP32_MatrixShowJan`) has several branches from a
messy January rewrite. Newest / most complete first:

| Branch | Tip | State |
|---|---|---|
| **`main-v16.4`** | local `main` + this branch | **V16.4.x** — clean PlatformIO project (`src/`, `main.cpp`, `platformio.ini`), real `ContentManager` + web layer, custom flash storage. Compiles. Fresh, disconnected git history. |
| `postcompile1` | `12ea22e` | Flat Arduino layout, partial rebuild. Superseded. |
| `ppalombaro-patch-1` | `06b056b` | Early V15.4 snapshot; behind `postcompile1`. |
| `SeptStart` = `origin/main` | `a733c2a` | Gutted "V15.4 fix" skeleton, does not compile. |

`origin/main` on GitHub still points at the broken `a733c2a`. Push `main-v16.4`
and retarget the GitHub default branch to it.

---

## What works

| Area | Status |
|---|---|
| Build (`pio run -e esp32dev`) | ✅ clean |
| Boot, Wi-Fi STA, hostname, SNTP time (DST-aware via `configTzTime`) | ✅ |
| Web pages (`/`, `/control`, `/schedule`, `/times`, `/logs`, `/discovery`) | ✅ |
| Content discovery from the custom flash blob (`ContentManager::readCustomStorage`) | ✅ |
| **Scene** render (`CONTENT_SCENE`) | ✅ `drawSceneFile()` plots `pixels[]`; `matrix0Scene`→right window, `matrix1Scene`→left |
| **Frame animation** (`CONTENT_ANIMATION`) | ✅ non-blocking frame advance in `update()`; per-frame `scene` or `matrix0/1Scene`; honours `loop:true` |
| **Scroll** (`CONTENT_SCROLL`) | ✅ reads `text`/`speed` from the blob, theme-colour cycling; font is uppercase ASCII 32–90 (text uppercased on load) |
| **Countdown** (`CONTENT_COUNTDOWN`) | ✅ `targetDate` (local wall-clock string or epoch) vs system clock, flashes `00` past target; blank until the clock syncs |
| **Procedural** (Chase, Snowfall ×3, Sparkling Stars, Color Wave) | ✅ one non-blocking tick per `update()`; Color Wave implemented |
| **Test pattern** (`CONTENT_TEST`, `/api/test`) | ✅ solid red fill both windows |
| Non-blocking playback engine | ✅ `renderContent()` sets state, `update()`→`updatePlayback()` advances; no `delay()` in the render path |
| Theme colours for Scroll/Countdown | ✅ set from the content item's `theme` (`ThemeManager::themeNameToId`), palette from `Config.h` |
| Random rotation + theme filter + **eligible-content mask** | ✅ mask persisted to NVS keyed on `type\|theme\|name` |
| **Daily scheduler** (run mode + on/off window, midnight-wrap) | ✅ persisted to NVS; blanks the display outside the window; SNTP-unsynced ⇒ stays ON |
| "Now playing" indicator (`/api/status`, polled on `/control`) | ✅ |
| mDNS (`palombaro-matrix.local`) | ❌ not wired — access by IP |
| Serial command console / task watchdog | ❌ removed in V16.2.x |
| OTA | ❌ `app0` (1.5 MB) and `app1` (1 MB) are unequal — not usable without repartitioning |

---

## Hardware

| Item | Value | Source |
|---|---|---|
| MCU | ESP32-DevKitC / WROOM-32, PlatformIO board `esp32dev` | `platformio.ini` |
| Matrix 0 | 20 cols × 25 rows (500 px), GPIO 16 — "right window" | `Config.h` |
| Matrix 1 | 20 × 25 (500 px), GPIO 17 — "left window" | `Config.h` |
| LED chipset | `WS2811`, `COLOR_ORDER RGB` | `Config.h` |
| `TOTAL_LEDS` | 1000 | `Config.h` |
| Default brightness | 20 / 255 (`DEFAULT_BRIGHTNESS`) | persisted to NVS `show-config` / `brightness` |
| Wiring | row-major **serpentine**, physical origin bottom-left; Y flipped so the drawing origin is top-left | `MatrixDisplay::xyToIndex()` |
| Wi-Fi / hostname | `src/Secrets.h` (git-ignored) — copy from `src/Secrets.h.example` | — |
| Timezone | `EST5EDT,M3.2.0/2,M11.1.0/2` POSIX string in `main.cpp` (`configTzTime`) | edit for other zones |

### Flash layout — [`partitions.csv`](../partitions.csv)

```
nvs      0x9000    0x5000     Preferences ("show-config"): brightness, schedule, exclusions
otadata  0xe000    0x2000
app0     0x10000   0x180000   firmware slot A
app1     0x190000  0x100000   firmware slot B (unequal to app0 — OTA not usable as-is)
ffat     0x290000  0xE0000    content blob (917,504 B)
```

**Content is not a filesystem.** `data_in/` is packed by
`tools/FFAT/build_simple_storage.py` into a flat indexed blob and flashed raw to
`0x290000`; `ContentManager` reads it with `esp_flash_read()`. Firmware,
`partitions.csv` and the flash script all agree on `0x290000`. See
[SPIFFS_UPLOAD_GUIDE.md](SPIFFS_UPLOAD_GUIDE.md).

---

## Repository layout (`main-v16.4`)

```
ESP32_MatrixShow/
├── platformio.ini          env:esp32dev, platform pinned, partitions.csv
├── partitions.csv
├── json_datain_2_FFAT.bat  data build + flash pipeline (Windows)
├── ffat_manifest.txt        SHA-256 manifest of the last packed blob
│
├── src/                     ALL firmware code
│   ├── main.cpp             setup()/loop(); configTzTime; Wi-Fi
│   ├── Config.h / .cpp      hardware, themes, colours, NVS keys, FW_VERSION; #includes Secrets.h
│   ├── Secrets.h.example    copy to Secrets.h (git-ignored) and set Wi-Fi creds
│   ├── MatrixDisplay.*      LED buffer + serpentine XY mapping
│   ├── ContentManager.*     flash-blob reader, registry, playback engine, scheduler, eligible mask
│   ├── ThemeManager.*       current theme + theme colour accessors + themeNameToId
│   ├── WebController.*      HTTP server + page routes
│   ├── WebActions.*         /api/* handlers
│   ├── WebPages.*           HTML generation
│   ├── Animations.*         procedural effects (namespace)
│   ├── Scroll.*             JSON-driven scrolling text
│   ├── Countdown.*          JSON-driven countdown
│   └── Logger.h             serial + web-buffer logging singleton
│
├── data_in/                 content SOURCE tree (JSON) — the only content folder, tracked
│   ├── schedule.json
│   ├── scenes/<theme>/*.json
│   ├── animations/<theme>/<name>/{animation_timeline.json, frame files}
│   ├── scroll/<theme>/*.json
│   ├── countdown/<theme>/*.json
│   └── test/… , scenes/test/…
│   (data/ = transient minify staging, git-ignored, created+deleted by the .bat.)
│
├── tools/
│   ├── FFAT/                minify_json.py, build_simple_storage.py, generate_manifest.py
│   └── LED_Content_Studio_V16_4_10.html   browser scene/animation editor
│
├── SETUP_GUIDE.md / QUICK_REFERENCE.md   PlatformIO migration notes + shortcuts
└── doco/                    <-- authoritative docs (this folder)
```

Themes present in `data_in/`: `christmas`, `halloween`, `thanksgiving`,
`newyear`, `osu`, plus `test`.

---

## Architecture at a glance

```
 setup()                          loop()  (single thread, ~10 ms tick)
   display.begin()                  web.handle()            ── HTTP /api/*
   content.begin(&display) ──┐      themeManager.update()
   WiFi + configTzTime       │      content.update()        ── schedule gate + playback + random
   themeManager.begin()      │
   web.begin(...)            ▼
        ┌─────────────────────────────────────────────────┐
        │ ContentManager                                   │
        │  readCustomStorage()   <── flash blob @ 0x290000 │
        │  contentRegistry: ContentItem[]                  │
        │  loadScheduleFromNVS() / loadEligibleFromNVS()   │
        │  renderContent(id) → setTheme + set playback state
        │  update():                                       │
        │    if SCHEDULE && outside window → blank, return │
        │    updateRandomMode()  (respects eligible mask)  │
        │    updatePlayback():                             │
        │      SCENE      → drawSceneFile() once, hold     │
        │      ANIMATION  → frame advance on millis, loop  │
        │      SCROLL     → Scroll::update()               │
        │      COUNTDOWN  → Countdown::update()            │
        │      PROCEDURAL → Animations::* one tick         │
        └───────────────┬─────────────────────────────────┘
                        ▼
                 MatrixDisplay ── FastLED ── 2× WS2811 matrices
```

- Single-threaded; no Core-0 web task, no watchdog.
- `ThemeManager` holds `currentTheme` and returns the `Config.h` palette via
  `getColor1/2/3()`; `renderContent()` sets it from the item's `theme`.
- `renderContent()` never blocks; `/api/clear` calls `stopPlayback()`.

See [ARCHITECTURE.md](ARCHITECTURE.md) for the module-by-module reference and
JSON schemas.

---

## First-time setup

1. `cp src/Secrets.h.example src/Secrets.h` and set `WIFI_SSID` / `WIFI_PASSWORD` / `HOSTNAME`. The build fails with a clear `#error` if `Secrets.h` is missing.
2. Build + flash firmware, then pack + flash `data_in/` — see [InstallationGuide.md](InstallationGuide.md).
3. Browse to the ESP32's IP. `/discovery` lists content; `/control` renders it; `/times` configures the daily window.

---

## Known limitations / backlog

- Scroll font is uppercase ASCII 32–90 only.
- `app0`/`app1` partitions are unequal — OTA needs a repartition first.
- No mDNS — access by IP.
- No watchdog on an unattended display.
- Wi-Fi password is still present in pre-V16.4.13 git history (rotate the router password).
- See the full [Known gaps table](ARCHITECTURE.md#known-gaps--cleanup-backlog).

---

## Quick links

- [InstallationGuide.md](InstallationGuide.md) — toolchain, build, flash firmware + data, first boot
- [SPIFFS_UPLOAD_GUIDE.md](SPIFFS_UPLOAD_GUIDE.md) — the custom flash-storage pipeline and JSON schemas
- [ARCHITECTURE.md](ARCHITECTURE.md) — module reference, web API, known gaps
- `tools/LED_Content_Studio_V16_4_10.html` — browser content editor
