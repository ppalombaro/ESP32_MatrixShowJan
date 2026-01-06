#!/usr/bin/env python3
"""
Scene Converter V15.1.1
Converts hardcoded C++ scenes to JSON format

Usage: python scene_converter.py Scenes.cpp

V15.1.1-2026-01-04T11:15:00Z - Created automated conversion tool
"""

import re
import json
import sys

def parse_setpixel_call(line, default_center=(10, 12)):
    """Extract pixel data from setPixel() call"""
    # Match: setPixel(matrix, x, y, CRGB::Color) or CRGB(r,g,b)
    
    # Try named color first
    match = re.search(r'setPixel\s*\(\s*matrix\s*,\s*(\d+|centerX[^,]*)\s*,\s*(\d+|[^,]*)\s*,\s*CRGB::(\w+)', line)
    if match:
        x_expr, y_expr, color = match.groups()
        x = eval_coordinate(x_expr, default_center[0])
        y = eval_coordinate(y_expr, default_center[1])
        rgb = named_color_to_rgb(color)
        return (x, y, rgb)
    
    # Try RGB color
    match = re.search(r'setPixel\s*\(\s*matrix\s*,\s*(\d+|centerX[^,]*)\s*,\s*(\d+|[^,]*)\s*,\s*CRGB\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)', line)
    if match:
        x_expr, y_expr, r, g, b = match.groups()
        x = eval_coordinate(x_expr, default_center[0])
        y = eval_coordinate(y_expr, default_center[1])
        return (x, y, (int(r), int(g), int(b)))
    
    return None

def eval_coordinate(expr, center):
    """Evaluate coordinate expression relative to center"""
    expr = expr.strip()
    if expr.isdigit():
        return int(expr) - center  # Convert absolute to relative
    
    # Handle centerX +/- offset
    if 'centerX' in expr or 'centerY' in expr or 'cx' in expr or 'cy' in expr:
        # Simple arithmetic
        expr = expr.replace('centerX', '0').replace('centerY', '0')
        expr = expr.replace('cx', '0').replace('cy', '0')
        try:
            return eval(expr)
        except:
            return 0
    
    return 0

def named_color_to_rgb(color_name):
    """Convert CRGB named color to RGB tuple"""
    colors = {
        'Red': (255, 0, 0),
        'Green': (0, 128, 0),
        'Blue': (0, 0, 255),
        'Yellow': (255, 255, 0),
        'Cyan': (0, 255, 255),
        'Magenta': (255, 0, 255),
        'White': (255, 255, 255),
        'Black': (0, 0, 0),
        'Orange': (255, 165, 0),
        'Gold': (255, 215, 0),
    }
    return colors.get(color_name, (255, 255, 255))

def convert_scene_function(cpp_code, function_name, scene_id, description, theme):
    """Convert a C++ scene function to JSON"""
    
    # Extract function body
    pattern = rf'void\s+Scenes::{function_name}\s*\([^)]*\)\s*\{{(.*?)\n\}}'
    match = re.search(pattern, cpp_code, re.DOTALL)
    
    if not match:
        print(f"Function {function_name} not found")
        return None
    
    function_body = match.group(1)
    
    # Find all setPixel calls
    pixels = []
    for line in function_body.split('\n'):
        if 'setPixel' in line:
            pixel_data = parse_setpixel_call(line)
            if pixel_data:
                x, y, (r, g, b) = pixel_data
                pixels.append({
                    "x": x,
                    "y": y,
                    "r": r,
                    "g": g,
                    "b": b
                })
    
    # Build JSON structure
    scene_json = {
        "sceneId": scene_id,
        "version": "15.1.1",
        "created": "2026-01-04",
        "description": description,
        "theme": theme,
        "centerX": 10,
        "centerY": 12,
        "pixels": pixels,
        "metadata": {
            "pixelCount": len(pixels),
            "createdBy": "Scene Converter V15.1.1",
            "originalFunction": function_name
        }
    }
    
    return scene_json

def main():
    if len(sys.argv) < 2:
        print("Usage: python scene_converter.py Scenes.cpp")
        sys.exit(1)
    
    with open(sys.argv[1], 'r') as f:
        cpp_code = f.read()
    
    # Scene definitions
    scenes = [
        ("drawGift", "gift", "Green gift box with red ribbon", "christmas"),
        ("drawYellowGift", "yellow_gift", "Gold gift with red ribbons", "christmas"),
        ("drawFrostyFace", "frosty_snowman", "Snowman with blue hat", "christmas"),
        ("drawGiantSnowflake", "giant_snowflake", "Cyan snowflake", "christmas"),
        ("drawPumpkin", "halloween_pumpkin", "Jack-o-lantern", "halloween"),
        ("drawBat", "halloween_bat", "Flying bat", "halloween"),
        ("drawGhost", "halloween_ghost", "Sheet ghost", "halloween"),
        ("drawTurkey", "thanksgiving_turkey", "Turkey", "thanksgiving"),
        ("drawFirework", "newyear_firework", "Firework burst", "newyear"),
    ]
    
    for func_name, scene_id, desc, theme in scenes:
        json_data = convert_scene_function(cpp_code, func_name, scene_id, desc, theme)
        if json_data:
            filename = f"{scene_id}.json"
            with open(filename, 'w') as f:
                json.dump(json_data, f, indent=2)
            print(f"Created {filename} ({len(json_data['pixels'])} pixels)")

if __name__ == '__main__':
    main()
