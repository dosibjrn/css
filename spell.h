#pragma once

#include "school.h"

namespace css
{

struct Spell
{
  float cast_time = 1.5f;
  float cost = 0.0f;
  float damage = 0.0f;  // per tick
  float healing = 0.0f;
  float num_ticks = 1.0f;
  float tick_after = 0.0f;
  float modifier = 0.428f;  // per tick
  float shield = 0.0f;
  bool channeled = false;
  bool dot = false;
  float cd = 0.0f;
  int level_req = 0;
  bool can_crit = true;
  School type = School::Shadow;
};

}  // namespace css
