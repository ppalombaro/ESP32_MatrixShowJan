"""
ffat_utils.py – Minify JSON + Build & Upload FFAT Image (with per-file progress)
Author: Pat Palombaro (updated 2026-01-05)
"""

import os
import sys
import csv
import json
import shutil
import subprocess
import argparse
import tempfile

# ==========================
# CONFIG
# ==========================
PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR    = os.path.join(PROJECT_DIR, "data")
DATA_MIN_DIR = os.path.join(PROJECT_DIR, "data_min")
PARTITIONS_CSV = os.path.join(PROJECT_DIR, "partitions.csv")
FFAT_NAME = "ffat"
DEFAULT_FFAT_OFFSET = 0x100000
DEFAULT_FFAT_SIZE   = 2 * 1024 * 1024  # 2 MB
PORT = "COM3"

# ==========================
# HELPERS
# ==========================
def parse_size(value):
    try:
        value = value.strip().upper()
        if value.endswith("K"):
            return int(value[:-1]) * 1024
        elif value.endswith("M"):
            return int(value[:-1]) * 1024 * 1024
        elif value.startswith("0X"):
            return int(value, 16)
        else:
            return int(value)
    except Exception:
        return None

def get_ffat_offset_and_size():
    if not os.path.isfile(PARTITIONS_CSV):
        print(f"ERROR: partitions.csv not found: {PARTITIONS_CSV}", file=sys.stderr)
        sys.exit(1)

    with open(PARTITIONS_CSV, newline="") as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if not row or row[0].startswith("#") or len(row) < 5:
                continue
            name, _, _, offset_str, size_str = row[:5]
            if name.strip().lower() == FFAT_NAME.lower():
                offset = parse_size(offset_str) or DEFAULT_FFAT_OFFSET
                size   = parse_size(size_str) or DEFAULT_FFAT_SIZE
                return offset, size

    print("WARNING: FFAT partition not found, using defaults.")
    return DEFAULT_FFAT_OFFSET, DEFAULT_FFAT_SIZE

def minify_json_files():
    if os.path.exists(DATA_MIN_DIR):
        shutil.rmtree(DATA_MIN_DIR)
    os.makedirs(DATA_MIN_DIR, exist_ok=True)

    all_files = []
    for root, _, files in os.walk(DATA_DIR):
        for f in files:
            if not f.lower().endswith(".json"):
                continue
            src = os.path.join(root, f)
            rel = os.path.relpath(src, DATA_DIR)
            dst = os.path.join(DATA_MIN_DIR, rel)
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            with open(src, "r", encoding="utf-8") as infile:
                data = json.load(infile)
            with open(dst, "w", encoding="utf-8") as outfile:
                json.dump(data, outfile, separators=(",", ":"))
            all_files.append(rel)
    return all_files

def get_folder_size(folder):
    total = 0
    for root, _, files in os.walk(folder):
        for f in files:
            total += os.path.getsize(os.path.join(root, f))
    return total

def build_ffat_image(file_list):
    """Create a temporary FFAT image using minified JSON, with per-file progress"""
    tmp_file = tempfile.NamedTemporaryFile(delete=False, suffix=".bin")
    tmp_file.close()
    print("==============================")
    print("STEP 3: Build FFAT image")
    print("-------------------------------")

    with open(tmp_file.name, "wb") as f_out:
        for idx, rel_path in enumerate(file_list, 1):
            src_path = os.path.join(DATA_MIN_DIR, rel_path)
            with open(src_path, "rb") as f_in:
                content = f_in.read()
            header = rel_path.encode("utf-8") + b"\n"
            f_out.write(header)
            f_out.write(content)
            f_out.write(b"\n")
            print(f"[{idx}/{len(file_list)}] Packed: {rel_path}")
    return tmp_file.name

def erase_ffat(offset, size):
    print("==============================")
    print("STEP 2: Erase FFAT")
    print("-------------------------------")
    print(f"Erasing FFAT region 0x{offset:X} - 0x{offset+size:X}...")
    cmd = [sys.executable, "-m", "esptool", "--chip", "esp32",
           "--port", PORT, "erase-region", str(offset), str(size)]
    subprocess.run(cmd, check=True)

def upload_ffat_image(offset, image_path, file_list):
    print("==============================")
    print("STEP 4: Upload FFAT image")
    print("-------------------------------")
    total_size = os.path.getsize(image_path)
    print(f"Uploading {len(file_list)} JSON files ({total_size} bytes total)...")
    # Upload the full FFAT image at once
    cmd = [sys.executable, "-m", "esptool", "--chip", "esp32",
           "--port", PORT, "write-flash", hex(offset), image_path]
    subprocess.run(cmd, check=True)
    # Print each file as "uploaded" for visual progress
    for idx, f in enumerate(file_list, 1):
        print(f"[{idx}/{len(file_list)}] Uploaded: {f}")

# ==========================
# MAIN
# ==========================
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Minify JSON + Build & Upload FFAT")
    parser.add_argument("--upload", action="store_true", help="Upload minified JSON to FFAT")
    args = parser.parse_args()

    ffat_offset, ffat_size = get_ffat_offset_and_size()
    print(f"FFAT_OFFSET={ffat_offset}")
    print(f"FFAT_SIZE={ffat_size}")

    print("==============================")
    print("STEP 1: Minify JSON")
    file_list = minify_json_files()
    data_size = get_folder_size(DATA_MIN_DIR)
    print(f"DATA_SIZE={data_size}")
    print("-------------------------------")

    if data_size > ffat_size:
        print("ERROR: Minified data exceeds FFAT partition size!", file=sys.stderr)
        sys.exit(1)

    if args.upload:
        erase_ffat(ffat_offset, ffat_size)
        ffat_image = build_ffat_image(file_list)
        upload_ffat_image(ffat_offset, ffat_image, file_list)
        os.remove(ffat_image)
        print("==============================")
        print("[OK] FFAT JSON upload complete")
