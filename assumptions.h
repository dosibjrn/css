#pragma once

#include <vector>
#include <string>

namespace css
{

struct Assumptions
{
  float full_regen_limit = 0.3f;  // overrided by -r argument
  float buff_fraction = 0.2f;  // overridden by -b argument, set to 1.0 on mode 5

  // gh = greater heal; fh = flash heal; h = heal; r = renew; poh = prayer of healing
  // Note: spell order:                      h    h    gh   gh   fh   r    poh
  std::vector<float> spell_max_freqs =      {1.0, 1.0, 1.0, 0.2, 1.0, 0.2, 0.1};
  // std::vector<float> spell_max_freqs =      {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  std::vector<int> spell_ranks =            {2,   4,   1,   4,   7,   9,   4};   
  std::vector<float> initial_spell_counts = {1,   0,   0,   0,   0,   0,   0};
  std::vector<float> fixed_spell_counts =   {1,   0,   0,   0,   0,   0,   0};
  bool use_fixed_spell_counts = 0;

  //                                     casts, ticks, ticks_oom
  std::vector<int> initial_regen_vals = {20,    20,    20};
 
  
  // For each spell sequence, e.g. 1-20 casts, then 0-10 ticks of regen. when oom, regen 0-10 ticks and until mana for next spell is obtained
  int max_casts = 20;
  int max_ticks = 10;
  int max_ticks_oom = max_ticks;

  bool penalize_oom = true;


  float pvp_combat_length = 100.0f;  // seconds

  float healing_from_crit_fraction = 0.2f;  // e.g. 20% of extra healing from crit is not overhealing

  std::vector<float> pve_combat_lengths = {60.0f, 120.0f, 180.0f, 240.0f, 300.0f, 360.0f};
  // std::vector<float> pve_combat_lengths = {150.0f};
  std::vector<float> pve_combat_weigths = {1.0f,  2.0f,   2.0f,   1.0f,   1.0f,   1.0f};

  // Items with special effects
  std::string darkmoon_card_name = "darkmoon card: blue dragon";

  // Is the transcendence 8 really obtainable for you?
  bool transc8_exists = false;

  // Note: quite limited for now reflecting my broke ass self
  bool enchantments = 1;

  // Raid buffs
  bool fort = 1;
  bool motw = 1;
  bool spirit = 1;
  bool ai = 1;


  bool flask = false;

  // More
  float target_hp = 10000.0f;
  bool pi_self = false;
};

namespace global
{

extern Assumptions assumptions;

}  // namespace global

void ReadAssumptions(const std::string& fn);

}  // namespace css
