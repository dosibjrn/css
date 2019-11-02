#pragma once

#include "spell.h"

namespace css
{

Spell MindBlast(const PriestCharacter& c, int rank);
Spell Swp(const PriestCharacter&c, int rank);
Spell MindFlay(const PriestCharacter& c, int rank, int ticks);
Spell VampiricEmbrace();
Spell TouchOfWeakness(const PriestCharacter& c, int rank);

}  // namespace css
