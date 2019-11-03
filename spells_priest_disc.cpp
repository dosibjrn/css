#include "spells_priest_disc.h"

namespace css
{

Spell Shield(const PriestCharacter& c, int rank)
{
  Spell s;
  s.cost = 355; // This is rank8
  s.shield = 6220;
  // std::cout << "Check Shield impl." << std::endl;
  return s;
}

}  // namespace css

