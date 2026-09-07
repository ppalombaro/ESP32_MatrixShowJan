# ESP32 MatrixShow — VS Code + PlatformIO Setup

**V16.4.13.** This is the one-time environment setup. For build/flash/first-boot
detail see [`doco/InstallationGuide.md`](doco/InstallationGuide.md); for the
project overview see [`doco/README.md`](doco/README.md).

The project was migrated from Arduino IDE to PlatformIO in January 2026. The
canonical layout is now `src/` + `main.cpp` + `platformio.ini` (no `.ino`).

---

## 1. Install the toolchain (~15 min)

1. Install **VS Code** — https://code.visualstudio.com/
2. Extensions (Ctrl+Shift+X) → search **PlatformIO IDE** → Install → restart VS Code.
3. Confirm the PlatformIO (alien-head) icon appears in the left sidebar.
4. Install the **Silicon Labs CP210x** USB driver for the DevKitC serial port.

## 2. Open the project

**File → Open Folder** → `D:\patpa\VSCODE\PlatformIO\Projects\ESP32_MatrixShow`
(the actual working copy). PlatformIO auto-detects `platformio.ini`. On first
build it downloads the pinned ESP32 platform (`espressif32@6.12.0`, ~500 MB) and
toolchain — this first build takes 10-15 minutes; later builds are seconds.

## 3. Set your Wi-Fi credentials (required)

```
copy src\Secrets.h.example src\Secrets.h
```
Edit `src\Secrets.h` — set `WIFI_SSID`, `WIFI_PASSWORD`, `HOSTNAME`. `Secrets.h`
is git-ignored; the build stops with a clear `#error` if it is missing.

Check the COM port in `platformio.ini` (`monitor_port` / `upload_port`, default
`COM3`) against Device Manager → Ports → "Silicon Labs CP210x".

## 4. Build / upload / monitor

| Action | Shortcut | CLI |
|---|---|---|
| Build | Ctrl+Alt+B | `pio run -e esp32dev` |
| Upload + monitor | Ctrl+Alt+U | `pio run -e esp32dev -t upload` |
| Serial monitor | Ctrl+Alt+S | `pio device monitor` |

Expected first serial line: `ESP32 Matrix Show V16.4.13`.

## 5. Flash the content

Terminal → `json_datain_2_FFAT.bat` (packs `data_in/` and flashes the blob to
`0x290000`). Firmware and content are flashed separately — see
[`doco/SPIFFS_UPLOAD_GUIDE.md`](doco/SPIFFS_UPLOAD_GUIDE.md).

---

## Troubleshooting

| Problem | Fix |
|---|---|
| Build: `Missing src/Secrets.h` | Do step 3. |
| PlatformIO not detected | Restart VS Code; check the extension is enabled. |
| COM port not found | Device Manager → Ports; update `platformio.ini`. |
| Upload "Failed to connect" / timeout | Hold **BOOT** on the ESP32 during upload; try another cable; close any open serial monitor. |
| `esptool not found` (BAT) | Run a firmware build first — PlatformIO installs `~/.platformio/packages/tool-esptoolpy/esptool.py` on first build. |
| Themed colours all look like Christmas | Old firmware — rebuild from `main-v16.4`. |

See [`doco/InstallationGuide.md`](doco/InstallationGuide.md) for the full
troubleshooting table and the first-boot checklist.
