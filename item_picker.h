#pragma once

#include <map>

#include "item.h"
#include "priest_character.h"

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
  void AddBanned(std::string s) { m_banned[s] = true; }
  void Calculate();

  void CoutBestItems();
  void CoutCharacterStats() const;
  void CoutCurrentValues() const;
  void CoutCurrentValuesAlt() const;
  void CoutBestCounts() const;

  std::vector<Item> getBestItems() const;
  float getBestValue() const { return m_val_best; }
  PriestCharacter getCharacter() { return m_c_best; }

private:
  Item pickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name = "") const;
  float value(const PriestCharacter& c) const;
  float valuePvpShadow(const PriestCharacter& c) const;
  float valuePvpHealing(const PriestCharacter& c) const;
  float valuePveHealing(const PriestCharacter& c) const;

  void intermediateCout(int iteration);

  std::vector<std::vector<float>> bestCounts(const PriestCharacter& c,
                                            const std::vector<std::vector<float>>& init_counts,
                                            std::vector<float> *mana_to_regen_muls) const;

  bool isLocked(std::string s) const { return m_locked.find(s) != m_locked.end(); }
  bool isBanned(std::string s) const { return m_banned.find(s) != m_banned.end(); }

  std::map<std::string, Item> m_items;
  std::map<std::string, bool> m_locked;
  std::map<std::string, bool> m_banned;
  PriestCharacter m_c_in;
  PriestCharacter m_c_curr;
  std::string m_item_table_name;
  std::vector<std::vector<float>> m_curr_pve_healing_counts;
  // const std::vector<float> m_pve_healing_combat_lengths = {120.0f, 180.0f, 240.0f, 300.0f, 360.0f};
  const std::vector<float> m_pve_healing_combat_lengths = {120.0f};
  std::vector<float> m_mana_to_regen_muls =               {0.0f,   0.0f,   0.0f,   0.0f,   0.0f};
  // const std::vector<float> m_pve_healing_combat_lengths = {60.0f, 120.0f, 180.0f};


  PriestCharacter m_c_best;
  std::vector<std::vector<float>> m_pve_healing_counts_best;
  float m_val_best;
  std::map<std::string, Item> m_items_best;
  std::map<std::string, Item> m_items_prev_intermediate_results;

  ValueChoice m_value_choice = ValueChoice::pvp_shadow;
};

}  // namespace css

// All in all could be: one slot at a time, pick best for slot. When no slot changed, we're good.
