import argparse
import struct
import hashlib
import os
import sys
import subprocess
import tempfile

SECTOR_SIZE = 512

def parse_manifest(manifest_file):
    """Read manifest CSV: path,length,sha256"""
    files = []
    with open(manifest_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(",")
            if len(parts) != 3:
                continue
            path, length, sha = parts
            files.append((path, int(length), sha))
    return files

def parse_ffat_image(image_file):
    """Return list of tuples (path, content) from FFAT image"""
    files = []
    with open(image_file, "rb") as f:
        data = f.read()

    cursor = 0
    while cursor < len(data):
        if cursor + 1 > len(data):
            break
        name_len = data[cursor]
        cursor += 1
        if name_len == 0 or cursor + name_len + 4 > len(data):
            break
        name = data[cursor:cursor + name_len].decode("utf-8")
        cursor += name_len
        file_size = struct.unpack("<I", data[cursor:cursor + 4])[0]
        cursor += 4
        content = data[cursor:cursor + file_size]
        cursor += ((file_size + SECTOR_SIZE - 1) // SECTOR_SIZE) * SECTOR_SIZE
        files.append((name, content))
    return files

def read_flash_with_esptool(exe_path, port, offset, size):
    """Read flash from ESP32 into a temporary file"""
    with tempfile.NamedTemporaryFile(delete=False) as tmp:
        temp_file = tmp.name
    cmd = [
        exe_path,
        "--chip", "esp32",
        "--port", port,
        "read_flash",
        str(offset),
        str(size),
        temp_file
    ]
    print("Reading flash from ESP32 using esptool...")
    print(" ".join(cmd))
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        print(result.stdout.decode())
        print(result.stderr.decode())
        os.unlink(temp_file)
        raise RuntimeError("Failed to read flash with esptool")
    return temp_file

def verify_flash(exe_path, port, offset, size, manifest_file):
    manifest = parse_manifest(manifest_file)
    print(f"Parsed {len(manifest)} files from manifest.")

    try:
        flash_file = read_flash_with_esptool(exe_path, port, offset, size)
    except Exception as e:
        print("Error reading flash:", e)
        return 1

    try:
        flash_files = parse_ffat_image(flash_file)
    except Exception as e:
        print("Error parsing FFAT image from flash:", e)
        os.unlink(flash_file)
        return 1

    # Compare manifest to flashed files
    all_ok = True
    manifest_dict = {p: sha for p, _, sha in manifest}
    for name, content in flash_files:
        sha_actual = hashlib.sha256(content).hexdigest()
        sha_expected = manifest_dict.get(name)
        if sha_expected is None:
            print(f"WARNING: {name} not found in manifest")
            continue
        if sha_actual != sha_expected:
            print(f"FAIL: {name} SHA mismatch")
            all_ok = False
        else:
            print(f"OK: {name} SHA verified")

    os.unlink(flash_file)
    return 0 if all_ok else 1

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", required=True, help="Path to esptool.exe")
    parser.add_argument("--port", required=True, help="ESP32 serial port")
    parser.add_argument("--offset", type=lambda x: int(x, 0), required=True, help="Flash offset")
    parser.add_argument("--size", type=int, required=True, help="FFAT image size")
    parser.add_argument("--manifest", required=True, help="Manifest file from build")
    args = parser.parse_args()

    result = verify_flash(args.exe, args.port, args.offset, args.size, args.manifest)
    if result == 0:
        print("All files verified successfully!")
    else:
        print("Some files failed verification!")
    sys.exit(result)

if __name__ == "__main__":
    main()
