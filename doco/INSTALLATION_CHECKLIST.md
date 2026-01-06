# V15.1.2 Installation Checklist

## Pre-Installation

- [ ] Arduino IDE installed (1.8.x or 2.x)
- [ ] ESP32 board support installed
- [ ] FastLED library installed
- [ ] ArduinoJson library installed (v6.x)
- [ ] ESP32 Sketch Data Upload plugin installed

## Step 1: Extract Package

- [ ] Extract ZIP to known location
- [ ] Verify folder structure:
  ```
  ESP32_MatrixShow_v15.1.1_Package/
  ├── src/              ✓ Source code
  ├── data/             ✓ SPIFFS content
  ├── tools/            ✓ Utilities
  └── docs/             ✓ Documentation
  ```

## Step 2: Configure WiFi

- [ ] Open `src/Config.h`
- [ ] Update WiFi credentials:
  ```cpp
  #define WIFI_SSID "YourNetworkName"
  #define WIFI_PASS "YourPassword"
  ```
- [ ] Update hostname if desired (optional)
- [ ] Save Config.h

## Step 3: Upload Firmware

- [ ] Open `src/ESP32_MatrixShow_v15_1.ino` in Arduino IDE
- [ ] Verify all files are in IDE tabs (should see 20+ tabs)
- [ ] Select correct board: ESP32 Dev Module
- [ ] Select correct port
- [ ] Click Verify/Compile
  - [ ] No errors?
  - [ ] If errors, check library versions
- [ ] Click Upload
- [ ] Wait for "Done uploading" message

## Step 4: Upload SPIFFS Data (CRITICAL!)

**This step is REQUIRED for V15.1.2 features to work!**

- [ ] Copy `data/` folder to sketch directory:
  ```
  YourDocuments/Arduino/ESP32_MatrixShow_v15_1/
  ├── ESP32_MatrixShow_v15_1.ino
  ├── [all .h/.cpp files]
  └── data/              ← Copy here!
      ├── schedule.json
      └── scenes/
  ```
  
- [ ] In Arduino IDE:
  - [ ] Tools → ESP32 Sketch Data Upload
  - [ ] Wait for upload to complete (30-60 seconds)
  - [ ] Look for "SPIFFS Upload Complete" message

## Step 5: Verify Operation

- [ ] Open Serial Monitor (115200 baud)
- [ ] Reset ESP32 (press reset button)
- [ ] Check startup messages:
  ```
  ✓ "SPIFFS mounted successfully"
  ✓ "Content discovered: X scenes, Y animations"
  ✓ "Web server started"
  ✓ "WiFi connected"
  ```

## Step 6: Test Web Interface

- [ ] Open browser
- [ ] Navigate to `http://matrix.local`
  - If doesn't work, use IP address from Serial Monitor
- [ ] Verify web interface loads
- [ ] Check "Preview" page shows auto-discovered content
- [ ] Test scene selection
- [ ] Verify LED display responds

## Step 7: Upload More Content (Optional)

- [ ] Convert more scenes:
  ```bash
  cd tools/
  python3 scene_converter.py ../src/Scenes.cpp
  ```
- [ ] Copy generated JSON files to `data/scenes/[theme]/`
- [ ] Re-upload SPIFFS data
- [ ] Reboot ESP32

## Troubleshooting

### WiFi Won't Connect
- [ ] Double-check SSID/password in Config.h
- [ ] Check Serial Monitor for connection attempts
- [ ] Try moving ESP32 closer to router
- [ ] Verify 2.4GHz WiFi (ESP32 doesn't support 5GHz)

### Web Interface Shows No Content
- [ ] Verify SPIFFS upload completed
- [ ] Check Serial Monitor: "Content discovered: 0 scenes" = problem
- [ ] Re-upload SPIFFS data
- [ ] Check `data/` folder is in correct location

### Scenes Don't Display
- [ ] Verify JSON syntax (use online validator)
- [ ] Check Serial Monitor for error messages
- [ ] Try hardcoded fallback scenes (should always work)
- [ ] Verify pixel coordinates are valid for your matrix size

### Compilation Errors
- [ ] Update ArduinoJson to v6.x (not v7.x)
- [ ] Update FastLED to latest version
- [ ] Verify all .h/.cpp files are in sketch directory
- [ ] Check Config.h has valid settings

## Success Criteria

✅ WiFi connected  
✅ Web interface accessible  
✅ Content auto-discovered from SPIFFS  
✅ Scenes display on LED matrix  
✅ Configuration pages work  
✅ Random show mode cycles through content  

## Post-Installation

- [ ] Configure show schedule in web UI
- [ ] Select content for random mode (checkboxes)
- [ ] Test different themes
- [ ] Set desired brightness
- [ ] Bookmark web interface URL

## Getting Help

If issues persist:

1. Check Serial Monitor output
2. Review docs/ folder guides
3. Verify hardware connections (see original README.md)
4. Check original INSTALL.txt for hardware details

---

**Installation complete? Start adding your own content!**
