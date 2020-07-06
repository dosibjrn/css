#include "assumptions.h"

#include <iostream>
#include <fstream>
#include <map>

#include "csv.h"

namespace css
{

namespace global
{

Assumptions assumptions;

}  // namespace global

namespace
{

template<typename TypeIn, typename TypeOut>
void SetVal(const std::vector<TypeIn>& vals_in, std::vector<TypeOut>* vals_out)
{
  vals_out->clear();
  std::cout << "saw vals: ";
  for (auto val : vals_in) {
    vals_out->push_back(static_cast<TypeOut>(val));
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

template<typename TypeIn, typename TypeOut>
void SetVal(const std::vector<TypeIn>& vals_in, TypeOut* val_out)
{
  *val_out = static_cast<TypeOut>(vals_in[0]);
  std::cout << "saw val: " << vals_in[0] << std::endl;
}

template<typename TypeIn, typename TypeOut>
void SetIfGiven(const std::string& key, std::map<std::string, std::vector<TypeIn>>& assumptions_in, TypeOut* val)
{
  if (assumptions_in.find(key) != assumptions_in.end()) {
    const auto& vals = assumptions_in[key];
    std::cout << "Found key: " << key << ", ";
    SetVal(vals, val);
  }
}

std::map<std::string, std::vector<float>> AssumptionsToMap(const std::string &fn)
{
  std::map<std::string, std::vector<float>> assumptions_in;
  std::ifstream is(fn.c_str());
  while (is.good()) {
    std::string line;
    std::getline(is, line);
    line = RemoveComments(line);
    if (!line.empty()) {
      auto entries = SplitCsvLine(line);
      if (entries.size() > 1) {
        std::string key = entries[0];
        assumptions_in[key] = std::vector<float>();
        for (size_t i = 1; i < entries.size(); ++i) {
          assumptions_in[key].push_back(static_cast<float>(atof(entries[i].c_str())));
        }
      }
    }
  }
  return assumptions_in;
}

std::map<std::string, std::vector<std::string>> AssumptionsToStringMap(const std::string &fn)
{
  std::map<std::string, std::vector<std::string>> assumptions_in;
  std::ifstream is(fn.c_str());
  while (is.good()) {
    std::string line;
    std::getline(is, line);
    line = RemoveComments(line);
    if (!line.empty()) {
      auto entries = SplitCsvLine(line);
      if (entries.size() > 1) {
        std::string key = entries[0];
        assumptions_in[key] = std::vector<std::string>();
        for (size_t i = 1; i < entries.size(); ++i) {
          assumptions_in[key].push_back(entries[i]);
        }
      }
    }
  }
  return assumptions_in;
}

}  // namespace

void ReadAssumptions(const std::string& fn)
{
  auto assumptions_in = AssumptionsToMap(fn);
  auto assumptions_in_string = AssumptionsToStringMap(fn);
  SetIfGiven("full_regen_limit", assumptions_in, &global::assumptions.full_regen_limit);
  SetIfGiven("buff_fraction", assumptions_in, &global::assumptions.buff_fraction);
  SetIfGiven("spell_max_freqs", assumptions_in, &global::assumptions.spell_max_freqs);
  SetIfGiven("spell_ranks", assumptions_in, &global::assumptions.spell_ranks);
  SetIfGiven("initial_spell_counts", assumptions_in, &global::assumptions.initial_spell_counts);
  SetIfGiven("fixed_spell_counts", assumptions_in, &global::assumptions.fixed_spell_counts);
  SetIfGiven("use_fixed_spell_counts", assumptions_in, &global::assumptions.use_fixed_spell_counts);
  SetIfGiven("initial_regen_vals", assumptions_in, &global::assumptions.initial_regen_vals);
  SetIfGiven("max_casts", assumptions_in, &global::assumptions.max_casts);
  SetIfGiven("max_ticks", assumptions_in, &global::assumptions.max_ticks);
  SetIfGiven("max_ticks_oom", assumptions_in, &global::assumptions.max_ticks_oom);
  SetIfGiven("penalize_oom", assumptions_in, &global::assumptions.penalize_oom);
  SetIfGiven("pvp_combat_length", assumptions_in, &global::assumptions.pvp_combat_length);
  SetIfGiven("healing_from_crit_fraction", assumptions_in, &global::assumptions.healing_from_crit_fraction);
  SetIfGiven("pve_combat_lengths", assumptions_in, &global::assumptions.pve_combat_lengths);
  SetIfGiven("pve_combat_weights", assumptions_in, &global::assumptions.pve_combat_weights);
  SetIfGiven("transc8_exists", assumptions_in, &global::assumptions.transc8_exists);
  SetIfGiven("enchantments", assumptions_in, &global::assumptions.enchantments);
  SetIfGiven("fort", assumptions_in, &global::assumptions.fort);
  SetIfGiven("motw", assumptions_in, &global::assumptions.motw);
  SetIfGiven("spirit", assumptions_in, &global::assumptions.spirit);
  SetIfGiven("ai", assumptions_in, &global::assumptions.ai);
  SetIfGiven("flask", assumptions_in, &global::assumptions.flask);

  SetIfGiven("target_hp", assumptions_in, &global::assumptions.target_hp);
  SetIfGiven("mana_penalty", assumptions_in, &global::assumptions.mana_penalty);
  SetIfGiven("holy_disc", assumptions_in, &global::assumptions.holy_disc);

  SetIfGiven("min_combat_length", assumptions_in, &global::assumptions.min_combat_length);
  SetIfGiven("max_log_entry_diff_in_combat", assumptions_in, &global::assumptions.max_log_entry_diff_in_combat);
  SetIfGiven("precast", assumptions_in, &global::assumptions.precast);
  SetIfGiven("swap_cast", assumptions_in, &global::assumptions.swap_cast);
  SetIfGiven("time_step", assumptions_in, &global::assumptions.time_step);
  SetIfGiven("reaction_time", assumptions_in, &global::assumptions.reaction_time);
  SetIfGiven("skip_player", assumptions_in_string, &global::assumptions.skip_player);
  SetIfGiven("n_last_entries_for_alt_stats", assumptions_in, &global::assumptions.n_last_entries_for_alt_stats);
  SetIfGiven("use_deficit_time_sum", assumptions_in, &global::assumptions.use_deficit_time_sum);
  SetIfGiven("water_type", assumptions_in, &global::assumptions.water_type);
  SetIfGiven("total_deficit_to_pop_trinkets", assumptions_in, &global::assumptions.total_deficit_to_pop_trinkets);

  SetIfGiven("no_renew_on", assumptions_in_string, &global::assumptions.no_renew_on);
  SetIfGiven("use_alt_for_log_based_picks", assumptions_in, &global::assumptions.use_alt_for_log_based_picks);

  SetIfGiven("drop_bad_items_early", assumptions_in, &global::assumptions.drop_bad_items_early);
  SetIfGiven("keep_best_per_slot", assumptions_in, &global::assumptions.keep_best_per_slot);

  SetIfGiven("blizzard_log", assumptions_in, &global::assumptions.blizzard_log);
  SetIfGiven("wcl_log", assumptions_in, &global::assumptions.wcl_log);
  SetIfGiven("cast_delay", assumptions_in, &global::assumptions.cast_delay);
  SetIfGiven("log_based_calc_stride", assumptions_in, &global::assumptions.log_based_calc_stride);

  // post process
  global::assumptions.n_last_entries_for_alt_stats /= global::assumptions.log_based_calc_stride;
}

}  // namespace css

