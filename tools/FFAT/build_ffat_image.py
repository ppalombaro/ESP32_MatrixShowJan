import os
import sys
import struct
import hashlib
import argparse

SECTOR_SIZE = 512

def pad_to_sector(data: bytes) -> bytes:
    padding = (-len(data)) % SECTOR_SIZE
    return data + b'\x00' * padding

def collect_files(src_dir):
    entries = []
    for root, _, files in os.walk(src_dir):
        for f in files:
            full_path = os.path.join(root, f)
            rel_path = os.path.relpath(full_path, src_dir).replace("\\","/")
            with open(full_path, "rb") as fh:
                content = fh.read()
            entries.append((rel_path, content))
    return entries

def build_image(entries, max_size):
    image = b""
    for path, content in entries:
        path_bytes = path.encode("utf-8")
        if len(path_bytes) > 255:
            raise ValueError(f"Filename too long: {path}")
        header = struct.pack("<B", len(path_bytes)) + path_bytes + struct.pack("<I", len(content))
        entry = header + content
        image += pad_to_sector(entry)
        if len(image) > max_size:
            raise ValueError("Image exceeds maximum size")
    if len(image) < max_size:
        image += b'\x00' * (max_size - len(image))
    return image

def write_manifest(entries, manifest_file):
    with open(manifest_file, "w") as f:
        for path, content in entries:
            sha = hashlib.sha256(content).hexdigest()
            f.write(f"{path},{len(content)},{sha}\n")
    print(f"Manifest written to {manifest_file}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_dir")
    parser.add_argument("output_file")
    parser.add_argument("max_size", type=int)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--manifest")
    args = parser.parse_args()

    entries = collect_files(args.input_dir)
    print(f"Collected {len(entries)} files")
    total_size = sum(len(pad_to_sector(c)) + len(p.encode("utf-8")) + 5 for p, c in entries)
    print(f"Total size after sector padding: {total_size} bytes")

    if args.manifest:
        write_manifest(entries, args.manifest)

    if args.dry_run:
        print("Dry-run mode: image not written")
        return

    img = build_image(entries, args.max_size)
    with open(args.output_file, "wb") as f:
        f.write(img)
    print(f"FFAT image written to {args.output_file} ({len(img)} bytes)")

if __name__ == "__main__":
    main()
