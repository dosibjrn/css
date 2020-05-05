#include "water.h"

#include "assumptions.h"
#include "stats.h"

namespace css
{

Water BestWater(const PriestCharacter &c)
{
  int water_level = ((c.level - 5)/10)*10 + 5;
  Water w;
  float water_max = 0.0f;
  float dura = 0.0f;
  switch (water_level) {
    case 5:
      water_max = 436.8f;
      dura = 21;
    case 15 :
      water_max = 835.2f;
      dura = 24;
      break;
    case 25 :
      water_max = 1344.6f;
      dura = 27.0f;
      break;
    case 35 :
      water_max = 1992.0f;
      dura = 30.0f;
      break;
    case 45 :
      water_max = 2934.0f;
      dura = 30.0f;
      break;
    case 55 :
      water_max = 4200.0f;
      dura = 30.0f;
      break;
    default :  // lvl 1
      water_max = 151.2f;
      dura = 18.0f;
      break;
  }
  if (c.level >= 51) {
    if (global::assumptions.water_type == 1) {
      water_max = 4410.0f;
      dura = 30.0f;
    } 
    if (global::assumptions.water_tyoe == 2) {
      Stats s(c);
      water_max = s.getMaxMana();
      dura = 25.0f;
    }
  }

  w.num_ticks = dura/3.0f;
  w.per_tick = water_max/w.num_ticks;
  return w;
}

}  // namespace css
