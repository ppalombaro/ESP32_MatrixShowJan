# Spooky Tracking Eyeballs Animation - Complete Example
## V15.0.3.1 - 2025-01-03
### Halloween Theme: Bloodshot Eyes with Pupil Tracking

---

## 🎯 PROJECT OVERVIEW

**Animation Concept**: Two large spooky eyeballs (one per matrix) that look in different directions as if tracking viewers. Red bloodshot eyes with black pupils.

**Technical Approach**: 
- **Static Scenes**: 9 eye positions (center, 8 directions) × 2 eyes = 18 scenes total
- **Scene-Based Animation**: Frame timeline with different eye positions
- **Code Enhancement**: Add randomness to direction changes

**Why This Works**: 
- Each eye position is static (perfect for scene-based tool)
- Randomness added in firmware code (not in tool)
- Two matrices = two separate eyes for better effect

---

## 📐 DESIGN SPECIFICATIONS

### Matrix Layout:
```
Matrix 1 (Left Eye):  20 cols × 25 rows
Matrix 2 (Right Eye): 20 cols × 25 rows
```

### Eye Structure:
```
Outer Circle (Eyeball):
- Center: X=10, Y=12 (middle of each matrix)
- Radius: 8 pixels
- Color: Dark Red RGB(100, 0, 0) with white veins

Iris (Inner Circle):
- Radius: 5 pixels  
- Color: Bright Red RGB(255, 0, 0)

Pupil (Center Dot):
- Radius: 2 pixels
- Color: Black RGB(0, 0, 0)
- MOVES to create "looking" effect

Blood Vessels:
- Random red lines radiating from iris
- Color: Bright Red RGB(200, 0, 0)
```

### Pupil Positions (9 Directions):
```
        UP-LEFT     UP      UP-RIGHT
             ↖      ↑         ↗
              (8,8) (10,8) (12,8)

   LEFT ←  (8,12)  CENTER  (12,12)  → RIGHT
                   (10,12)

              (8,16) (10,16) (12,16)
             ↙      ↓         ↘
      DOWN-LEFT    DOWN    DOWN-RIGHT
```

---

## 🎨 STEP-BY-STEP: USING THE TOOL

### PHASE 1: Create Base Eye Template

#### Step 1: Open LED Content Studio

1. Open `LED_Content_Studio_v15.0.3.1_CORRECTED.html`
2. Scene Editor tab should be active
3. "Current Setup (2× 20x25 matrices)" should be selected
4. Click "🔨 Build Canvas from Layout"
5. You should see 40×25 grid (two matrices side-by-side)

---

#### Step 2: Create "Left Eye Looking Center" Scene

**This is our reference scene - we'll duplicate it for other positions**

1. Click "➕ New Scene"
2. Name: `left_eye_center`
3. Click OK
4. Set **Center X**: `10` (middle of left matrix)
5. Set **Center Y**: `12` (vertical center)

**Now draw the eye...**

---

#### Step 3: Draw the Eyeball (Outer Circle)

**We'll draw this manually, pixel by pixel**

**Set color to Dark Red**: Click color picker → Enter `#640000` (RGB 100,0,0)

**Draw outer circle at radius 8**:

```
Row by row (Y coordinate) - X positions to click:

Y=4:  X=9,10,11 (top of eyeball)
Y=5:  X=7,8,9,10,11,12,13
Y=6:  X=6,7,8,9,10,11,12,13,14
Y=7:  X=5,6,7,8,9,10,11,12,13,14,15
Y=8:  X=4,5,6,7,8,9,10,11,12,13,14,15,16
Y=9:  X=3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
Y=10: X=3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
Y=11: X=2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
Y=12: X=2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 (center row)
Y=13: X=2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
Y=14: X=3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
Y=15: X=3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
Y=16: X=4,5,6,7,8,9,10,11,12,13,14,15,16
Y=17: X=5,6,7,8,9,10,11,12,13,14,15
Y=18: X=6,7,8,9,10,11,12,13,14
Y=19: X=7,8,9,10,11,12,13
Y=20: X=9,10,11 (bottom of eyeball)
```

**Shortcut**: This is tedious! Skip to "Alternative: PNG Import Method" below if you prefer.

---

#### Step 4: Draw the Iris (Inner Red Circle)

**Set color to Bright Red**: `#FF0000` (RGB 255,0,0)

