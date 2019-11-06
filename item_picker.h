#pragma once

#include <map>

#include "item.h"
#include "priest_character.h"

namespace css
{

class ItemPicker {
 public:
  ItemPicker(const PriestCharacter& c, std::string item_table_name);
  std::vector<Item> getBestItems() const;
  void CoutBestItems();
  void CoutCharacterStats() const;
  float getBestValue() const { return value(m_c_curr); }
 private:
  Item PickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name = "") const;
  float value(const PriestCharacter& c) const;

  std::map<std::string, Item> m_items;
  PriestCharacter m_c_curr;
};

}  // namespace css

// All in all could be: one slot at a time, pick best for slot. When no slot changed, we're good.
