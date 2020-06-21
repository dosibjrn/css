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
  std::vector<Item> getSetItems(const std::string& set_name) const;
  std::vector<std::string> getSetItemSlots(const std::string& set_name) const;
  std::vector<Item> getItems(const std::string& slot) const;
  std::vector<Item> getItems(const std::vector<std::string>& slots) const;
  std::vector<std::string> getItemSlots() const;

  std::string nameToSlot(const std::string& name) const;

  void removeItem(const std::string& name);

 private:
  void prepareSlotMap();
  void prepareColumnIndexes(const std::string& header_line);
  Item lineToItem(const std::string& line);

  std::vector<std::vector<Item>> m_items;
  std::map<std::string, std::vector<Item>> m_set_items;
  std::map<std::string, int> m_slots;
  std::map<std::string, std::string> m_name_to_slot;
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
  int m_spell_crit_ix = -1;
  int m_spell_hit_ix = -1;

  int m_arcane_res_ix = -1;
  int m_nature_res_ix = -1;
  int m_fire_res_ix = -1;
  int m_frost_res_ix = -1;
  int m_shadow_res_ix = -1;

  int m_armor_ix = -1;
  int m_defense_ix = -1;
  int m_dodge_ix = -1;
  int m_parry_ix = -1;

  int m_source_ix = -1;
};

}  // namespace css

