# LED Content Studio V16.4 - Required Updates
**V16.4.8-2026-01-11T22:40:00Z - ContentManager Compatibility**

## Critical Missing Fields

Your current LED Content Studio V16.2 is **INCOMPATIBLE** with ContentManager V16.4. The ContentManager expects fields that the Studio doesn't generate.

---

## Required Changes:

### 1. Scene JSON Format (Static Frames)

**Current V16.2 Output:**
```json
{
  "sceneId": "christmas_tree",
  "version": "16.2",
  "theme": "christmas",
  "description": "Christmas Tree",
  "centerX": 10,
  "centerY": 12,
  "pixels": [...],
  "metadata": { "pixelCount": 150 }
}
```

**Required V16.4 Output:**
```json
{
  "sceneId": "christmas_tree",
  "version": "16.4",
  "theme": "christmas",
  "description": "Christmas Tree",
  "centerX": 10,
  "centerY": 12,
  "durationMs": 5000,
  "pixels": [...],
  "metadata": { "pixelCount": 150 }
}
```

**SIMPLE CHANGE:** Just add ONE optional field: `durationMs`
- If missing → uses project default (5000ms)
- Scenes are ALWAYS mirrored on all matrices automatically

**Add to normalizeScene() function (line 376):**
```javascript
function normalizeScene(data) {
    return {
        "sceneId": data.sceneId || "untitled",
        "version": "16.4",
        "theme": data.theme || "christmas",
        "description": data.description || data.displayName || data.sceneId || "Scene",
        "centerX": parseInt(data.centerX) || 10,
        "centerY": parseInt(data.centerY) || 12,
        "durationMs": parseInt(data.durationMs) || 5000,  // V16.4.9 - NEW: Optional display duration
        "pixels": data.pixels || [],
        "metadata": {
            "pixelCount": (data.pixels || []).length
        }
    };
}
```

---

### 2. Animation Timeline JSON Format

**Current V16.2 Output:**
```json
{
  "displayName": "Spooky Eyes",
  "version": "16.2",
  "theme": "halloween",
  "loop": true,
  "frames": [
    { "sceneId": "eyes_01", "durationMs": 100 },
    { "sceneId": "eyes_02", "durationMs": 100 }
  ],
  "metadata": { "frameCount": 2 }
}
```

**Required V16.4 Output (Option A - Mirrored on all matrices):**
```json
{
  "displayName": "Spooky Eyes",
  "version": "16.4",
  "theme": "halloween",
  "loop": true,
  "durationMs": 8000,
  "scene": "animations/halloween/spooky_eyes/animation_timeline.json",
  "frames": [
    { "sceneId": "eyes_01", "durationMs": 100 },
    { "sceneId": "eyes_02", "durationMs": 100 }
  ],
  "metadata": { "frameCount": 2 }
}
```

**Required V16.4 Output (Option B - Per-matrix control):**
```json
{
  "displayName": "Spooky Eyes",
  "version": "16.4",
  "theme": "halloween",
  "loop": true,
  "durationMs": 8000,
  "matrix0Scene": "animations/halloween/spooky_eyes/animation_timeline.json",
  "matrix1Scene": "animations/halloween/spooky_eyes/animation_timeline.json",
  "matrix2Scene": "",
  "frames": [
    { "sceneId": "eyes_01", "durationMs": 100 },
    { "sceneId": "eyes_02", "durationMs": 100 }
  ],
  "metadata": { "frameCount": 2 }
}
```

**RULES:**
1. `durationMs` is optional (defaults to sum of all frame durations OR 8000ms)
2. Use **EITHER**:
   - `scene` (mirrored on all active matrices) **OR**
   - `matrix0Scene`, `matrix1Scene`, `matrix2Scene` (per-matrix control)
3. If using matrix0/1/2 and a matrix is missing → mirrors matrix0Scene
4. Backward compatible: missing fields use defaults

