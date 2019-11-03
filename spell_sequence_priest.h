#pragma once

#include <vector>

#include "priest_character.h"
#include "spell.h"

namespace css
{

std::vector<Spell> SpellsSequencePaistiLvl51(const PriestCharacter& c, int mind_flay_ticks, int mind_blasts, int shields, int touches);
std::vector<Spell> SpellsSequencePaistiLvl52(const PriestCharacter& c, int mind_flay_ticks, int mind_blasts, int shields, int touches,
                                             float flay_down_rank = 0.0f, float blast_down_rank = 0.0f);
}  // namespace css

