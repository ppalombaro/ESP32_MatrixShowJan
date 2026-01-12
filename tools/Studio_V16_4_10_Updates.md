# LED Content Studio V16.4.10 - Required Updates
**V16.4.10-2026-01-12T03:15:00Z - Final compatibility fixes**

## Summary of What Was Fixed in ContentManager:

✅ **Issue 1:** Scenes backward compatible - CONFIRMED OK
✅ **Issue 2:** Timeline duration now calculated from sum of frames - FIXED
✅ **Issue 3:** Relative scene paths now supported - FIXED

## Studio Updates Needed:

### 1. Update normalizeScene() - Add optional durationMs

**Find this function (around line 376):**
```javascript
function normalizeScene(data) {
    return {
        "sceneId": data.sceneId || "untitled",
        "version": "16.2",
        "theme": data.theme || "christmas",
        "description": data.description || data.displayName || data.sceneId || "Scene",
        "centerX": parseInt(data.centerX) || 10,
        "centerY": parseInt(data.centerY) || 12,
        "pixels": data.pixels || [],
        "metadata": {
            "pixelCount": (data.pixels || []).length
        }
    };
}
```

**Replace with:**
```javascript
function normalizeScene(data) {
    const result = {
        "sceneId": data.sceneId || "untitled",
        "version": "16.4",  // V16.4.10 - Updated version
        "theme": data.theme || "christmas",
        "description": data.description || data.displayName || data.sceneId || "Scene",
        "centerX": parseInt(data.centerX) || 10,
        "centerY": parseInt(data.centerY) || 12,
        "pixels": data.pixels || [],
        "metadata": {
            "pixelCount": (data.pixels || []).length
        }
    };
    
    // V16.4.10 - Add optional durationMs if provided
    if (data.durationMs && data.durationMs > 0) {
        result.durationMs = parseInt(data.durationMs);
    }
    
    return result;
}
```

---

### 2. Update exportAnimation() - Add optional durationMs

**Find this function (around line 567):**
```javascript
function exportAnimation() {
    try {
        const data = JSON.parse(document.getElementById('animJsonEditor').value);
        if (!data.displayName) {
            alert("Missing required field: displayName");
            return;
        }
        if (!data.frames || !Array.isArray(data.frames)) {
            alert("Missing or invalid frames array");
            return;
        }
        
        const normalized = {
            "displayName": data.displayName,
            "version": "16.2",
            "theme": data.theme || "christmas",
            "loop": data.loop !== false,
            "frames": data.frames,
            "metadata": {
                "frameCount": data.frames.length
            }
        };
        
        const blob = new Blob([JSON.stringify(normalized)], {type: 'application/json'});
        const a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = `animation_timeline.json`;
        a.click();
    } catch (e) { 
        alert("Invalid JSON: " + e.message); 
    }
}
```

**Replace with:**
```javascript
function exportAnimation() {
    try {
        const data = JSON.parse(document.getElementById('animJsonEditor').value);
        if (!data.displayName) {
            alert("Missing required field: displayName");
            return;
        }
        if (!data.frames || !Array.isArray(data.frames)) {
            alert("Missing or invalid frames array");
            return;
        }
        
        const normalized = {
            "displayName": data.displayName,
            "version": "16.4",  // V16.4.10 - Updated version
            "theme": data.theme || "christmas",
            "loop": data.loop !== false,
            "frames": data.frames,
            "metadata": {
                "frameCount": data.frames.length
            }
        };
        
        // V16.4.10 - Add optional durationMs if provided
        // ContentManager will auto-calculate from frames if missing
        if (data.durationMs && data.durationMs > 0) {
            normalized.durationMs = parseInt(data.durationMs);
        }
        
        const blob = new Blob([JSON.stringify(normalized)], {type: 'application/json'});
        const a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = `animation_timeline.json`;
        a.click();
    } catch (e) { 
        alert("Invalid JSON: " + e.message); 
    }
}
```

---

### 3. Optional UI Enhancement - Add Duration Input

**You can optionally add a duration input field to the Scene Editor section:**