**Draw iris at radius 5, centered at (10, 12)**:

```
Y=7:  X=9,10,11
Y=8:  X=8,9,10,11,12
Y=9:  X=7,8,9,10,11,12,13
Y=10: X=7,8,9,10,11,12,13
Y=11: X=6,7,8,9,10,11,12,13,14
Y=12: X=6,7,8,9,10,11,12,13,14 (center)
Y=13: X=6,7,8,9,10,11,12,13,14
Y=14: X=7,8,9,10,11,12,13
Y=15: X=7,8,9,10,11,12,13
Y=16: X=8,9,10,11,12
Y=17: X=9,10,11
```

---

#### Step 5: Draw the Pupil (Black Center)

**Set color to Black**: `#000000`

**Pupil position for "looking center" is at (10, 12)**:

```
Y=10: X=9,10,11
Y=11: X=8,9,10,11,12
Y=12: X=8,9,10,11,12 (center)
Y=13: X=8,9,10,11,12
Y=14: X=9,10,11
```

---

#### Step 6: Add Blood Vessels (Veins)

**Set color to Blood Red**: `#C80000` (RGB 200,0,0)

**Draw random veins radiating from iris**:

```
Vein 1 (upper left):
Y=6: X=7
Y=7: X=6,7
Y=8: X=5,6

Vein 2 (upper right):
Y=6: X=13
Y=7: X=13,14
Y=8: X=14,15

Vein 3 (left):
Y=11: X=3,4
Y=12: X=3,4

Vein 4 (right):
Y=11: X=16,17
Y=12: X=16,17

Vein 5 (lower left):
Y=16: X=5,6
Y=17: X=6,7

Vein 6 (lower right):
Y=16: X=14,15
Y=17: X=13,14
```

---

#### Step 7: Export Base Scene

1. Click "📤 Export Current Scene"
2. Copy the JSON from output box
3. Save as `left_eye_center.json`

**Expected Output** (partial - showing structure):

```json
{
  "sceneId": "left_eye_center",
  "centerX": 10,
  "centerY": 12,
  "pixels": [
    {"x": -1, "y": -8, "r": 100, "g": 0, "b": 0},
    {"x": 0, "y": -8, "r": 100, "g": 0, "b": 0},
    {"x": 1, "y": -8, "r": 100, "g": 0, "b": 0},
    {"x": -3, "y": -7, "r": 100, "g": 0, "b": 0},
    ... (continues with all eyeball pixels)
    {"x": -1, "y": -5, "r": 255, "g": 0, "b": 0},
    {"x": 0, "y": -5, "r": 255, "g": 0, "b": 0},
    {"x": 1, "y": -5, "r": 255, "g": 0, "b": 0},
    ... (iris pixels)
    {"x": -1, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -2, "r": 0, "g": 0, "b": 0},
    ... (pupil pixels at center position)
    {"x": -3, "y": -6, "r": 200, "g": 0, "b": 0},
    {"x": -4, "y": -6, "r": 200, "g": 0, "b": 0}
    ... (blood vessel pixels)
  ]
}
```

---

### ALTERNATIVE: PNG Import Method (Much Faster!)

Instead of clicking every pixel, you can create the eye in an image editor:

1. Open Paint, Photoshop, GIMP, or any image editor
2. Create 20×25 pixel image
3. Draw circle with dark red fill
4. Draw inner red circle (iris)
5. Draw black center circle (pupil at center)
6. Add red lines for veins
7. Save as PNG
8. In LED Content Studio:
   - Click "🖼️ Import PNG Image"
   - Select your PNG file
   - Pixels automatically imported!
9. Fine-tune if needed
10. Export scene

**Much faster for complex scenes!**

---

### PHASE 2: Create All 9 Eye Positions

Now we need to create 8 more scenes with the pupil in different positions.

**The EASY way**: 
1. Export `left_eye_center.json`
2. Manually edit the JSON to move ONLY the pupil pixels
3. Import back into tool for each position

**The THOROUGH way**:
Create each scene individually by:
1. Importing base eye
2. Erasing old pupil
3. Drawing new pupil at offset position
4. Exporting

---

#### Scene 2: Left Eye Looking Up

1. New Scene: `left_eye_up`
2. Import or redraw base eye (eyeball + iris)
3. Draw pupil at position **(10, 8)** instead of (10, 12)
   - That's 4 pixels higher (Y - 4)

