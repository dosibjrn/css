#pragma once

#include <string>

#include "school.h"

namespace css
{

struct Spell
{
  std::string name = "";
  float cast_time = 1.5f;
  float cost = 0.0f;
  float damage = 0.0f;  // per tick
  float healing = 0.0f;
  float num_ticks = 1.0f;
  float tick_after = 0.0f;
  float modifier = 0.428f;  // per tick
  float shield = 0.0f;
  bool channeled = false;
  bool over_time = false;
  bool instant = false;
  float cd = 0.0f;
  int level_req = 0;
  int rank = 0;
  bool can_crit = true;

  School type = School::Shadow;

  // for over_time spells
  int ticked = 0;
  float start = 0.0f;
};

}  // namespace css
