#include "stats.h"

#include "spells_priest_disc.h"

namespace css 
{

float Stats::getEffectiveHp()
{
  Spell shield = Shield(c_, 10); 
  float ehp = c_.base_hp + (c_.stamina + 70)*10.0f + 2.0f*shield.shield;
  return ehp;
}

}  // namespace css
