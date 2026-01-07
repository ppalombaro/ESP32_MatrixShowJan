/* Scenes.cpp
   Legacy placeholder - all scenes now loaded from FFat
   VERSION: V15.3.0-2026-01-04T16:00:00Z - All scenes migrated to FFat
   
   V15.3.0-2026-01-04T16:00:00Z - Removed all hardcoded scene implementations
   All themes now use JSON files in data/ folder organized by theme
*/

#include "Scenes.h"

Scenes::Scenes(MatrixDisplay* display) : disp(display) {}

// V15.3.0-2026-01-04T16:00:00Z - All scene implementations removed
// Scenes are now loaded from FFat via SceneData class
// See data/christmas/, data/halloween/, data/thanksgiving/, data/newyear/, data/osu/