**Pupil pixels for "up" position**:
```
Y=6:  X=9,10,11
Y=7:  X=8,9,10,11,12
Y=8:  X=8,9,10,11,12 (center of pupil, 4 pixels up)
Y=9:  X=8,9,10,11,12
Y=10: X=9,10,11
```

Export as `left_eye_up.json`

---

#### Scene 3: Left Eye Looking Down

Pupil at **(10, 16)** - 4 pixels lower

```
Y=14: X=9,10,11
Y=15: X=8,9,10,11,12
Y=16: X=8,9,10,11,12 (center of pupil)
Y=17: X=8,9,10,11,12
Y=18: X=9,10,11
```

Export as `left_eye_down.json`

---

#### Scene 4: Left Eye Looking Left

Pupil at **(8, 12)** - 2 pixels left

```
Y=10: X=7,8,9
Y=11: X=6,7,8,9,10
Y=12: X=6,7,8,9,10 (center of pupil)
Y=13: X=6,7,8,9,10
Y=14: X=7,8,9
```

Export as `left_eye_left.json`

---

#### Scene 5: Left Eye Looking Right

Pupil at **(12, 12)** - 2 pixels right

```
Y=10: X=11,12,13
Y=11: X=10,11,12,13,14
Y=12: X=10,11,12,13,14 (center of pupil)
Y=13: X=10,11,12,13,14
Y=14: X=11,12,13
```

Export as `left_eye_right.json`

---

#### Scene 6-9: Diagonal Positions

**Up-Left**: Pupil at (8, 8)  
**Up-Right**: Pupil at (12, 8)  
**Down-Left**: Pupil at (8, 16)  
**Down-Right**: Pupil at (12, 16)

Follow same pattern, adjusting pupil position accordingly.

---

### PHASE 3: Create Right Eye Scenes

**Option 1**: Create 9 more scenes for right eye manually  
**Option 2**: Mirror the left eye scenes in code (recommended)

**We'll use Option 2** - the firmware will mirror left eye to right eye.

But for completeness, if you want separate right eye scenes:

1. Same process as left eye
2. Name them `right_eye_center`, `right_eye_up`, etc.
3. Center X = **30** (10 + 20 offset for second matrix)
4. Same Y positions
5. Export all 9 scenes

---

## 📋 ACTUAL OUTPUT FILES

### File 1: left_eye_center.json

```json
{
  "sceneId": "left_eye_center",
  "centerX": 10,
  "centerY": 12,
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
    {"x": -2, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": -2, "r": 0, "g": 0, "b": 0},
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
    {"x": -3, "y": -1, "r": 0, "g": 0, "b": 0},
    {"x": -2, "y": -1, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": -1, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -1, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -1, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": -1, "r": 0, "g": 0, "b": 0},
    {"x": 3, "y": -1, "r": 0, "g": 0, "b": 0},
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
    {"x": -3, "y": 0, "r": 0, "g": 0, "b": 0},
    {"x": -2, "y": 0, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": 0, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": 0, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": 0, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": 0, "r": 0, "g": 0, "b": 0},
    {"x": 3, "y": 0, "r": 0, "g": 0, "b": 0},
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
    {"x": -3, "y": 1, "r": 0, "g": 0, "b": 0},
    {"x": -2, "y": 1, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": 1, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": 1, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": 1, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": 1, "r": 0, "g": 0, "b": 0},
    {"x": 3, "y": 1, "r": 0, "g": 0, "b": 0},
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
    {"x": -2, "y": 2, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": 2, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": 2, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": 2, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": 2, "r": 0, "g": 0, "b": 0},
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
```

**That's 234 pixels for one complete eye!**

---

### File 2: left_eye_up.json

Similar structure, but pupil pixels at Y-4 offset:

```json
{
  "sceneId": "left_eye_up",
  "centerX": 10,
  "centerY": 12,
  "pixels": [
    ... (same eyeball and iris pixels) ...
    
    {"x": -1, "y": -6, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -6, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -6, "r": 0, "g": 0, "b": 0},
    {"x": -2, "y": -5, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": -5, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -5, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -5, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": -5, "r": 0, "g": 0, "b": 0},
    {"x": -2, "y": -4, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": -4, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -4, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -4, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": -4, "r": 0, "g": 0, "b": 0},
    {"x": -2, "y": -3, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": -3, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -3, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -3, "r": 0, "g": 0, "b": 0},
    {"x": 2, "y": -3, "r": 0, "g": 0, "b": 0},
    {"x": -1, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 0, "y": -2, "r": 0, "g": 0, "b": 0},
    {"x": 1, "y": -2, "r": 0, "g": 0, "b": 0}
  ]
}
```