**Update exportAnimation() function (line 567):**
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
        
        // V16.4.9 - Calculate total duration from all frames if not provided
        const totalDuration = data.frames.reduce((sum, frame) => sum + (frame.durationMs || 100), 0);
        const timelinePath = `animations/${data.theme || 'christmas'}/${data.displayName.toLowerCase().replace(/\s+/g, '_')}/animation_timeline.json`;
        
        const normalized = {
            "displayName": data.displayName,
            "version": "16.4",
            "theme": data.theme || "christmas",
            "loop": data.loop !== false,
            "durationMs": parseInt(data.durationMs) || totalDuration || 8000,  // V16.4.9 - Optional
            "frames": data.frames,
            "metadata": {
                "frameCount": data.frames.length
            }
        };
        
        // V16.4.9 - Use EITHER scene (mirror) OR matrix0/1/2 (per-matrix)
        if (data.matrix0Scene || data.matrix1Scene || data.matrix2Scene) {
            // Per-matrix control
            normalized.matrix0Scene = data.matrix0Scene || timelinePath;
            normalized.matrix1Scene = data.matrix1Scene || normalized.matrix0Scene;  // Mirror if missing
            normalized.matrix2Scene = data.matrix2Scene || "";
        } else {
            // Mirror on all matrices (default)
            normalized.scene = data.scene || timelinePath;
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

### 3. Add UI Controls for New Fields

**In Scene Editor section, add after grid controls (SIMPLE - just duration):**
```html
<div class="section" style="margin-top: 15px;">
  <span class="req-label">Scene Properties (V16.4)</span>
  <div class="form-row">
    <label>Display Duration (ms):</label>
    <input type="number" id="sceneDuration" value="5000" min="100" step="100">
    <span style="color: var(--text-dim); font-size: 12px;">Optional - defaults to 5000ms if empty</span>
  </div>
</div>
```

**In Animation Builder section, add before timeline JSON:**
```html
<div class="section" style="margin-top: 15px;">
  <span class="req-label">Animation Properties (V16.4)</span>
  <div class="form-row">
    <label>Total Duration (ms):</label>
    <input type="number" id="animDuration" placeholder="Auto-calculated from frames" min="100" step="100">
    <span style="color: var(--text-dim); font-size: 12px;">Optional - leave empty to auto-calculate</span>
  </div>
  
  <div style="margin-top: 15px; padding: 10px; background: #0a0618; border-radius: 4px;">
    <div style="color: var(--neon-glow); font-weight: bold; margin-bottom: 10px;">Matrix Assignment (Optional)</div>
    <div style="display: flex; gap: 10px; margin-bottom: 10px;">
      <label style="display: flex; align-items: center; gap: 5px;">
        <input type="radio" name="matrixMode" value="mirror" checked onchange="toggleMatrixMode()">
        <span>Mirror on all matrices</span>
      </label>
      <label style="display: flex; align-items: center; gap: 5px;">
        <input type="radio" name="matrixMode" value="individual" onchange="toggleMatrixMode()">
        <span>Per-matrix control</span>
      </label>
    </div>
    
    <div id="mirrorMode">
      <div class="form-row">
        <label>Scene path:</label>
        <input type="text" id="animScene" placeholder="Auto-generated" style="width: 400px;">
      </div>
    </div>
    
    <div id="individualMode" style="display: none;">
      <div class="form-row">
        <label>Matrix 0 (Right):</label>
        <input type="text" id="animMatrix0" placeholder="Auto-generated" style="width: 400px;">
      </div>
      <div class="form-row">
        <label>Matrix 1 (Left):</label>
        <input type="text" id="animMatrix1" placeholder="Mirrors Matrix 0 if empty" style="width: 400px;">
      </div>
      <div class="form-row">
        <label>Matrix 2 (Mega):</label>
        <input type="text" id="animMatrix2" placeholder="Optional" style="width: 400px;">
      </div>
    </div>
  </div>
</div>

<script>
function toggleMatrixMode() {
  const mirror = document.querySelector('input[name="matrixMode"][value="mirror"]').checked;
  document.getElementById('mirrorMode').style.display = mirror ? 'block' : 'none';
  document.getElementById('individualMode').style.display = mirror ? 'none' : 'block';
}
</script>
```

---

### 4. Update syncToText() to Read UI Values

**Replace syncToText() function (SIMPLE - just reads duration):**
```javascript
function syncToText() {
    if (!currentScene) return;
    const s = scenes[currentScene];
    
    // V16.4.9 - Only read optional duration field
    const duration = parseInt(document.getElementById('sceneDuration').value);
    if (duration && duration > 0) {
        s.durationMs = duration;
    }
    
    const normalized = normalizeScene(s);
    scenes[currentScene] = normalized;
    document.getElementById('jsonEditor').value = JSON.stringify(normalized, null, 2);
}
```

---

## Summary of Changes

**SCENES (Simple):**
1. ✅ Add ONE optional field: `durationMs` (defaults to 5000ms if missing)
2. ✅ Scenes are ALWAYS mirrored on all active matrices
3. ✅ Update version from "16.2" to "16.4"

**ANIMATIONS (Flexible):**
1. ✅ Add ONE optional field: `durationMs` (defaults to sum of frames OR 8000ms)
2. ✅ Support TWO modes:
   - **Mirror mode**: Use `scene` field (mirrored on all matrices)
   - **Per-matrix mode**: Use `matrix0Scene`, `matrix1Scene`, `matrix2Scene`
3. ✅ If matrix1/2 missing in per-matrix mode → mirrors matrix0Scene
4. ✅ Update version from "16.2" to "16.4"

**BACKWARD COMPATIBILITY:**
- ✅ Old V16.2 files work (missing fields use defaults)
- ✅ No breaking changes

---

## Testing Checklist

After making changes:

1. **Export a scene** - verify JSON has all required fields
2. **Export an animation timeline** - verify JSON has all required fields
3. **Import folder** - verify it still loads properly
4. **Run through json_datain_2_FFAT.bat** - verify it minifies correctly
5. **Upload to ESP32** - verify ContentManager discovers the content
6. **Check web logs** - look for "Discovered X themes" message

---

## Backward Compatibility Note

Old V16.2 JSON files **will still work** because ContentManager uses defaults:
- Missing `durationMs` → defaults to 5000ms
- Missing `matrix0Scene` → uses file path
- Missing `matrix1Scene` → mirrors matrix0Scene
- Missing `matrix2Scene` → empty string

But **new** content should use V16.4 format for full control.
