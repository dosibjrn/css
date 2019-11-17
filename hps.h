#pragma once

#include <vector>

#include "spell.h"
#include "priest_character.h"

namespace css
{

float Hps(const PriestCharacter& c, const std::vector<Spell>& spell_sequence, float end_at_s, float mana_to_regen);
float HpsPvp(const PriestCharacter& c);
float HpsPve(const PriestCharacter& c, float combat_length);
std::vector<Spell> PveHealingSequence(const PriestCharacter& c);
std::vector<Spell> PveHealingSequence(const PriestCharacter& c, const std::vector<float>& spell_counts);
std::vector<float> FindBestPveHealingCounts(const PriestCharacter& c, 
                                            const std::vector<float>& initial_counts,
                                            float combat_length,
                                            float* mana_to_regen_mul);
Spell IxToSpell(const PriestCharacter& c, int choice_ix);
}  // namespace css