---

## 🎬 ANIMATION BUILDER: Create the Timeline

### Step 1: Load Scenes into Animation Builder

1. Click **"Animation Builder"** tab
2. Click **"🔄 Refresh from Scene Editor"**
3. Should show all 9 scenes created

---

### Step 2: Build Timeline (Random Movement)

We'll create a pattern that looks random but is actually scripted:

**Frame Sequence**:
```
Frame 1:  center     → 1000ms (look at viewer)
Frame 2:  left       → 600ms  (glance left)
Frame 3:  center     → 800ms  (back to viewer)
Frame 4:  right      → 600ms  (glance right)
Frame 5:  center     → 500ms  (back to viewer)
Frame 6:  up         → 700ms  (look up)
Frame 7:  center     → 500ms  (back to viewer)
Frame 8:  down       → 900ms  (look down - creepy stare)
Frame 9:  up_left    → 400ms  (quick diagonal)
Frame 10: down_right → 800ms  (opposite diagonal)
Frame 11: center     → 1200ms (long stare at viewer)
Frame 12: left       → 300ms  (quick look)
Frame 13: right      → 300ms  (quick look other way)
Frame 14: center     → 1500ms (final stare)
```

**Total duration**: ~10 seconds per loop

---

### Step 3: Configure Matrix Assignment

**Option 1: Mirrored Eyes** (Both eyes same direction - spooky)
```json
{
  "1": "left_eye_center",
  "2": "left_eye_center"
}
```
Both matrices show same eye position (code will mirror for right eye)

**Option 2: Independent Eyes** (Eyes look different directions - creepier!)
```json
{
  "1": "left_eye_center",
  "2": "right_eye_up"
}
```
Left eye center, right eye looking up (lazy eye effect)

**We'll use Option 1 for simplicity**

---

### Step 4: Export Animation

1. Animation ID: `spooky_eyes_tracking`
2. Loop: ✅ CHECKED
3. Click **"💾 Export Animation JSON"**

---

### File 3: spooky_eyes_tracking.json

```json
{
  "animationId": "spooky_eyes_tracking",
  "loop": true,
  "frames": [
    {"scene": "left_eye_center", "durationMs": 1000},
    {"scene": "left_eye_left", "durationMs": 600},
    {"scene": "left_eye_center", "durationMs": 800},
    {"scene": "left_eye_right", "durationMs": 600},
    {"scene": "left_eye_center", "durationMs": 500},
    {"scene": "left_eye_up", "durationMs": 700},
    {"scene": "left_eye_center", "durationMs": 500},
    {"scene": "left_eye_down", "durationMs": 900},
    {"scene": "left_eye_up_left", "durationMs": 400},
    {"scene": "left_eye_down_right", "durationMs": 800},
    {"scene": "left_eye_center", "durationMs": 1200},
    {"scene": "left_eye_left", "durationMs": 300},
    {"scene": "left_eye_right", "durationMs": 300},
    {"scene": "left_eye_center", "durationMs": 1500}
  ],
  "assignments": {
    "1": "left_eye_center",
    "2": "left_eye_center"
  },
  "metadata": {
    "createdBy": "LED Content Studio v15.0.3.1",
    "createdDate": "2025-01-03T12:00:00.000Z",
    "totalDurationMs": 10100,
    "frameCount": 14
  }
}
```

---

## 💻 FIRMWARE IMPLEMENTATION

Now the tricky part - converting these scenes to actual firmware code.

### Option 1: Manual Scene Functions (Current Firmware)

Create scene functions in **HolidayAnimations.cpp**:

