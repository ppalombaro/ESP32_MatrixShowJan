# --- CONFIG ---

import os
import shutil
import re

# --- CONFIG ---
PROJECT_DIR = r"D:\patpa\Documents\Arduino\ESP32_MatrixShow"  # <-- CHANGE THIS
BACKUP = True
PARTITION_FILE = os.path.join(PROJECT_DIR, "partitions.csv")  # Optional, auto-create if missing
FFAT_PARTITION_NAME = "ffat"  # Name of storage partition

# --- FUNCTIONS ---
def backup_file(file_path):
    backup_path = file_path + ".bak"
    shutil.copyfile(file_path, backup_path)
    print(f"Backup created: {backup_path}")

def replace_spiffs_with_ffat(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    original_content = content

    # Replace include statements
    content = re.sub(r'#include\s*<SPIFFS\.h>', '#include <FFat.h>', content, flags=re.IGNORECASE)

    # Replace SPIFFS.begin() calls
    content = re.sub(r'SPIFFS\.begin\s*\((.*?)\)', r'FFat.begin(\1)', content, flags=re.IGNORECASE)

    # Replace other SPIFFS references
    content = re.sub(r'\bSPIFFS\b', 'FFat', content)

    if content != original_content:
        if BACKUP:
            backup_file(file_path)
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Updated: {file_path}")

def ensure_ffat_partition(partition_file):
    # If no partition file exists, create a default one with FFAT
    if not os.path.isfile(partition_file):
        print(f"No partition table found, creating {partition_file} with FFAT storage.")
        content = "name,type,subtype,size,flags\nffat,data,fat,2M,\n"
        with open(partition_file, 'w', encoding='utf-8') as f:
            f.write(content)
        return

    # Read current partitions
    with open(partition_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # Check if FFAT partition already exists
    if any(FFAT_PARTITION_NAME in line for line in lines):
        print(f"FFAT partition already exists in {partition_file}")
        return

    # Add FFAT partition at the end
    lines.append(f"{FFAT_PARTITION_NAME},data,fat,2M,\n")
    if BACKUP:
        backup_file(partition_file)
    with open(partition_file, 'w', encoding='utf-8') as f:
        f.writelines(lines)
    print(f"FFAT partition added to {partition_file}")

# --- MAIN ---
print("🚀 Starting SPIFFS -> FFAT migration...")
for root, dirs, files in os.walk(PROJECT_DIR):
    for file in files:
        if file.endswith(('.ino', '.cpp', '.h')):
            file_path = os.path.join(root, file)
            replace_spiffs_with_ffat(file_path)

ensure_ffat_partition(PARTITION_FILE)
print("✅ Migration and partition update completed.")
