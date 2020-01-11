#pragma once

#include <algorithm>
#include <map>
#include <random>

#include "assumptions.h"
#include "item.h"
#include "item_table.h"
#include "priest_character.h"
#include "regen.h"

namespace css
{


class ItemPicker {
 public:
  enum class ValueChoice {
    pvp_shadow,
    pvp_healing,
    pve_healing,
  };
  ItemPicker(const PriestCharacter& c, std::string item_table_name, ValueChoice value_choice = ValueChoice::pvp_shadow);
  void AddLocked(std::string s) { m_locked[s] = true; }
  void ClearLocked() { m_locked.clear(); }
  void AddBanned(std::string s) { m_banned[s] = true; }
  void AddWhitelisted(std::string s) { m_whitelist[s] = true; }

  void Calculate();
  void PickBestForSlots(const ItemTable &item_table, bool disable_bans, int iteration, int max_iterations, //
                        int* static_for_all_slots, int* iters_without_new_best);
  void FinalCouts();

  void CoutBestItems();
  void CoutCharacterStats() const;
  void CoutCurrentValues() const;
  void CoutCurrentValuesAlt() const;
  void CoutBestCounts() const;
  void CoutDiffsToStart();
  void CoutAllUpgrades();
  void CoutAllUpgradesFromStart();

  std::vector<Item> getBestItems() const;
  float getBestValue() const { return m_val_best; }
  PriestCharacter getCharacter() { return m_c_best; }

private:
  Item pickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name = "") const;
  float value(const PriestCharacter& c) const;
  float valuePvpShadow(const PriestCharacter& c) const;
  float valuePvpHealing(const PriestCharacter& c) const;
  float valuePveHealing(const PriestCharacter& c) const;

  std::vector<float> getPveInfo(const PriestCharacter& c) const;

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

  std::vector<float> m_pve_info;

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
};

}  // namespace css

// All in all could be: one slot at a time, pick best for slot. When no slot changed, we're good.
