# JSON Data → FFAT Pipeline

This project automates the process of:

1. Minifying JSON data
2. Building an FFAT image for ESP32
3. Flashing the image to the ESP32
4. Post-flash SHA256 verification
5. Cleaning up the staging folder

---

## Pipeline Overview

```
data_in/  (raw JSON files)
     |
     v
+-------------------+
|   minify_json.py  |
|   (minify JSON)   |
+-------------------+
     |
     v
data/       (staging folder)
     |
     v
+------------------------+
| build_ffat_image.py    |
| (build FFAT image +    |
|  generate manifest)    |
+------------------------+
     |
     v
ffat.bin
     |
     v
+------------------------+
| Flash using esptool.exe|
+------------------------+
     |
     v
+------------------------+
| verify_ffat_flash.py   |
| (read flash, SHA256    |
|  verification)         |
+------------------------+
     |
     v
Cleanup: data/ folder removed
```

---

## Directory Structure

```
ESP32_MatrixShow/
│
├─ data_in/                  <-- Raw JSON files
├─ data/                     <-- Staging for minified JSONs
├─ ffat.bin                  <-- Output FFAT image
└─ tools/
   └─ FFAT/
       ├─ minify_json.py
       ├─ build_ffat_image.py
       ├─ verify_ffat_flash.py
       └─ json_datain_2_FFAT.bat
```

---

## Usage

### Standard run:

```
tools\FFAT\json_datain_2_FFAT.bat
```

This will:

1. Minify all JSON files in `data_in/` to `data/`
2. Build the FFAT image `ffat.bin`
3. Flash the image to the ESP32
4. Verify the flash against the manifest (SHA256)
5. Clean the staging folder

### Dry-run (no image, manifest only):

```
tools\FFAT\json_datain_2_FFAT.bat --dry-run
```

- Generates manifest and prints sizes
- Does **not** write FFAT image or flash ESP32
- Useful to check dataset size and SHA256 checksums before flashing

---

## Configuration

Edit `json_datain_2_FFAT.bat`:

- `PROJECT_DIR` → Root project folder
- `DATA_IN` → Source JSON folder
- `DATA_OUT` → Staging folder (automatically cleaned)
- `IMAGE_OUT` → Output FFAT image
- `PORT` → ESP32 serial port
- `BAUD` → Flash baud rate
- `FFAT_SIZE` → Max FFAT partition size
- `ESPTOOL` → Path to `esptool.exe`
- `PYTHON` → Python executable (must have esptool and pyserial installed)

---

## FFAT Image Details

- Files are padded to 512-byte sectors
- Each file stored as:  
  `1 byte filename length` + `filename UTF-8` + `4 bytes file size` + `file content`  
- Image size cannot exceed `FFAT_SIZE`

---

## Manifest & SHA256 Verification

- Manifest file is a CSV: `filename,length,sha256`
- Used for post-flash verification
- `verify_ffat_flash.py` reads the ESP32 flash using `esptool.exe` and compares SHA256 of each file
- Reports **OK** or **FAIL** per file
- BAT will pause and exit with error if any file fails verification

**BAT call example:**

```bat
python verify_ffat_flash.py --exe "C:\Users\patpa\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe" --port COM3 --offset 0x290000 --size 917504 --manifest ffat_manifest.txt
```

---

## Requirements

- Python 3.x
- Packages:

```cmd
pip install esptool pyserial
```

- `esptool.exe` available for flashing
- ESP32 connected to the specified COM port

---

## Notes

- No `mkfatfs.exe` required — everything is Python-based
- Staging folder (`data/`) is cleaned automatically after success
- Dry-run mode allows pre-flash validation
- Post-flash SHA256 ensures data integrity

---

## Future Enhancements

- Optional compression before FFAT build
- Multiple FFAT partition support
- Automated retries on flash failures
- Visual report of verification results