```cpp
// V15.0.3.1 - 2025-01-03 - Spooky Eyeball Scenes
// Generated from LED Content Studio tool

class EyeScenes {
private:
  MatrixDisplay* disp;
  
  // Draw complete eye with pupil at specified offset
  void drawEye(int matrix, int pupilOffsetX, int pupilOffsetY) {
    disp->clearMatrix(matrix);
    
    int centerX = COLS / 2;  // 10
    int centerY = ROWS / 2;  // 12
    
    // Eyeball outer (dark red)
    CRGB darkRed(100, 0, 0);
    
    // Draw circle radius 8 - eyeball
    // (Using circle drawing or manual pixels from JSON)
    for (int dy = -8; dy <= 8; dy++) {
      for (int dx = -8; dx <= 8; dx++) {
        if (dx*dx + dy*dy <= 64) {  // radius^2 = 8^2 = 64
          int x = centerX + dx;
          int y = centerY + dy;
          if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
            disp->setPixel(matrix, x, y, darkRed);
          }
        }
      }
    }
    
    // Iris (bright red)
    CRGB brightRed(255, 0, 0);
    for (int dy = -5; dy <= 5; dy++) {
      for (int dx = -5; dx <= 5; dx++) {
        if (dx*dx + dy*dy <= 25) {  // radius^2 = 5^2 = 25
          int x = centerX + dx;
          int y = centerY + dy;
          if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
            disp->setPixel(matrix, x, y, brightRed);
          }
        }
      }
    }
    
    // Pupil (black) - OFFSET from center
    int pupilX = centerX + pupilOffsetX;
    int pupilY = centerY + pupilOffsetY;
    
    for (int dy = -2; dy <= 2; dy++) {
      for (int dx = -2; dx <= 2; dx++) {
        if (dx*dx + dy*dy <= 4) {  // radius^2 = 2^2 = 4
          int x = pupilX + dx;
          int y = pupilY + dy;
          if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
            disp->setPixel(matrix, x, y, CRGB::Black);
          }
        }
      }
    }
    
    // Blood vessels (6 veins)
    CRGB bloodRed(200, 0, 0);
    
    // Vein 1: upper left
    disp->setPixel(matrix, centerX - 3, centerY - 6, bloodRed);
    disp->setPixel(matrix, centerX - 4, centerY - 6, bloodRed);
    disp->setPixel(matrix, centerX - 4, centerY - 5, bloodRed);
    disp->setPixel(matrix, centerX - 5, centerY - 4, bloodRed);
    
    // Vein 2: upper right
    disp->setPixel(matrix, centerX + 3, centerY - 6, bloodRed);
    disp->setPixel(matrix, centerX + 4, centerY - 6, bloodRed);
    disp->setPixel(matrix, centerX + 4, centerY - 5, bloodRed);
    disp->setPixel(matrix, centerX + 5, centerY - 4, bloodRed);
    
    // Vein 3: left side
    disp->setPixel(matrix, centerX - 7, centerY - 1, bloodRed);
    disp->setPixel(matrix, centerX - 6, centerY - 1, bloodRed);
    disp->setPixel(matrix, centerX - 7, centerY, bloodRed);
    disp->setPixel(matrix, centerX - 6, centerY, bloodRed);
    
    // Vein 4: right side
    disp->setPixel(matrix, centerX + 6, centerY - 1, bloodRed);
    disp->setPixel(matrix, centerX + 7, centerY - 1, bloodRed);
    disp->setPixel(matrix, centerX + 6, centerY, bloodRed);
    disp->setPixel(matrix, centerX + 7, centerY, bloodRed);
    
    // Vein 5: lower left
    disp->setPixel(matrix, centerX - 5, centerY + 4, bloodRed);
    disp->setPixel(matrix, centerX - 4, centerY + 5, bloodRed);
    
    // Vein 6: lower right
    disp->setPixel(matrix, centerX + 4, centerY + 5, bloodRed);
    disp->setPixel(matrix, centerX + 5, centerY + 4, bloodRed);
  }

public:
  EyeScenes(MatrixDisplay* display) : disp(display) {}
  
  // 9 eye direction scenes
  void drawEyeCenter(int matrix)     { drawEye(matrix, 0, 0); }
  void drawEyeUp(int matrix)         { drawEye(matrix, 0, -4); }
  void drawEyeDown(int matrix)       { drawEye(matrix, 0, 4); }
  void drawEyeLeft(int matrix)       { drawEye(matrix, -2, 0); }
  void drawEyeRight(int matrix)      { drawEye(matrix, 2, 0); }
  void drawEyeUpLeft(int matrix)     { drawEye(matrix, -2, -4); }
  void drawEyeUpRight(int matrix)    { drawEye(matrix, 2, -4); }
  void drawEyeDownLeft(int matrix)   { drawEye(matrix, -2, 4); }
  void drawEyeDownRight(int matrix)  { drawEye(matrix, 2, 4); }
};
```

