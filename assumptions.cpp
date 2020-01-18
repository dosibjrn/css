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

template<typename T>
void SetVal(const std::vector<float>& vals_in, std::vector<T>* vals_out)
{
  vals_out->clear();
  std::cout << "saw vals: ";
  for (auto val : vals_in) {
    vals_out->push_back(val);
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

template<typename T>
void SetVal(const std::vector<float>& vals_in, T* val_out)
{
  *val_out = vals_in[0];
  std::cout << "saw val: " << vals_in[0] << std::endl;
}

template<typename T>
void SetIfGiven(const std::string& key, std::map<std::string, std::vector<float>>& assumptions_in, T* val)
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
        for (int i = 1; i < entries.size(); ++i) {
          assumptions_in[key].push_back(atof(entries[i].c_str()));
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
  SetIfGiven("transc8_exists", assumptions_in, &global::assumptions.transc8_exists);
  SetIfGiven("enchantments", assumptions_in, &global::assumptions.enchantments);
  SetIfGiven("fort", assumptions_in, &global::assumptions.fort);
  SetIfGiven("motw", assumptions_in, &global::assumptions.motw);
  SetIfGiven("spirit", assumptions_in, &global::assumptions.spirit);
  SetIfGiven("ai", assumptions_in, &global::assumptions.ai);
  SetIfGiven("flask", assumptions_in, &global::assumptions.flask);

  SetIfGiven("target_hp", assumptions_in, &global::assumptions.target_hp);
}

}  // namespace css

