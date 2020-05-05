#pragma once

#include "priest_character.h"

namespace css {

struct Water {
  float per_tick = 0.0f;
  float num_ticks = 0.0f;
};

Water BestWater(const PriestCharacter &c);

}  // namespace css