---

### File 4: SpookyEyesAnimation.cpp

**Create animation with RANDOMNESS**:

```cpp
// V15.0.3.1 - 2025-01-03 - Spooky Tracking Eyes Animation
// Based on tool-generated timeline with added randomness

class SpookyEyesAnimation {
private:
  MatrixDisplay* disp;
  EyeScenes* eyes;
  
  unsigned long lastChange = 0;
  int currentDirection = 0;  // 0=center, 1=up, 2=down, 3=left, 4=right, etc.
  int frameIndex = 0;
  
  // V15.0.3.1 - 2025-01-03 - Base timeline from tool
  struct Frame {
    int direction;
    unsigned long duration;
  };
  
  const Frame baseTimeline[14] = {
    {0, 1000},  // center
    {3, 600},   // left
    {0, 800},   // center
    {4, 600},   // right
    {0, 500},   // center
    {1, 700},   // up
    {0, 500},   // center
    {2, 900},   // down
    {5, 400},   // up-left
    {8, 800},   // down-right
    {0, 1200},  // center
    {3, 300},   // left
    {4, 300},   // right
    {0, 1500}   // center
  };
  
  // V15.0.3.1 - 2025-01-03 - Add randomness to durations
  unsigned long getRandomDuration(unsigned long baseDuration) {
    // Vary by ±30%
    int variation = random(-30, 31);
    long adjusted = baseDuration + (baseDuration * variation / 100);
    return constrain(adjusted, 100, 5000);  // Keep reasonable bounds
  }
  
  void drawDirection(int matrix, int direction) {
    switch(direction) {
      case 0: eyes->drawEyeCenter(matrix); break;
      case 1: eyes->drawEyeUp(matrix); break;
      case 2: eyes->drawEyeDown(matrix); break;
      case 3: eyes->drawEyeLeft(matrix); break;
      case 4: eyes->drawEyeRight(matrix); break;
      case 5: eyes->drawEyeUpLeft(matrix); break;
      case 6: eyes->drawEyeUpRight(matrix); break;
      case 7: eyes->drawEyeDownLeft(matrix); break;
      case 8: eyes->drawEyeDownRight(matrix); break;
    }
  }

public:
  SpookyEyesAnimation(MatrixDisplay* display, EyeScenes* eyeScenes) 
    : disp(display), eyes(eyeScenes) {}
  
  void update() {
    unsigned long currentTime = millis();
    
    // Get current frame info
    Frame currentFrame = baseTimeline[frameIndex];
    
    // V15.0.3.1 - 2025-01-03 - Add randomness to duration
    unsigned long frameDuration = getRandomDuration(currentFrame.duration);
    
    // Check if time to change
    if (currentTime - lastChange >= frameDuration) {
      // V15.0.3.1 - 2025-01-03 - Occasional random direction injection
      int direction = currentFrame.direction;
      if (random(100) < 10) {  // 10% chance of random look
        direction = random(0, 9);  // Any of 9 directions
      }
      
      // Draw on both matrices (mirrored)
      drawDirection(0, direction);  // Left eye
      drawDirection(1, direction);  // Right eye (same direction for now)
      
      // Move to next frame
      frameIndex++;
      if (frameIndex >= 14) {
        frameIndex = 0;  // Loop
      }
      
      lastChange = currentTime;
      disp->show();
    }
  }
};
```

---

### File 5: Integration into HolidayAnimations.cpp

Add to **HolidayAnimations.h**:

```cpp
// V15.0.3.1 - 2025-01-03 - Spooky eyeballs
void spookyTrackingEyes();
```

Add to **HolidayAnimations.cpp**:

```cpp
// V15.0.3.1 - 2025-01-03 - Include eye scenes
#include "SpookyEyesAnimation.cpp"

EyeScenes* eyeScenes = nullptr;
SpookyEyesAnimation* eyeAnim = nullptr;

// In constructor or init:
eyeScenes = new EyeScenes(disp);
eyeAnim = new SpookyEyesAnimation(disp, eyeScenes);

// V15.0.3.1 - 2025-01-03 - Spooky tracking eyes animation
void HolidayAnimations::spookyTrackingEyes() {
  eyeAnim->update();
}
```

---

### Add to ThemeManager

In **ThemeManager.cpp**, add to Halloween animated theme:

