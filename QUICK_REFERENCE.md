# VS Code + PlatformIO - Quick Reference
**V16.4.13.** Full setup: [`SETUP_GUIDE.md`](SETUP_GUIDE.md) · build/flash detail: [`doco/InstallationGuide.md`](doco/InstallationGuide.md)

## 🎯 DAILY WORKFLOW

### Upload Firmware:
```
Ctrl+Alt+U    (upload + open serial monitor)
```

### Build Only (Test Compile):
```
Ctrl+Alt+B    (faster than upload if just checking for errors)
```

### Serial Monitor:
```
Ctrl+Alt+S    (open/close monitor)
```

### Flash Data:
```
1. Open terminal: Ctrl+`
2. Run: json_datain_2_FFAT.bat
3. Then: Ctrl+Alt+U to upload firmware
```

---

## ⌨️ ESSENTIAL SHORTCUTS

### Navigation:
- `Ctrl+P` = Quick open file
- `Ctrl+Shift+F` = Search all files
- `F12` = Go to definition
- `Shift+F12` = Find all references
- `Alt+Left/Right` = Navigate back/forward

### Editing:
- `Ctrl+/` = Comment/uncomment line
- `Ctrl+D` = Select next occurrence
- `Alt+Up/Down` = Move line up/down
- `Ctrl+Shift+K` = Delete line
- `Ctrl+Space` = Trigger autocomplete

### Terminal:
- `Ctrl+`` = Toggle terminal
- `Ctrl+Shift+`` = New terminal
- Type `pio` in terminal to see PlatformIO commands

---

## 🔍 REAL-TIME ERROR DETECTION

### While Typing:
- **Red squiggle** = Syntax error (fix immediately)
- **Yellow squiggle** = Warning (may still compile)
- **Blue squiggle** = Info/suggestion

### Hover Over Code:
- Hover over variable → see type
- Hover over function → see documentation
- Hover over error → see what's wrong

### IntelliSense Autocomplete:
```cpp
display.    ← Type dot, see all methods
    setPixel()
    clear()
    show()
    ...
```

---

## 🐛 DEBUGGING TIPS

### Compiler Errors:
- Click error in terminal → jumps to file:line
- Errors appear in "PROBLEMS" panel (Ctrl+Shift+M)
- Fix top error first (cascading errors common)

### Serial Monitor Not Working:
- Check COM port in platformio.ini
- Close Arduino IDE (port conflict)
- Press ESP32 reset button

### Upload Failures:
- Press BOOT button during upload
- Check USB cable
- Verify baud rate: 921600

---

## 📁 FILE STRUCTURE REMINDERS

```
src/main.cpp       ← Your main sketch (was .ino)
src/*.h/cpp        ← All your classes
platformio.ini     ← Configuration (COM port, baud, libraries)
data_in/           ← Your JSON files
tools/FFAT/        ← Python scripts
```

---

## 🚀 COMPARE TO ARDUINO IDE

| Task | Arduino IDE | VS Code |
|------|-------------|---------|
| Find errors | Compile (30s) | Real-time (0s) |
| Upload | 45s | 10s (incremental) |
| Search all files | Manual | Ctrl+Shift+F |
| Serial monitor | Separate window | Integrated |
| Autocomplete | Basic | Full IntelliSense |
| Multi-file edit | No | Yes |

---

## 📞 NEED HELP?

1. Check SETUP_GUIDE.md for detailed instructions
2. Post error text in chat (include full terminal output)
3. Screenshot if visual issue

**Common Issues:**
- "PlatformIO not ready" → Reload window (Ctrl+Shift+P → Reload Window)
- "Upload failed" → Press BOOT button, close Arduino IDE
- "Library missing" → Check platformio.ini lib_deps section

---

## ✅ FIRST-TIME CHECKLIST

- [ ] VS Code installed
- [ ] PlatformIO extension installed
- [ ] `src/Secrets.h` created from `src/Secrets.h.example` (Wi-Fi creds)
- [ ] Project opens without errors
- [ ] First build succeeds (Ctrl+Alt+B)
- [ ] Upload works (Ctrl+Alt+U)
- [ ] Serial monitor shows boot messages
- [ ] BAT file flashes data successfully

---

**Save this file to your desktop for quick reference!**
