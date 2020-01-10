#pragma once

#include <vector>
#include <string>

namespace css
{

struct Assumptions
{
  float full_regen_limit = 1.0f;  // overrided by -r argument
  float buff_fraction = 1.0f;  // overridden by -b argument

  // gh = greater heal; fh = flash heal; h = heal; r = renew; poh = prayer of healing
  // Note: spell order:                    h    h    gh   gh   fh   r   poh
  std::vector<float> spell_max_freqs =    {1.0, 1.0, 1.0, 0.2, 1.0, 0.2, 0.1};
  std::vector<int> spell_ranks =          {2,   4,   1,   4,   7,   10,  5};   
  std::vector<float> initial_spell_counts = {1,   0,   0,   0,   0,   0,   0};
  std::vector<float> fixed_spell_counts =   {11,  0,   6,   0,   15,  3,   0};
  bool use_fixed_spell_counts = false;

  //                                     casts, ticks, ticks_oom
  std::vector<int> initial_regen_vals = {20,    20,    20};
 
  
  // For each spell sequence, e.g. 1-20 casts, then 0-10 ticks of regen. when oom, regen 0-10 ticks and until mana for next spell is obtained
  int max_casts = 20;
  int max_ticks = 10;
  int max_ticks_oom = 10;

  bool penalize_oom = true;


  float pvp_combat_length = 100.0f;  // seconds

  float healing_from_crit_fraction = 0.2f;  // e.g. 20% of extra healing from crit is not overhealing

  std::vector<float> pve_combat_lengths = {60.0f, 120.0f, 180.0f, 240.0f, 300.0f, 360.0f};
  std::vector<float> pve_combat_weights = {1.0f,  2.0f,   2.0f,   1.0f,   1.0f,   1.0f};

  // Items with special effects
  std::string darkmoon_card_name = "darkmoon card";
};

namespace global
{

extern Assumptions assumptions;

}  // namespace global

}  // namespace css
