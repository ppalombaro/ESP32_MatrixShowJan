# Auto-Scaling Scene System - V15.2.3
## Matrix-Aware Rendering with Optional Scaling

### OVERVIEW
Your JSON scene files now automatically adapt to different matrix sizes without any modifications. The system detects which matrix is being used and adjusts rendering accordingly.

### HOW IT WORKS

**Without "scaleForLargeMatrix"** (default behavior):
- Scene renders at original size
- Auto-centers on any matrix size
- Small scenes stay small on large matrices
- Perfect for detail work, text, small icons

**With "scaleForLargeMatrix": true**:
- Scene auto-scales proportionally for larger matrices
- 25x20 → 40x50 matrix = 2x scaling (40/25 = 1.6x columns, rounds to 2x)
- Large scenes fill the display appropriately
- Perfect for full-screen scenes, backgrounds

### JSON FORMAT

**Standard Scene (no scaling):**
```json
{
  "sceneId": "small_icon",
  "centerX": 10,
  "centerY": 12,
  "pixels": [
    {"x": 0, "y": 0, "r": 255, "g": 0, "b": 0}
  ]
}
```
Result: Same size on all matrices, auto-centered

**Auto-Scaling Scene:**
```json
{
  "sceneId": "christmas_tree",
  "centerX": 10,
  "centerY": 12,
  "scaleForLargeMatrix": true,
  "pixels": [
    {"x": 0, "y": 0, "r": 255, "g": 0, "b": 0}
  ]
}
```
Result: 
- Matrix 0 (25x20): Original size
- Matrix 1 (25x20): Original size
- Matrix 2 (40x50): Scaled 2x larger

### AUTOMATIC FEATURES

1. **Auto-Centering**: 
   - centerX/centerY can be omitted
   - System uses matrix center automatically
   - Or specify explicit center for fine control

2. **Bounds Checking**:
   - Uses actual matrix dimensions
   - Prevents overflow on any size matrix
   - Clips pixels outside bounds safely

3. **Scale Calculation**:
   - Base size assumed: 25 columns
   - Scale = matrixCols / 25.0
   - Examples:
     - 25 cols → 1.0x (no scaling)
     - 40 cols → 1.6x scaling
     - 50 cols → 2.0x scaling

### EXISTING FILES - NO CHANGES NEEDED

All your existing JSON files work perfectly as-is:
- christmas_tree.json → Works on all matrices (no scaling)
- santa_claus.json → Works on all matrices (no scaling)
- gift.json → Works on all matrices (no scaling)

To enable scaling, just add one line:
```json
"scaleForLargeMatrix": true,
```

### WHEN TO USE SCALING

**Use scaleForLargeMatrix: true for:**
- Full-screen scenes (trees, characters, backgrounds)
- Scenes designed to fill the matrix
- Holiday displays meant to be prominent
- Anything that should look bigger on larger displays

**DON'T use scaling (omit or set false) for:**
- Small decorative elements
- Text/numbers that need precise sizing
- Icons or symbols
- Fine detail work
- Multi-scene compositions where relative size matters

### LED CONTENT STUDIO

No changes needed to LED Content Studio!
- Design at 25x20 as always
- Export JSON as usual
- Add "scaleForLargeMatrix": true manually if desired
- Or leave it off for fixed-size rendering

### TECHNICAL DETAILS

**Matrix Dimensions Detection:**
```cpp
int matrixRows = disp->getMatrixRows(matrix);  // Returns actual rows
int matrixCols = disp->getMatrixCols(matrix);  // Returns actual cols
```

**Scale Calculation:**
```cpp
if (scaleForLargeMatrix && matrixCols > 25) {
    scale = (float)matrixCols / 25.0;
}
```

**Coordinate Transformation:**
```cpp
scaledX = originalX * scale;
scaledY = originalY * scale;
absX = centerX + scaledX;
absY = centerY + scaledY;
```

**Bounds Checking:**
```cpp
if (absX >= 0 && absX < matrixCols && absY >= 0 && absY < matrixRows) {
    // Render pixel
}
```

### EXAMPLES

**Example 1: Christmas Tree (scaled)**
```json
{
  "sceneId": "christmas_tree",
  "scaleForLargeMatrix": true,
  "pixels": [...]
}
```
- 25x20 matrix: 15 pixels tall tree
- 40x50 matrix: 30 pixels tall tree (2x)

**Example 2: Small Star Icon (not scaled)**
```json
{
  "sceneId": "star_icon",
  "pixels": [...]
}
```
- 25x20 matrix: 5x5 pixel star, centered
- 40x50 matrix: Same 5x5 pixel star, centered

**Example 3: Manual Center Override**
```json
{
  "sceneId": "corner_decoration",
  "centerX": 5,
  "centerY": 5,
  "pixels": [...]
}
```
- Places scene near top-left on any matrix
- Useful for multi-element compositions

### MIGRATION GUIDE

**For existing scenes that should scale:**
1. Open JSON file
2. Add `"scaleForLargeMatrix": true,` after description
3. Done!

**For existing scenes that should NOT scale:**
- No changes needed
- Already works perfectly

**For new scenes:**
- Design at 25x20 in LED Content Studio
- Export JSON
- Decide if you want scaling
- Add scaleForLargeMatrix flag if yes
- Upload to SPIFFS

### TESTING

To test both modes:
1. Load scene on Matrix 0 or 1 (25x20) - verify looks good
2. If you have Matrix 2 (megamatrix):
   - Without scaling: Scene centered, original size
   - With scaling: Scene centered, scaled proportionally
3. Adjust scaleForLargeMatrix as desired

V15.2.3-2026-01-04T15:00:00Z