Find the scene editor controls and add after the grid:
```html
<div class="section" style="margin-top: 10px;">
  <label class="req-label">Display Duration (Optional)</label>
  <div style="display: flex; gap: 10px; align-items: center;">
    <input type="number" id="sceneDuration" placeholder="Auto (5000ms)" min="100" step="100" style="width: 150px;">
    <span style="color: var(--text-dim); font-size: 12px;">Leave empty for default (5 seconds)</span>
  </div>
</div>
```

**And add for Animation Builder:**
```html
<div class="section" style="margin-top: 10px;">
  <label class="req-label">Total Duration (Optional)</label>
  <div style="display: flex; gap: 10px; align-items: center;">
    <input type="number" id="animDuration" placeholder="Auto-calculated from frames" min="100" step="100" style="width: 200px;">
    <span style="color: var(--text-dim); font-size: 12px;">Leave empty to auto-calculate</span>
  </div>
</div>
```

**Then update syncToText() to read the duration field:**
```javascript
function syncToText() {
    if (!currentScene) return;
    const s = scenes[currentScene];
    
    // V16.4.10 - Read optional duration
    const durationInput = document.getElementById('sceneDuration');
    if (durationInput) {
        const duration = parseInt(durationInput.value);
        if (duration && duration > 0) {
            s.durationMs = duration;
        }
    }
    
    const normalized = normalizeScene(s);
    scenes[currentScene] = normalized;
    document.getElementById('jsonEditor').value = JSON.stringify(normalized, null, 2);
}
```

---

## How Relative Paths Work Now:

Your timeline JSON:
```json
{
  "displayName": "spooky_eyes",
  "frames": [
    { "scene": "left_eye_center", "durationMs": 1000 },
    { "scene": "left_eye_left", "durationMs": 600 }
  ]
}
```

**ContentManager will resolve:**
- Timeline path: `animations/halloween/spooky_eyes/eyes_timeline.json`
- Frame scene: `"left_eye_center"`
- **Resolved to:** `animations/halloween/spooky_eyes/left_eye_center.json`

**Rules:**
1. If frame scene contains `/` → treated as absolute path
2. If frame scene is just a name → prepends timeline's directory
3. Auto-adds `.json` extension if missing

---

## Testing Checklist:

After updating Studio:

1. ✅ Create a new scene → verify JSON has `version: "16.4"`
2. ✅ Add duration field → verify it appears in exported JSON
3. ✅ Leave duration empty → verify it's omitted (backward compatible)
4. ✅ Export timeline → verify JSON has `version: "16.4"`
5. ✅ Timeline with frames → verify ContentManager calculates total duration
6. ✅ Upload to ESP32 → verify animations play with correct frame timing

---

## What Changed in ContentManager V16.4.10:

**File:** ContentManager.cpp

**Line 189-210:** Timeline duration calculation
- Now sums all frame durations if `durationMs` missing
- Falls back to 5000ms only if frames array empty

**Line 342-391:** Animation rendering
- Now reads timeline JSON
- Iterates through frames array
- Uses `resolveScenePath()` to handle relative scene names
- Renders each frame with correct duration

**Line 292-319:** Helper function `resolveScenePath()`
- Takes timeline path and scene name
- Returns full path to scene file
- Handles both relative names ("left_eye_center") and absolute paths

---

## Example Working Structure:

```
data_in/
  animations/
    halloween/
      spooky_eyes/
        eyes_timeline.json         ← references "left_eye_center"
        left_eye_center.json       ← rendered by ContentManager
        left_eye_left.json
        left_eye_right.json
        ...
```

Timeline JSON:
```json
{
  "displayName": "Spooky Eyes",
  "version": "16.4",
  "theme": "halloween",
  "loop": true,
  "frames": [
    { "scene": "left_eye_center", "durationMs": 1000 },
    { "scene": "left_eye_left", "durationMs": 600 }
  ]
}
```

**ContentManager calculates:** Total duration = 1000 + 600 = 1600ms
**ContentManager resolves:** "left_eye_center" → "animations/halloween/spooky_eyes/left_eye_center.json"

---

## Summary:

✅ **Scenes:** Backward compatible, optional `durationMs` field (defaults 5000ms)
✅ **Timelines:** Auto-calculate duration from frames, optional override
✅ **Relative paths:** Scene names resolved relative to timeline folder
✅ **Studio changes:** Minimal - just update 2 functions and version number

All three systems (ContentManager, JSON files, Studio) are now in sync!
