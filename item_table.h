#pragma once

#include <map>
#include <vector>
#include <string>

#include "item.h"

namespace css
{

class ItemTable {
 public:
  ItemTable(std::string csv_file_name);
  std::vector<Item> getItems(const std::string& slot) const;
  std::vector<Item> getItems(const std::vector<std::string>& slots) const;
  std::vector<std::string> getItemSlots() const;

 private:
  void prepareSlotMap();
  void prepareColumnIndexes(const std::string& header_line);
  Item lineToItem(const std::string& line);

  std::vector<std::vector<Item>> m_items;
  std::map<std::string, int> m_slots;
  int m_slot_ix = -1;
  int m_name_ix = -1;
  int m_strength_ix = -1;
  int m_agility_ix = -1;
  int m_int_ix = -1;
  int m_stamina_ix = -1;
  int m_spirit_ix = -1;
  int m_sp_ix = -1;
  int m_sp_shadow_ix = -1;
  int m_mp5_ix = -1;
  int m_sp_healing_ix = -1;
};

}  // namespace css

