#include "set_bonuses.h"

#include <iostream>
#include <sstream>

#include "item_operations.h"

namespace css
{

namespace {

typedef std::map<std::string, Item> SetBonusListType;

SetBonusListType getSetBonusList()
{
  SetBonusListType bonus_list;
  bool add_fake_devout = true;
  if (add_fake_devout) {
    {
      Item i;
      i.sp = 23*0.1;
      i.name = "devout 1";
      bonus_list[i.name] = i;
    }

    {
      Item i;
      i.sp = 23*0.1;
      i.name = "devout 2";
      bonus_list[i.name] = i;
    }

    {
      Item i;
      i.sp = 23*0.1;
      i.name = "devout 3";
      bonus_list[i.name] = i;
    }
    {
      Item i;
      i.sp = 23*0.7;
      i.name = "devout 4";
      bonus_list[i.name] = i;
    }
  } else {
    Item i;
    i.sp = 23;
    i.name = "devout 4";
    bonus_list[i.name] = i;
  }
  return bonus_list;
}

namespace globals {
  SetBonusListType bonus_list = getSetBonusList();
}  // namespace globals

std::string getSetName(const std::string& item_name)
{
  if (item_name.substr(0, 6) == "devout") { return "devout"; }
  return "";
}
}  // namespace

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
  if (globals::bonus_list.find(bonus_name) != globals::bonus_list.end()) {
    Item bonus = globals::bonus_list[bonus_name];
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
    std::cout << "!!!! Trying to remove from non existing set?? come on." << std::endl;
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
  if (globals::bonus_list.find(bonus_name) != globals::bonus_list.end()) {
    Item bonus = globals::bonus_list[bonus_name];
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
