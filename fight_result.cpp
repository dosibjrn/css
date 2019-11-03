#include "fight_result.h"

namespace css
{

FightResult fightResultSum(const FightResult &a, const FightResult &b)
{
  FightResult sum;
  sum.mana_drop = a.mana_drop + b.mana_drop;
  sum.time_to_out_of_fsr = a.time_to_out_of_fsr + b.time_to_out_of_fsr;
  sum.duration = a.duration + b.duration;
  sum.damage_spells = a.damage_spells + b.damage_spells;
  sum.damage_wand = a.damage_wand + b.damage_wand;
  sum.time_wand = a.time_wand + b.time_wand;
  sum.spell_total_cost = a.spell_total_cost + b.spell_total_cost;
  sum.dps = a.dps + b.dps;
  sum.mobs_per_min = a.mobs_per_min + b.mobs_per_min;
  sum.regen_for_next_time = a.regen_for_next_time + b.regen_for_next_time;
  sum.drink_to_full_time = a.drink_to_full_time + b.drink_to_full_time;
  sum.mobs_per_drink = a.mobs_per_drink + b.mobs_per_drink;
  sum.shields = a.shields + b.shields;
  sum.blasts = a.blasts + b.blasts;
  sum.flay_ticks = a.flay_ticks + b.flay_ticks;
  sum.touches = a.touches + b.touches;
  sum.blast_down_rank = a.blast_down_rank + b.blast_down_rank;
  sum.flay_down_rank = a.flay_down_rank + b.flay_down_rank;
  sum.diff = a.diff + b.diff;
  return sum;
}

void fightResultMul(float mul, FightResult* a)
{
  a->mana_drop *= mul;
  a->time_to_out_of_fsr *= mul;
  a->duration *= mul;
  a->damage_spells *= mul;
  a->damage_wand *= mul;
  a->time_wand *= mul;
  a->spell_total_cost *= mul;
  a->dps *= mul;
  a->mobs_per_min *= mul;
  a->regen_for_next_time *= mul;
  a->drink_to_full_time *= mul;
  a->mobs_per_drink *= mul;
  a->shields *= mul;
  a->blasts *= mul;
  a->flay_ticks *= mul;
  a->touches *= mul;
  a->blast_down_rank *= mul;
  a->flay_down_rank *= mul;
  a->diff *= mul;
}

void coutFightResult(const FightResult& a)
{
  std::cout << "mana_drop :           " << a.mana_drop << std::endl;
  std::cout << "time_to_out_of_fsr :  " << a.time_to_out_of_fsr << std::endl;
  std::cout << "duration :            " << a.duration << std::endl;
  std::cout << "damage_spells :       " << a.damage_spells << std::endl;
  std::cout << "damage_wand :         " << a.damage_wand << std::endl;
  std::cout << "time_wand :           " << a.time_wand << std::endl;
  std::cout << "spell_total_cost :    " << a.spell_total_cost << std::endl;
  std::cout << "dps :                 " << a.dps << std::endl;
  std::cout << "mobs_per_min :        " << a.mobs_per_min << std::endl;
  std::cout << "regen_for_next_time : " << a.regen_for_next_time << std::endl;
  std::cout << "mobs_per_drink :      " << a.mobs_per_drink << std::endl;
  std::cout << "shields :             " << a.shields << std::endl;
  std::cout << "blasts :              " << a.blasts << std::endl;
  std::cout << "flay_ticks :          " << a.flay_ticks << std::endl;
  std::cout << "touches :             " << a.touches << std::endl;
  std::cout << "blast_down_rank :     " << a.blast_down_rank << std::endl;
  std::cout << "flay_down_rank :      " << a.flay_down_rank << std::endl;
  std::cout << "diff:                 " << a.diff << std::endl;
}

}  // namespace css

