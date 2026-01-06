import os
import json

# Path to your data folder
DATA_FOLDER = r"D:\patpa\Documents\Arduino\ESP32_MatrixShow\data"

total_size = 0

for root, dirs, files in os.walk(DATA_FOLDER):
    for file in files:
        if file.endswith(".json"):
            path = os.path.join(root, file)
            try:
                with open(path, "r", encoding="utf-8") as f:
                    data = json.load(f)
                # Serialize minified JSON
                minified = json.dumps(data, separators=(",", ":"))
                total_size += len(minified.encode("utf-8"))
            except Exception as e:
                print(f"Error processing {path}: {e}")

print(f"Total minimized JSON size: {total_size} bytes ({total_size/1024:.2f} KB)")
