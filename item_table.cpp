#include "item_table.h"

#include <fstream>
#include <iostream>
#include <string>

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
    Item i = lineToItem(line);
    if (m_slots.find(i.slot) != m_slots.end()) {
      m_items[m_slots[i.slot]].push_back(i);
    }
  }
  m_items[m_slots["trinket 2"]] = m_items[m_slots["trinket"]];
  m_items[m_slots["finger 2"]] = m_items[m_slots["finger"]];
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
std::string wand_dps_name = "wand dps";
std::string wand_type_name = "wand type";

std::vector<std::string> SplitCsvLine(const std::string& line)
{
  char d = ',';
  int s = 0;
  int e = 0;
  int size = line.size();
  std::vector<std::string> splitted;
  while (e >= 0) {
    e = line.find(d, s);
    splitted.push_back(line.substr(s, e - s));
    s = e + 1;
    // std::cout << "splitted: |" << splitted.back() << "|, s:" << s << ", e: " << e << std::endl;
  }
  return splitted;
}

int IxFrom(const std::vector<std::string>& splitted, const std::string& name)
{
  int size = splitted.size();
  for (int i = 0; i < size; ++i) {
    if (splitted[i] == name) {
      return i;
    }
  }
  return -1;
}

float FloatWithCheck(const std::vector<std::string>& splitted, int ix)
{
  if (ix < 0 || ix >= splitted.size()) {
    return 0.0f;
  } else {
    return atof(splitted[ix].c_str());
  }
}

std::string StringWithCheck(const std::vector<std::string>& splitted, int ix)
{
  if (ix < 0 || ix >= splitted.size()) {
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
  return i;
}

}  // namespace css
