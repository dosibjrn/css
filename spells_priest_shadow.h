#pragma once

#include "priest_character.h"
#include "spell.h"

namespace css
{

float ModifiedShadow(const PriestCharacter& c, const Spell& s);
Spell MindBlast(const PriestCharacter& c, int rank);
Spell Swp(const PriestCharacter&c, int rank);
Spell MindFlay(const PriestCharacter& c, int rank, int ticks);
Spell VampiricEmbrace();
Spell TouchOfWeakness(const PriestCharacter& c, int rank);

}  // namespace css