```cpp
case THEME_HALLOWEEN_ANIMATED:
  switch (currentAnimationIndex) {
    case 0: holidayAnims->pumpkinPulse(); break;
    case 1: holidayAnims->ghostFloat(); break;
    case 2: holidayAnims->spookyTrackingEyes(); break;  // NEW!
    // ... rest of animations
  }
  break;
```

---

## 🎯 FINAL RESULT

### What You Get:

**Visual Effect**:
- Two large bloodshot red eyes
- Black pupils that move to "track" viewers
- Creepy blood vessel details
- Smooth transitions between directions
- Random timing variations (not robotic)
- 10% chance of unexpected looks

**Technical Achievement**:
- Created 9 static eye position scenes in visual tool
- Built frame-based animation timeline
- Exported clean JSON scene data
- Implemented in firmware with randomness overlay
- Scene-based foundation + procedural enhancement

---

## 📊 FILE SUMMARY

**JSON Files Created** (for future SPIFFS use):
1. `left_eye_center.json` (234 pixels)
2. `left_eye_up.json`
3. `left_eye_down.json`
4. `left_eye_left.json`
5. `left_eye_right.json`
6. `left_eye_up_left.json`
7. `left_eye_up_right.json`
8. `left_eye_down_left.json`
9. `left_eye_down_right.json`
10. `spooky_eyes_tracking.json` (animation timeline)

**Firmware Files Created**:
1. `EyeScenes.cpp` - Eye drawing functions
2. `SpookyEyesAnimation.cpp` - Animation controller
3. Updates to `HolidayAnimations.cpp/h`
4. Updates to `ThemeManager.cpp`

---

## 🎓 KEY LESSONS

### 1. Tool Created the Foundation
- Visual scene editor made it easy to design eyes
- JSON export provides clean data format
- Animation builder created the movement timeline

### 2. Code Added the Intelligence
- Randomness in timing (makes it less robotic)
- Random direction injection (surprise factor)
- Smooth integration with existing system

### 3. Hybrid Approach Works Best
- Static elements (eye design) = Perfect for tool
- Dynamic behavior (randomness) = Perfect for code
- Combined = Professional result

### 4. Reusability
- Eye drawing function is parameterized
- Change pupil offset = different direction
- One function, 9 scenes

---

## 🚀 NEXT STEPS

### Immediate:
1. Create first eye scene in tool (30 min)
2. Test on hardware (5 min)
3. Adjust colors/size if needed (10 min)
4. Create remaining 8 positions (1 hour)

### Soon:
5. Implement firmware code (30 min)
6. Add randomness variations (15 min)
7. Fine-tune timing (15 min)
8. Test complete animation (ongoing)

### Future Enhancements:
- Add blinking (eyelid closing scene)
- Bloodshot veins pulse brighter occasionally
- Eyes dilate (pupil size changes)
- Different expressions (angry, surprised)
- Independent eye movement (lazy eye effect)

---

## 💡 BONUS IDEAS

### Enhancement 1: Eye Blinking

Add 3 more scenes:
- `eye_closing` (eyelid halfway)
- `eye_closed` (fully closed)
- `eye_opening` (eyelid halfway)

Insert into timeline randomly:
```cpp
if (random(100) < 5) {  // 5% blink chance
  // Blink sequence
  eyes->drawEyeClosing(matrix);
  delay(50);
  eyes->drawEyeClosed(matrix);
  delay(100);
  eyes->drawEyeOpening(matrix);
  delay(50);
}
```

### Enhancement 2: Pupil Dilation

When looking at viewer (center), dilate pupils:
```cpp
void drawEyeCenter(int matrix, bool dilated) {
  // ... draw eyeball and iris ...
  
  int pupilRadius = dilated ? 3 : 2;  // Bigger when dilated
  
  // Draw pupil with variable size
  for (int dy = -pupilRadius; dy <= pupilRadius; dy++) {
    // ... draw circle ...
  }
}
```

### Enhancement 3: Glowing Effect

Pulse the bloodshot veins:
```cpp
// In animation loop
uint8_t veinBrightness = beatsin8(60);  // Pulse over 60 BPM
CRGB bloodRed = CRGB(veinBrightness, 0, 0);
```

---

**Congratulations!** You've created a complex, creepy, professional Halloween animation using the tool + code hybrid approach! 🎃👁️👁️

