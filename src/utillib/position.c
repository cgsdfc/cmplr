#include "position.h"
#include <assert.h>

utillib_position const * utillib_position_current(void)
{
  static const utillib_position cur_pos={0,0};
  return &cur_pos;
}
