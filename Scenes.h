/* Scenes.h
   Placeholder for legacy compatibility - all scenes now in FFat
   VERSION: V15.3.0-2026-01-04T16:00:00Z - All scenes migrated to FFat JSON
*/

#ifndef SCENES_H
#define SCENES_H

#include "MatrixDisplay.h"
#include "Config.h"

class Scenes {
public:
  Scenes(MatrixDisplay* display);
  
  // V15.3.0-2026-01-04T16:00:00Z - All scenes removed (use FFat)
  // This class kept for backwards compatibility only
  
private:
  MatrixDisplay* disp;
};

#endif