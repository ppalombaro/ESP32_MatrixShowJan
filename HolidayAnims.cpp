#include "HolidayAnims.h"
#include "AnimationIDs.h"

HolidayAnims::HolidayAnims(MatrixDisplay* d)
: disp(d),
  snowfall(disp),
  chase(disp),
  scroll(disp),
  countdown(disp),
  snowflake(disp),
  star(disp)
{
}

void HolidayAnims::beginAll() {
    snowfall.begin();
    chase.begin();
    scroll.begin();
    countdown.begin();
    snowflake.begin();
    star.begin();
}

void HolidayAnims::updateAll() {
    snowfall.update();
    chase.update();
    scroll.update();
    countdown.update();
    snowflake.update();
    star.update();
}

Animation* HolidayAnims::getAnimationById(int id) {
    switch (id) {
        case ANIM_SNOWFALL:   return &snowfall;
        case ANIM_CHASE:      return &chase;
        case ANIM_SCROLL:     return &scroll;
        case ANIM_COUNTDOWN:  return &countdown;
        case ANIM_SNOWFLAKE:  return &snowflake;
        case ANIM_STAR:       return &star;
        default:
            return nullptr;
    }
}
