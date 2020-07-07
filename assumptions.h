#pragma once

#include <vector>
#include <string>

namespace css
{

struct Assumptions
{
  float full_regen_limit = 0.3f;  // overrided by -r argument
  float buff_fraction = 0.2f;  // overridden by -b argument, set to 1.0 on mode 5

  const std::string h = "Heal";
  const std::string gh = "Greater Heal";
  const std::string fh = "Flash Heal";
  const std::string r = "Renew";
  const std::string poh = "Prayer of Healing";
  const std::string s = "Power Word: Shield";
  const std::vector<std::string> spell_names = {h,    h,    gh,   gh,   fh,   fh,   fh,   r,    poh,  s};
  std::vector<float> spell_max_freqs =         {1.0f, 1.0f, 1.0f, 0.2f, 1.0f, 1.0f, 1.0f, 0.2f, 0.1f, 0.3f};
  // std::vector<float> spell_max_freqs =      {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
  std::vector<int> spell_ranks =               {2,    4,    1,    4,    1,    4,    7,    9,    4,    10};
  std::vector<float> spell_overheal =          {0.3f, 0.4f, 0.4f, 0.5f, 0.05f,0.1f, 0.15f,0.3f, 0.2f, 0.0f};
  std::vector<float> initial_spell_counts =    {1,    0,    0,    0,    0,    0,    0,    0,    0,    0};
  std::vector<float> fixed_spell_counts =      {1,    0,    0,    0,    0,    0,    0,    0,    0,    0};
  bool use_fixed_spell_counts = 0;

  //                                     casts, ticks, ticks_oom
  std::vector<int> initial_regen_vals = {20,    20,    20};
 
  
  // For each spell sequence, e.g. 1-20 casts, then 0-10 ticks of regen. when oom, regen 0-10 ticks and until mana for next spell is obtained
  int max_casts = 20;
  int max_ticks = 10;
  int max_ticks_oom = 0;

  bool penalize_oom = true;


  float pvp_combat_length = 100.0f;  // seconds

  float healing_from_crit_fraction = 0.2f;  // e.g. 20% of extra healing from crit is not overhealing

  std::vector<float> pve_combat_lengths = {60.0f, 120.0f, 180.0f, 240.0f, 300.0f, 360.0f};
  // std::vector<float> pve_combat_lengths = {150.0f};
  std::vector<float> pve_combat_weights = {1.0f,  2.0f,   2.0f,   1.0f,   1.0f,   1.0f};

  std::vector<float> target_dps_in =      {100,   100,    100,    100,    100,    100};

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
  float mana_penalty = 0.0f;

  bool holy_disc = true;

  // Log input related
  std::string log_in = "";
  float min_combat_length = 30.0f; // 30.0f;
  float max_log_entry_diff_in_combat = 10.0f; //600.0f; // 2.0f
  bool precast = true;
  bool swap_cast = true;
  float time_step = 0.1f;  // s
  float reaction_time = 0.5f;  // s

  std::string skip_player = "Paisti-Gehennas";

  int n_last_entries_for_alt_stats = 3000;

  bool use_deficit_time_sum = false;

  int water_type = 0;  // 0 = 4200/30s (dm water), 1 = 4410/30s (manna biscuits); 2 = 4%/s (festival dumplings)

  // Pop +healing and e.g. zg priest trinket at this
  float total_deficit_to_pop_trinkets = 10000.0f;

  // Related to speedup by dropping bad items early
  bool drop_bad_items_early = true;
  int keep_best_per_slot = 4;

  // No renew / T2 8p HoT wanted on these on log based. Essentially, list warriors here / in your conf
  // WIP / not really implemented :)
  std::vector<std::string> no_renew_on = {"Rawrmew-Gehennas", "Canoe-Gehennas", "Scottw-Gehennas", "Ripu-Gehennas",
    "Cadeyrn-Gehennas", "Kozue-Gehennas", "Cylian-Gehennas", "Gazghul-Gehennas", "Furygor-Gehennas", "Deviyo-Gehennas"};

  bool use_alt_for_log_based_picks = true;

  bool wcl_log = false;
  bool blizzard_log = false;  // dummy really atm

  float cast_delay = 0.15f;  // s

  // E.g. zg buff -> 1.15
  float all_stats_mul = 1.0f;

  int log_based_calc_stride = 1;

  // will be added to stat weight calc if agrees within this limit, e.g. +- 50%
  // i.e., if item would be valued to 10 based on current stat weights, it's stat,val -pair will be added
  // to further calculations if it's in range [5,20]
  float log_based_outlier_frac = 0.5f;
};

namespace global
{

extern Assumptions assumptions;

}  // namespace global

void ReadAssumptions(const std::string& fn);

}  // namespace css
