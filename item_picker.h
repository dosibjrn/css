#pragma once

#include <algorithm>
#include <map>
#include <random>

#include "assumptions.h"
#include "item.h"
#include "item_table.h"
#include "parse_based.h"
#include "priest_character.h"
#include "regen.h"
#include "hps.h"

namespace css
{


class ItemPicker {
 public:
  enum class ValueChoice {
    pvp_shadow,
    pvp_healing,
    pve_healing,
    pve_healing_log,
  };
  ItemPicker(const PriestCharacter& c, std::string item_table_name, ValueChoice value_choice = ValueChoice::pvp_shadow);
  void setTagName(const std::string& tag_name) { m_tag_name = tag_name; }
  void AddLocked(std::string s) { m_locked[s] = true; }
  void ClearLocked() { m_locked.clear(); }
  void AddBanned(std::string s) { m_banned[s] = true; }
  void AddWhitelisted(std::string s) { m_whitelist[s] = true; }

  void AddLog(const std::string& log_fn);

  void Calculate();
  void PickBestForSlots(const ItemTable &item_table, bool disable_bans, int iteration, int max_iterations, //
                        int* static_for_all_slots, int* iters_without_new_best);
  void FinalCouts();

  void CoutBestItems();
  void CoutCharacterStats() const;
  void CoutCurrentValues(std::string tag_name = "");
  void CoutCurrentValuesBasedOnRecordedDiffs(std::string tag_name);
  void CoutBestCounts() const;
  void CoutDiffsToStart();
  void CoutAllUpgrades(bool partial, bool from_start);

  std::vector<Item> getBestItems() const;
  float getBestValue() const { return m_val_best; }
  PriestCharacter getCharacter() { return m_c_best; }

private:
  Item pickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name = "", bool use_alt = false);
  float value(const PriestCharacter& c) const;
  float valuePvpShadow(const PriestCharacter& c) const;
  float valuePvpHealing(const PriestCharacter& c) const;
  float valuePveHealing(const PriestCharacter& c) const;
  float valuePveCombatLogHealing(const PriestCharacter& c) const;

  std::vector<PveInfo> getPveInfo(const PriestCharacter& c) const;

  void intermediateCout(int iteration);

  std::vector<std::vector<float>> bestCounts(const PriestCharacter& c,
                                            const std::vector<std::vector<float>>& init_counts,
                                            std::vector<Regen> *regens) const;

  bool isLocked(std::string s) const { return m_locked.find(s) != m_locked.end(); }
  bool isBanned(std::string s) const;
  bool isBanned(const Item& i) const { return isBanned(i.name) || isBanned(i.source); }
  bool isWhitelisted(std::string s) const { return m_whitelist.find(s) != m_whitelist.end(); }
  void updateIfNewBest(float val, bool disable_bans);

  void updateIfNewBest(float val, bool disable_bans, int iteration, int* iters_without_new_best);

  float valueIncreaseWeightsBased(const Item& item, float* special = nullptr);

  void swapToBestMatchingBonuses(const ItemTable& item_table);

  template<typename T>
      void shuffle(std::vector<T>* v)
      {
        int64_t s = v->size();
        std::vector<int64_t> ixs;
        for (int64_t i = 0; i < s; ++i) ixs.push_back(i); 

        std::shuffle(ixs.begin(), ixs.end(), m_generator);
        std::vector<T> tmp(s);
        std::swap(tmp, *v);
        for (int64_t i = 0; i < s; ++i) {
          v->at(i) = tmp[ixs[i]];
        }
      }

  std::map<std::string, bool> m_locked;
  std::map<std::string, bool> m_banned;
  std::map<std::string, bool> m_whitelist;  // to go around a ban of items one has

  PriestCharacter m_c_start;
  PriestCharacter m_c_curr;
  PriestCharacter m_c_best;

  std::string m_item_table_name;

  std::vector<std::vector<float>> m_start_pve_healing_counts;
  std::vector<std::vector<float>> m_curr_pve_healing_counts;
  std::vector<std::vector<float>> m_best_pve_healing_counts;
  const std::vector<float> m_pve_healing_combat_lengths = global::assumptions.pve_combat_lengths;

  std::vector<PveInfo> m_pve_info;

  // const std::vector<float> m_pve_healing_combat_lengths = {120.0f};
  // const std::vector<float> m_pve_healing_combat_lengths = {60.0f, 120.0f, 180.0f};
  std::vector<Regen> m_start_regens;
  std::vector<Regen> m_curr_regens;
  std::vector<Regen> m_best_regens;

  float m_val_best = 0.0f;
  float m_val_best_bans_on = 0.0f;
  std::map<std::string, Item> m_items;
  std::map<std::string, Item> m_items_best;
  std::map<std::string, Item> m_items_prev_intermediate_results;
  std::map<std::string, Item> m_items_start;

  std::default_random_engine m_generator;

  ValueChoice m_value_choice = ValueChoice::pvp_shadow;

  bool m_pve_healing_optimizes_counts = false;

  std::string m_tag_name;

  // Combat Log Healing related shizzles
  LogsType m_logs;
  float m_oh_limit = 0.9f;
  float m_time_left_mul = 0.5f;

  // For item comparisons -> stat weights
  std::vector<std::pair<Item, float>> m_stat_diffs_to_hps_diffs;
  std::vector<float> m_weights;
  static constexpr int m_weights_size = 7;

  float m_baseline_deficit_time_sum = 0.0f;
};

}  // namespace css

// All in all could be: one slot at a time, pick best for slot. When no slot changed, we're good.
