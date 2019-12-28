#include "set_bonuses.h"

#include <iostream>
#include <sstream>

#include "item_operations.h"

namespace css
{

namespace {
std::string getSetName(const std::string& item_name)
{
  if (item_name.substr(0, 6) == "devout") { return "devout"; }
  if (item_name.substr(0, 6) == "dreadm") { return "dreadmist"; }
  if (item_name.substr(0, 6) == "necrop") { return "necropile"; }
  return "";
}
}  // namespace

SetBonusListType SetBonuses::getSetBonusList(bool partial)
{
  SetBonusListType bonus_list;
  {
    Item i;
    i.defense = 3;
    i.name = "necropile 2";
    bonus_list[i.name] = i;
  }
  {
    Item i;
    i.intelligence = 5;
    i.name =  "necropile 3";
    bonus_list[i.name] = i;
  }
  {
    Item i;
    i.arcane_res = 15;
    i.nature_res = 15;
    i.fire_res = 15;
    i.frost_res = 15;
    i.shadow_res = 15;
    i.name = "necropile 4";
    bonus_list[i.name] = i;
  }
  {
    Item i;
    i.armor = 200;
    i.name = "devout 2";
    bonus_list[i.name] = i;
    i.name = "dreadmist 2";
    bonus_list[i.name] = i;
  }
  {
    Item i;
    i.sp = 23;
    i.name = "devout 4";
    bonus_list[i.name] = i;
    i.name = "dreadmist 4";
    bonus_list[i.name] = i;
    i.name = "necropilt 5";
    bonus_list[i.name] = i;
  }

  if (partial) {
    return toPartial(bonus_list);
  }
  return bonus_list;
}

SetBonuses::SetBonuses() {
  m_bonus_list = getSetBonusList(true);
}

SetBonusListType SetBonuses::toPartial(SetBonusListType& bonus_list)
{
  SetBonusListType partial_bonus_list;
  for (auto& entry : bonus_list) {
    std::string set_name = entry.first;
    size_t space_pos = set_name.find(" ");
    int n = atoi(set_name.substr(space_pos).c_str());
    set_name = set_name.substr(0, space_pos);
    Item to_split = entry.second;
    Item partial_item;
    AddToItemWithMul(to_split, 1.0f/n, &partial_item);
    for (int i = 1; i <= n; ++i) {
      std::stringstream ss;
      ss << set_name << " " << i;
      std::string splitted_name = ss.str();
      if (partial_bonus_list.find(splitted_name) == partial_bonus_list.end()) {
        partial_bonus_list[splitted_name] = partial_item;
      } else {
        AddToItemWithMul(partial_item, 1.0f, &partial_bonus_list[splitted_name]);
      }
    }
  }
  return partial_bonus_list;
}

void SetBonuses::AddItem(const Item& item)
{
  std::string set_name = getSetName(item.name);
  if (set_name == "") {
    return;
  }
  bool verbose = false;
  if (verbose) std::cout << "adding " << item.name << " with set name: " << set_name << std::endl;
  if (m_sets.find(set_name) == m_sets.end()) {
    std::set<std::string> s;
    m_sets[set_name] = s;
  }
  m_sets[set_name].insert(item.name);
  int items_of_set = m_sets[set_name].size();
  std::stringstream ss;
  ss << set_name << " " << items_of_set;
  std::string bonus_name = ss.str();
  if (verbose) std::cout << "items_of_set: " << items_of_set << ", bonus_name: " << bonus_name << std::endl;
  if (m_bonus_list.find(bonus_name) != m_bonus_list.end()) {
    Item bonus = m_bonus_list[bonus_name];
    m_bonus_names.insert(bonus_name);
    ss.str("");
    for (auto t_bonus_name : m_bonus_names) {
      ss << t_bonus_name << " ";
    }
    m_total_bonus.name = ss.str();
    if (verbose) CoutItem(bonus);
    AddToItemWithMul(bonus, 1.0f, &m_total_bonus);
  }
}

void SetBonuses::RemoveItem(const Item& item)
{
  std::string set_name = getSetName(item.name);
  if (set_name == "") {
    return;
  }
  auto set_it = m_sets.find(set_name);
  if (set_it == m_sets.end()) {
    std::cout << "!!!! Trying to remove from non existing set: " << set_name << "?? come on." << std::endl;
    return;
  }

  auto item_it = m_sets[set_name].find(item.name);
  if (item_it == m_sets[set_name].end()) {
    std::cout << "!!!! Trying to remove non existing item??? come on." << std::endl;
    return;
  }
  int items_of_set = m_sets[set_name].size();
  m_sets[set_name].erase(item_it);
  std::stringstream ss;
  ss << set_name << " " << items_of_set;
  std::string bonus_name = ss.str();
  if (m_bonus_list.find(bonus_name) != m_bonus_list.end()) {
    Item bonus = m_bonus_list[bonus_name];
    auto bonus_name_it = m_bonus_names.find(bonus_name);
    if (bonus_name_it == m_bonus_names.end()) {
      std::cout << "!!!! Trying to remove non existing bonus name??? come on." << std::endl;
      return;
    }
    m_bonus_names.erase(bonus_name_it);
    ss.str("");
    for (auto t_bonus_name : m_bonus_names) {
      ss << t_bonus_name << " ";
    }
    m_total_bonus.name = ss.str();
    AddToItemWithMul(bonus, -1.0f, &m_total_bonus);
  }
}

}  // namespace css
