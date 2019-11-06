#pragma once

#include <map>

#include "item.h"
#include "priest_character.h"

namespace css
{

class ItemPicker {
 public:
  ItemPicker(const PriestCharacter& c, std::string item_table_name);
  void AddLocked(std::string s) { m_locked[s] = true; }
  void AddBanned(std::string s) { m_banned[s] = true; }
  void Recalculate();

  void CoutBestItems();
  void CoutCharacterStats() const;

  std::vector<Item> getBestItems() const;
  float getBestValue() const { return value(m_c_curr); }
  PriestCharacter getCharacter() { return m_c_curr; }

private:
  Item pickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name = "") const;
  float value(const PriestCharacter& c) const;
  bool isLocked(std::string s) const { return m_locked.find(s) != m_locked.end(); }
  bool isBanned(std::string s) const { return m_banned.find(s) != m_banned.end(); }

  std::map<std::string, Item> m_items;
  std::map<std::string, bool> m_locked;
  std::map<std::string, bool> m_banned;
  PriestCharacter m_c_in;
  PriestCharacter m_c_curr;
  std::string m_item_table_name;
};

}  // namespace css

// All in all could be: one slot at a time, pick best for slot. When no slot changed, we're good.
