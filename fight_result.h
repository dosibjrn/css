#pragma once

namespace css
{

struct FightResult {
  float mana_drop = 0.0f;
  float time_to_out_of_fsr = 0.0f;
  float duration = 0.0f;
  float damage_spells = 0.0f;
  float damage_wand = 0.0f;
  float time_wand = 0.0f;
  float spell_total_cost = 0.0f;
  float dps = 0.0f;
  float mobs_per_min = 0.0f;
  float regen_for_next_time = 0.0f;
  float drink_to_full_time = 0.0f;
  float mobs_per_drink = 0.0f;
  float shields = 0.0f;
  float blasts = 0.0f;
  float flay_ticks = 0.0f;
  float touches = 0.0f;
  float blast_down_rank = 0.0f;
  float flay_down_rank = 0.0f;
  float diff = 0.0f;
};

FightResult fightResultSum(const FightResult &a, const FightResult &b);
void fightResultMul(float mul, FightResult* a);
void coutFightResult(const FightResult& a);

}  // namespace css

