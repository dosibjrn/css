#pragma once

#include <tuple>
#include <vector>

#include "spell.h"
#include "regen.h"
#include "priest_character.h"

namespace css
{

struct PveInfo
{
  float target_alive_mul = 1.0f;
  float oom_penalty_mul = 1.0f;
  float regen_penalty_mul = 1.0f;
  float hps = 1.0f;
};

std::pair<float, PveInfo> HpsWithRegen(const PriestCharacter& c, const std::vector<Spell>& spell_sequence, float end_at_s, Regen regen);
float HpsPvp(const PriestCharacter& c);
float HpsPve(const PriestCharacter& c, float combat_length);
std::vector<Spell> PveHealingSequence(const PriestCharacter& c);
std::vector<Spell> PveHealingSequence(const PriestCharacter& c, const std::vector<float>& spell_counts);
std::vector<float> FindBestPveHealingCounts(const PriestCharacter& c, 
                                            const std::vector<float>& initial_counts,
                                            float combat_length,
                                            Regen* regen);
Spell IxToSpell(const PriestCharacter& c, int choice_ix);

std::vector<float> InitialSpellCounts();
Regen InitialRegen();
Regen FindBestRegen(const PriestCharacter& c, 
                    const std::vector<float>& spell_counts, 
                    float combat_length, 
                    Regen current_regen); 
}  // namespace css
