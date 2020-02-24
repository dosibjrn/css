#include "spells_priest_disc.h"

namespace css
{

Spell Shield(const PriestCharacter& c, int rank)
{
  Spell s;
  s.name = "Power Word: Shield";
  if (rank == 8) {
    s.cost = 355; // This is rank8
    s.shield = 622;
    s.rank = 8;
  } else {
    s.rank = 10;
    s.cost = 500;
    s.shield = 942;
  }
  s.shield *= (1.0f + 0.05f*c.talents.imp_pws);
  s.modifier = 0.1f;
  s.cast_time = 1.5f;
  s.instant = true;
  s.cd = 4.0f;
  s.shield += s.modifier*(c.sp + c.sp_healing);
  s.can_crit = false;
  return s;
}

}  // namespace css

