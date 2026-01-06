import os, json, sys, hashlib

def sha256_bytes(data: bytes) -> str:
    h = hashlib.sha256()
    h.update(data)
    return h.hexdigest()

if len(sys.argv) < 3 or len(sys.argv) > 4:
    print("Usage: minify_json.py <src_dir> <dst_dir> [--dry-run]")
    sys.exit(1)

SRC = sys.argv[1]
DST = sys.argv[2]
DRY_RUN = len(sys.argv) == 4 and sys.argv[3] == "--dry-run"

total_size = 0
entries = []
dataset_hash = hashlib.sha256()

try:
    for root, _, files in os.walk(SRC):
        for f in sorted(files):
            if not f.endswith(".json"):
                continue

            src = os.path.join(root, f)
            rel = os.path.relpath(src, SRC)
            dst = os.path.join(DST, rel)

            with open(src, "r", encoding="utf-8") as fh:
                data = json.load(fh)

            minified = json.dumps(data, separators=(",", ":")).encode("utf-8")
            size = len(minified)
            file_hash = sha256_bytes(minified)

            total_size += size
            dataset_hash.update(minified)
            entries.append((rel, size, file_hash))

            print(f"{'DRY ' if DRY_RUN else ''}Minimized: {rel} ({size} bytes)")

            if not DRY_RUN:
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                with open(dst, "wb") as out:
                    out.write(minified)

    # Write manifest + checksum to TOOLS folder (not data/)
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    with open(os.path.join(tools_dir, "ffat_manifest.txt"), "w") as m:
        for rel, size, h in entries:
            m.write(f"{rel},{size},{h}\n")

    with open(os.path.join(tools_dir, "ffat_dataset_checksum.txt"), "w") as c:
        c.write(dataset_hash.hexdigest() + "\n")

    print("\nSUMMARY")
    print(f"Files: {len(entries)}")
    print(f"Total size: {total_size} bytes ({total_size/1024:.2f} KB)")
    print(f"Dataset SHA256: {dataset_hash.hexdigest()}")

    sys.exit(0)

except Exception as e:
    print("ERROR:", e)
    sys.exit(1)
