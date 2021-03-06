#include "item_table.h"

#include <fstream>
#include <iostream>
#include <string>

#include "csv.h"
#include "set_bonuses.h"

namespace css
{



ItemTable::ItemTable(std::string csv_file_name)
{
  prepareSlotMap();
  std::ifstream is(csv_file_name.c_str());
  std::string header_line;
  if (std::getline(is, header_line)) {
    prepareColumnIndexes(header_line);
  }
  std::string line;
  m_items.resize(m_slots.size() + 2);
  while (std::getline(is, line)) {
    AddItem(lineToItem(line));
  }
}



std::vector<Item> ItemTable::getItems(const std::string& slot) const
{
  std::vector<Item> items;
  if (m_slots.find(slot) != m_slots.end()) {
    const auto& added = m_items.at(m_slots.at(slot));
    items.insert(items.begin(), added.begin(), added.end());
  }
  return items;
}

std::vector<Item> ItemTable::getItems(const std::vector<std::string>& slots) const
{
  std::vector<Item> items;
  for (auto slot : slots) {
    auto added = getItems(slot);
    items.insert(items.begin(), added.begin(), added.end());
  }
  return items;
}


std::vector<std::string> ItemTable::getItemSlots() const
{
  std::vector<std::string> slots;
  for (const auto entry : m_slots) {
    slots.push_back(entry.first);
  }
  return slots;
}

std::vector<Item> ItemTable::getSetItems(const std::string& set_name) const
{
  if (m_set_items.find(set_name) != m_set_items.end()) {
    return m_set_items.at(set_name);
  } else {
    return std::vector<Item>{};
  }
}


std::set<std::string> ItemTable::getSetItemSlots(const std::string& set_name) const
{
  std::set<std::string> slots;
  auto items = getSetItems(set_name);
  for (auto& item : items) {
    slots.insert(item.slot);
  }
  return slots;
}


std::string ItemTable::NameToSlot(const std::string& name) const
{ 
  if (m_name_to_slot.find(name) == m_name_to_slot.end()) {
    return "";
  } else {
    return m_name_to_slot.at(name);
  }
}

void ItemTable::AddItem(const Item& i)
{
  if (m_slots.find(i.slot) != m_slots.end()) {
    m_items[m_slots[i.slot]].push_back(i);
    if (i.slot == "trinket") {
      m_items[m_slots["trinket 2"]].push_back(i);
    }
    if (i.slot == "finger") {
      m_items[m_slots["finger 2"]].push_back(i);
    }
    m_name_to_slot[i.name] = i.slot;
    auto set_names = getSetNames(i.name);
    for (auto set_name : set_names) {
      m_set_items[set_name].push_back(i);
    }
  }
}

void ItemTable::RemoveItem(const std::string& name)
{
  auto slot = NameToSlot(name);
  if (slot == "") return;

  if (m_slots.find(slot) == m_slots.end()) return;

  int slot_ix = m_slots[slot];
  auto& items_for_slot = m_items[slot_ix];
  int n = static_cast<int>(items_for_slot.size());
  for (int i = 0; i < n; ++i) {
    if (items_for_slot[i].name == name) {
      items_for_slot.erase(items_for_slot.begin() + i);
      return;
    }
  }
  // TODO: this still leaves trinket 2, finger 2 and set items
}

void ItemTable::prepareSlotMap()
{
  int i = 0;
  m_slots["head"] = i++;
  m_slots["neck"] = i++;
  m_slots["shoulders"] = i++;
  m_slots["back"] = i++;
  m_slots["chest"] = i++;
  m_slots["wrists"] = i++;
  m_slots["two-hand"] = i++;
  m_slots["one-hand"] = i++;
  m_slots["main hand"] = i++;
  m_slots["off hand"] = i++;
  m_slots["ranged"] = i++;
  m_slots["hands"] = i++;
  m_slots["waist"] = i++;
  m_slots["legs"] = i++;
  m_slots["feet"] = i++;
  m_slots["finger"] = i++;
  m_slots["finger 2"] = i++;
  m_slots["trinket"] = i++;
  m_slots["trinket 2"] = i++;
}

namespace
{
// column names corresponding to csv, this supports adding new lines in between etc.
std::string slot_name = "slot";
std::string name_name = "name";

std::string strength_name = "str";
std::string agility_name = "agi";
std::string int_name = "int";
std::string stamina_name = "stam";
std::string spirit_name = "spirit";

std::string sp_name = "sp";
std::string sp_shadow_name = "sp_shadow";
std::string mp5_name = "mp5";
std::string sp_healing_name = "sp_healing";
std::string spell_crit_name = "spell crit";
std::string spell_hit_name = "spell hit";

std::string arcane_res_name = "arcane resistance";
std::string nature_res_name = "nature resistance";
std::string fire_res_name = "fire resistance";
std::string frost_res_name = "frost resistance";
std::string shadow_res_name = "shadow resistance";

std::string armor_name = "armor";
std::string defense_name = "defense";
std::string dodge_name = "dodge";
std::string parry_name = "parry";

std::string wand_dps_name = "wand dps";
std::string wand_type_name = "wand type";

std::string source_name = "source";

int IxFrom(const std::vector<std::string>& splitted, const std::string& name)
{
  int size = static_cast<int>(splitted.size());
  for (int i = 0; i < size; ++i) {
    if (splitted[i] == name) {
      return i;
    }
  }
  return -1;
}

float FloatWithCheck(const std::vector<std::string>& splitted, int ix)
{
  if (ix < 0 || ix >= static_cast<int>(splitted.size())) {
    return 0.0f;
  } else {
    return static_cast<float>(atof(splitted[ix].c_str()));
  }
}

std::string StringWithCheck(const std::vector<std::string>& splitted, int ix)
{
  if (ix < 0 || ix >= static_cast<int>(splitted.size())) {
    return "N/A";
  } else {
    return splitted[ix];
  }
}

}  // namespace

void ItemTable::prepareColumnIndexes(const std::string& header_line)
{
  auto splitted = SplitCsvLine(header_line);
  m_slot_ix = IxFrom(splitted, slot_name);
  m_name_ix = IxFrom(splitted, name_name);

  m_strength_ix = IxFrom(splitted, strength_name);
  m_agility_ix = IxFrom(splitted, agility_name);
  m_int_ix = IxFrom(splitted, int_name);
  m_stamina_ix = IxFrom(splitted, stamina_name);
  m_spirit_ix = IxFrom(splitted, spirit_name);

  m_sp_ix = IxFrom(splitted, sp_name);
  m_sp_shadow_ix = IxFrom(splitted, sp_shadow_name);
  m_mp5_ix = IxFrom(splitted, mp5_name);
  m_sp_healing_ix = IxFrom(splitted, sp_healing_name);
  m_spell_crit_ix = IxFrom(splitted, spell_crit_name);
  m_spell_hit_ix = IxFrom(splitted, spell_hit_name);

  m_arcane_res_ix = IxFrom(splitted, arcane_res_name);
  m_nature_res_ix = IxFrom(splitted, nature_res_name);
  m_fire_res_ix = IxFrom(splitted, fire_res_name);
  m_frost_res_ix = IxFrom(splitted, frost_res_name);
  m_shadow_res_ix = IxFrom(splitted, shadow_res_name);

  m_armor_ix = IxFrom(splitted, armor_name);
  m_defense_ix = IxFrom(splitted, defense_name);
  m_dodge_ix = IxFrom(splitted, dodge_name);
  m_parry_ix = IxFrom(splitted, parry_name);

  m_source_ix = IxFrom(splitted, source_name);
}

Item ItemTable::lineToItem(const std::string& line)
{
  auto splitted = SplitCsvLine(line);
  Item i;
  i.slot = StringWithCheck(splitted, m_slot_ix);
  i.name = StringWithCheck(splitted, m_name_ix);

  i.strength = FloatWithCheck(splitted, m_strength_ix);
  i.agility = FloatWithCheck(splitted, m_agility_ix);
  i.intelligence = FloatWithCheck(splitted, m_int_ix);
  i.stamina = FloatWithCheck(splitted, m_stamina_ix);
  i.spirit = FloatWithCheck(splitted, m_spirit_ix);

  i.sp = FloatWithCheck(splitted, m_sp_ix);
  i.sp_shadow = FloatWithCheck(splitted, m_sp_shadow_ix);
  i.mp5 = FloatWithCheck(splitted, m_mp5_ix);
  i.sp_healing = FloatWithCheck(splitted, m_sp_healing_ix);
  i.spell_crit = FloatWithCheck(splitted, m_spell_crit_ix);
  i.spell_hit = FloatWithCheck(splitted, m_spell_hit_ix);

  i.arcane_res = FloatWithCheck(splitted, m_arcane_res_ix);
  i.nature_res = FloatWithCheck(splitted, m_nature_res_ix);
  i.fire_res = FloatWithCheck(splitted, m_fire_res_ix);
  i.frost_res = FloatWithCheck(splitted, m_frost_res_ix);
  i.shadow_res = FloatWithCheck(splitted, m_shadow_res_ix);

  i.armor = FloatWithCheck(splitted, m_armor_ix);
  i.defense = FloatWithCheck(splitted, m_defense_ix);
  i.dodge = FloatWithCheck(splitted, m_dodge_ix);
  i.parry = FloatWithCheck(splitted, m_parry_ix);

  i.source = StringWithCheck(splitted, m_source_ix);
  return i;
}

}  // namespace css
