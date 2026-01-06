#!/usr/bin/env python3
"""
Generate All 9 Eye Position JSON Files
V15.0.3.1 - 2025-01-03

This script creates all eye direction files by modifying pupil positions.
Run this in the data/animations/halloween/spooky_eyes/ directory.
"""

import json
import copy

# Base eye with center pupil (all 234 pixels)
base_eye = {
    "sceneId": "left_eye_center",
    "centerX": 10,
    "centerY": 12,
    "description": "Bloodshot eyeball looking at viewer - center position",
    "version": "15.0.3.1",
    "created": "2025-01-03",
    "pixels": [
        {"x": -1, "y": -8, "r": 100, "g": 0, "b": 0},
        {"x": 0, "y": -8, "r": 100, "g": 0, "b": 0},
        {"x": 1, "y": -8, "r": 100, "g": 0, "b": 0},
        {"x": -3, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": -2, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": -1, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": 0, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": 1, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": 2, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": 3, "y": -7, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": -3, "y": -6, "r": 200, "g": 0, "b": 0},
        {"x": -2, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": -1, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": 0, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": 1, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": 2, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": 3, "y": -6, "r": 200, "g": 0, "b": 0},
        {"x": 4, "y": -6, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": -5, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": -5, "r": 200, "g": 0, "b": 0},
        {"x": -3, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": -1, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": 0, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": 1, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": 2, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": 3, "y": -5, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": -5, "r": 200, "g": 0, "b": 0},
        {"x": 5, "y": -5, "r": 100, "g": 0, "b": 0},
        {"x": -6, "y": -4, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": -4, "r": 200, "g": 0, "b": 0},
        {"x": -4, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": -1, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": 0, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": 1, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": 2, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": 3, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": -4, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": -4, "r": 200, "g": 0, "b": 0},
        {"x": 6, "y": -4, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": -3, "r": 100, "g": 0, "b": 0},
        {"x": -6, "y": -3, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": -3, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": -1, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": 0, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": 1, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": 2, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": 3, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": -3, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": -3, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": -3, "r": 100, "g": 0, "b": 0},
        {"x": 7, "y": -3, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": -2, "r": 100, "g": 0, "b": 0},
        {"x": -6, "y": -2, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": -2, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": -2, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": -2, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": -2, "r": 0, "g": 0, "b": 0},  # PUPIL
        {"x": -1, "y": -2, "r": 0, "g": 0, "b": 0},  # PUPIL
        {"x": 0, "y": -2, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 1, "y": -2, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 2, "y": -2, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 3, "y": -2, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": -2, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": -2, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": -2, "r": 100, "g": 0, "b": 0},
        {"x": 7, "y": -2, "r": 100, "g": 0, "b": 0},
        {"x": -8, "y": -1, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": -1, "r": 200, "g": 0, "b": 0},
        {"x": -6, "y": -1, "r": 200, "g": 0, "b": 0},
        {"x": -5, "y": -1, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": -1, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": -1, "r": 0, "g": 0, "b": 0},  # PUPIL
        {"x": -2, "y": -1, "r": 0, "g": 0, "b": 0},  # PUPIL
        {"x": -1, "y": -1, "r": 0, "g": 0, "b": 0},  # PUPIL
        {"x": 0, "y": -1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 1, "y": -1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 2, "y": -1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 3, "y": -1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 4, "y": -1, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": -1, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": -1, "r": 200, "g": 0, "b": 0},
        {"x": 7, "y": -1, "r": 200, "g": 0, "b": 0},
        {"x": 8, "y": -1, "r": 100, "g": 0, "b": 0},
        {"x": -8, "y": 0, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": 0, "r": 200, "g": 0, "b": 0},
        {"x": -6, "y": 0, "r": 200, "g": 0, "b": 0},
        {"x": -5, "y": 0, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": 0, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": 0, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": -2, "y": 0, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": -1, "y": 0, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 0, "y": 0, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 1, "y": 0, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 2, "y": 0, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 3, "y": 0, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 4, "y": 0, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": 0, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": 0, "r": 200, "g": 0, "b": 0},
        {"x": 7, "y": 0, "r": 200, "g": 0, "b": 0},
        {"x": 8, "y": 0, "r": 100, "g": 0, "b": 0},
        {"x": -8, "y": 1, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": 1, "r": 200, "g": 0, "b": 0},
        {"x": -6, "y": 1, "r": 200, "g": 0, "b": 0},
        {"x": -5, "y": 1, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": 1, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": 1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": -2, "y": 1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": -1, "y": 1, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 0, "y": 1, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 1, "y": 1, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 2, "y": 1, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 3, "y": 1, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 4, "y": 1, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": 1, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": 1, "r": 200, "g": 0, "b": 0},
        {"x": 7, "y": 1, "r": 200, "g": 0, "b": 0},
        {"x": 8, "y": 1, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": 2, "r": 100, "g": 0, "b": 0},
        {"x": -6, "y": 2, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": 2, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": 2, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": 2, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": 2, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": -1, "y": 2, "r": 0, "g": 0, "b": 0},   # PUPIL
        {"x": 0, "y": 2, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 1, "y": 2, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 2, "y": 2, "r": 0, "g": 0, "b": 0},    # PUPIL
        {"x": 3, "y": 2, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": 2, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": 2, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": 2, "r": 100, "g": 0, "b": 0},
        {"x": 7, "y": 2, "r": 100, "g": 0, "b": 0},
        {"x": -7, "y": 3, "r": 100, "g": 0, "b": 0},
        {"x": -6, "y": 3, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": 3, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": -1, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": 0, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": 1, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": 2, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": 3, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": 3, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": 3, "r": 100, "g": 0, "b": 0},
        {"x": 6, "y": 3, "r": 100, "g": 0, "b": 0},
        {"x": 7, "y": 3, "r": 100, "g": 0, "b": 0},
        {"x": -6, "y": 4, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": 4, "r": 200, "g": 0, "b": 0},
        {"x": -4, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": -3, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": -1, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": 0, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": 1, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": 2, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": 3, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": 4, "r": 255, "g": 0, "b": 0},
        {"x": 5, "y": 4, "r": 200, "g": 0, "b": 0},
        {"x": 6, "y": 4, "r": 100, "g": 0, "b": 0},
        {"x": -5, "y": 5, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": 5, "r": 200, "g": 0, "b": 0},
        {"x": -3, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": -2, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": -1, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": 0, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": 1, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": 2, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": 3, "y": 5, "r": 255, "g": 0, "b": 0},
        {"x": 4, "y": 5, "r": 200, "g": 0, "b": 0},
        {"x": 5, "y": 5, "r": 100, "g": 0, "b": 0},
        {"x": -4, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": -3, "y": 6, "r": 200, "g": 0, "b": 0},
        {"x": -2, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": -1, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": 0, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": 1, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": 2, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": 3, "y": 6, "r": 200, "g": 0, "b": 0},
        {"x": 4, "y": 6, "r": 100, "g": 0, "b": 0},
        {"x": -3, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": -2, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": -1, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": 0, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": 1, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": 2, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": 3, "y": 7, "r": 100, "g": 0, "b": 0},
        {"x": -1, "y": 8, "r": 100, "g": 0, "b": 0},
        {"x": 0, "y": 8, "r": 100, "g": 0, "b": 0},
        {"x": 1, "y": 8, "r": 100, "g": 0, "b": 0}
    ]
}

# Pupil positions for each direction
# Format: (offsetX, offsetY, description)
pupil_positions = {
    "center":      (0, 0, "looking at viewer - center position"),
    "up":          (0, -4, "looking up"),
    "down":        (0, 4, "looking down"),
    "left":        (-2, 0, "looking left"),
    "right":       (2, 0, "looking right"),
    "up_left":     (-2, -4, "looking up-left diagonal"),
    "up_right":    (2, -4, "looking up-right diagonal"),
    "down_left":   (-2, 4, "looking down-left diagonal"),
    "down_right":  (2, 4, "looking down-right diagonal")
}

def is_pupil_pixel(pixel):
    """Check if this pixel is a pupil pixel (black)"""
    return pixel["r"] == 0 and pixel["g"] == 0 and pixel["b"] == 0

def generate_eye_position(direction, offset_x, offset_y, description):
    """Generate JSON for specific eye direction by moving pupil pixels"""
    
    eye = copy.deepcopy(base_eye)
    eye["sceneId"] = f"left_eye_{direction}"
    eye["description"] = f"Bloodshot eyeball {description}"
    
    # First pass: mark pupil pixels for removal and collect positions
    pupil_pixels = []
    non_pupil_pixels = []
    
    for pixel in eye["pixels"]:
        if is_pupil_pixel(pixel):
            pupil_pixels.append((pixel["x"], pixel["y"]))
        else:
            non_pupil_pixels.append(pixel)
    
    # Second pass: create new pupil pixels at offset position
    new_pupil_pixels = []
    for x, y in pupil_pixels:
        new_pupil_pixels.append({
            "x": x + offset_x,
            "y": y + offset_y,
            "r": 0,
            "g": 0,
            "b": 0
        })
    
    # Where the old pupil was, we need iris pixels (bright red)
    for x, y in pupil_pixels:
        non_pupil_pixels.append({
            "x": x,
            "y": y,
            "r": 255,
            "g": 0,
            "b": 0
        })
    
    # Combine: non-pupil pixels + new pupil pixels
    eye["pixels"] = non_pupil_pixels + new_pupil_pixels
    
    # Sort by y then x for clean output
    eye["pixels"].sort(key=lambda p: (p["y"], p["x"]))
    
    return eye

def main():
    """Generate all 9 eye position files"""
    
    print("=" * 70)
    print("  Spooky Eyes JSON Generator - V15.0.3.1")
    print("=" * 70)
    print()
    
    # Generate all positions
    for direction, (offset_x, offset_y, description) in pupil_positions.items():
        eye_data = generate_eye_position(direction, offset_x, offset_y, description)
        filename = f"left_eye_{direction}.json"
        
        with open(filename, 'w') as f:
            json.dump(eye_data, f, indent=2)
        
        pixel_count = len(eye_data["pixels"])
        print(f"✓ Generated: {filename:30s} ({pixel_count} pixels)")
    
    print()
    print("=" * 70)
    print(f"  SUCCESS! All 9 eye position files generated")
    print("=" * 70)
    print()
    print("Files created:")
    for direction in pupil_positions.keys():
        print(f"  • left_eye_{direction}.json")
    print()
    print("Next steps:")
    print("  1. Copy these files to data/animations/halloween/spooky_eyes/")
    print("  2. (Optional) Upload to ESP32 SPIFFS for future use")
    print("  3. For now, animation is hardcoded in firmware")
    print()

if __name__ == "__main__":
    main()
