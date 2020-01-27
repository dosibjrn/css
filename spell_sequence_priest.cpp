#include "spell_sequence_priest.h"

#include <algorithm>

#include "spells_priest_shadow.h"
#include "spells_priest_disc.h"

namespace css
{

std::vector<Spell> SpellsSequencePaistiLvl51(const PriestCharacter& c, int mind_flay_ticks, int mind_blasts, int shields, int touches)
{
  std::vector<Spell> spells;
  if (mind_blasts > 0) {
    spells.push_back(MindBlast(c, 7));
    mind_blasts--;
  }
  spells.push_back(Swp(c, 7));
  if (shields > 0) {
    spells.push_back(Shield(c, 7));
    shields--;
  }
  spells.push_back(VampiricEmbrace());
  while (mind_flay_ticks > 0) {
    int ticks = std::min(3, mind_flay_ticks);
    spells.push_back(MindFlay(c, 4, ticks));
    mind_flay_ticks -= ticks;
  }
  while (touches > 0) {
    spells.push_back(TouchOfWeakness(c, 5));
    touches--;
  }
  while (mind_blasts > 0) {
    spells.push_back(MindBlast(c, 7));
    mind_blasts--;
  }
  while (shields > 0) {
    spells.push_back(Shield(c, 7));
    shields--;
  }
  return spells;
}

std::vector<Spell> SpellsSequencePaistiLvl52(const PriestCharacter& c, int mind_flay_ticks, int mind_blasts, int shields, int touches,
                                             float flay_down_rank, float blast_down_rank)
{
  std::vector<Spell> spells;
  int blast_base_down_rank = blast_down_rank;
  float blast_rem_down_rank = blast_down_rank - blast_base_down_rank;
  float blast_additional_down_rank_sum = 0.0f;

  int flay_base_down_rank = flay_down_rank;
  float flay_rem_down_rank = flay_down_rank - flay_base_down_rank;
  float flay_additional_down_rank_sum = 0.0f;

  int flay_max_rank = 5;
  int blast_max_rank = 8;


  if (mind_blasts > 0) {
    blast_additional_down_rank_sum += blast_rem_down_rank;
    spells.push_back(MindBlast(c, std::max<int>(1, std::min<int>(blast_max_rank, blast_max_rank - blast_base_down_rank))));
    mind_blasts--;
  }
  spells.push_back(Swp(c, 7));
  if (shields > 0) {
    spells.push_back(Shield(c, 7));
    shields--;
  }
  spells.push_back(VampiricEmbrace());
  bool first_flay = true;
  while (mind_flay_ticks > 0) {
    int ticks = std::min(3, mind_flay_ticks);
    
    if (shields == 0 && !first_flay) {
      ticks = std::min(2, mind_flay_ticks);
    }
    flay_additional_down_rank_sum += flay_rem_down_rank;
    spells.push_back(MindFlay(c, std::max<int>(1, std::min<int>(flay_max_rank, flay_max_rank - (flay_base_down_rank + flay_additional_down_rank_sum))), ticks));
    if (flay_additional_down_rank_sum >= 1.0f) {
      flay_additional_down_rank_sum -= 1.0f;
    }
    mind_flay_ticks -= ticks;
    first_flay = false;
  }
  while (touches > 0) {
    spells.push_back(TouchOfWeakness(c, 5));
    touches--;
  }
  while (mind_blasts > 0) {
    blast_additional_down_rank_sum += blast_rem_down_rank;
    spells.push_back(MindBlast(c, std::max<int>(1, std::min<int>(blast_max_rank, blast_max_rank - (blast_base_down_rank + blast_additional_down_rank_sum)))));
    if (blast_additional_down_rank_sum >= 1.0f) {
      blast_additional_down_rank_sum -= 1.0f;
    }
    mind_blasts--;
  }
  while (shields > 0) {
    spells.push_back(Shield(c, 7));
    shields--;
  }
  return spells;
}


}  // namespace css
