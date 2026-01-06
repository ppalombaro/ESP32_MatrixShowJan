import os, json, sys

SRC = r"D:\patpa\Documents\Arduino\ESP32_MatrixShow\data"
DST = r"D:\patpa\Documents\Arduino\ESP32_MatrixShow\data_min"
FFAT_MAX = 917504

total = 0

try:
    for root, _, files in os.walk(SRC):
        for f in files:
            if f.endswith(".json"):
                src = os.path.join(root, f)
                rel = os.path.relpath(src, SRC)
                dst = os.path.join(DST, rel)
                os.makedirs(os.path.dirname(dst), exist_ok=True)

                with open(src, "r", encoding="utf-8") as fh:
                    data = json.load(fh)

                minified = json.dumps(data, separators=(",", ":"))
                with open(dst, "w", encoding="utf-8") as fh:
                    fh.write(minified)

                size = len(minified.encode("utf-8"))
                total += size
                print(f"Minimized: {rel} ({size} bytes)")

    print(f"\nTOTAL SIZE: {total} bytes ({total/1024:.2f} KB)")

    if total > FFAT_MAX:
        print("ERROR: FFAT limit exceeded")
        sys.exit(1)
    elif total > FFAT_MAX * 0.9:
        print("WARNING: FFAT usage over 90%")
    else:
        print("FFAT usage OK")

except Exception as e:
    print("ERROR:", e)
    sys.exit(1)
