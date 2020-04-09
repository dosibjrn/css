#include "set_bonuses.h"

#include <iostream>
#include <sstream>

#include "assumptions.h"
#include "item_operations.h"
#include "priest_character.h"

namespace css
{

namespace {
bool PrefixMatches(const std::string& prefix, const std::string& item_name)
{
  int prefix_len = static_cast<int>(prefix.size());
  if (item_name.substr(0, prefix_len) == prefix) {
    return true;
  } else {
    return false;
  }
}

bool SuffixMatches(const std::string& suffix, const std::string& item_name) {
  int s = static_cast<int>(item_name.size());
  int suffix_len = static_cast<int>(suffix.size());
  if (s > suffix_len && item_name.substr(s - suffix_len) == suffix) {
    return true;
  } else {
    return false;
  }
}

// Some belong to two sets due to special effects modelled as set bonuses and being in a set
// the all-seeing eye of zuldazar, I am looking at you and judging by your name, you're looking back
std::vector<std::string> getSetNames(const std::string& item_name)
{
  // Prefix sets
  if (PrefixMatches("devout", item_name)) return {"devout"};
  if (PrefixMatches("dreadm", item_name)) return {"dreadmist"};
  if (PrefixMatches("necrop", item_name)) return {"necropile"};
  if (PrefixMatches("zanzil", item_name)) return {"zanzil"};
  if (PrefixMatches("zandalar confessor's", item_name)) return {"confessor's"};
  if (PrefixMatches("primalist's", item_name)) return {"primalist"};

  // Suffix sets
  if (SuffixMatches("transcendence", item_name)) return {"transcendence"};
  if (SuffixMatches("prophecy", item_name)) return {"prophecy"};

  // "Special" sets
  if (item_name == global::assumptions.darkmoon_card_name) return {"darkmoon"};
  if (item_name == "the all-seeing eye of zuldazar") return {"confessor's", "all-seeing"};
  if (item_name == "haxx'rah's charm of healing") return {"confessor's", "hazz'rah's"};

  return {};
}
}  // namespace

SetBonusListType SetBonuses::getSetBonusList()
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
  { 
    Item i;
    i.name = "transcendence 3";
    bonus_list[i.name] = i;
    i.name = "transcendence 8";
    bonus_list[i.name] = i;
  }
  { 
    Item i;
    i.name = "prophecy 3";
    bonus_list[i.name] = i;
    i.name = "prophecy 5";
    i.spell_crit = 2.0f;
    bonus_list[i.name] = i;
  }
  {
    Item i;
    i.name = "darkmoon 1";
    bonus_list[i.name] = i;
  }
  { 
    Item i;
    i.name = "confessor's 2";
    i.sp = 23;
    bonus_list[i.name] = i;
    i.name = "hazz'ra's 1";
    i.sp = 0;
    bonus_list[i.name] = i;
    i.name = "all-seeing 1";
    bonus_list[i.name] = i;
  }
  { 
    Item i;
    i.name = "primalist 2";
    i.sp_healing = 33;
  }
  { 
    Item i;
    i.name = "zanzil 2";
    i.sp = 6;
    i.spell_hit = 1;
  }

  return bonus_list;
}

SetBonuses::SetBonuses() {
  m_bonus_list = getSetBonusList();
  m_bonus_list_partial = toPartial(m_bonus_list);
}


void SetBonuses::SetPartialAndUpdateCharacter(bool b, PriestCharacter *c)
{
  if (m_partial == b) {
    return;
  } else if (m_partial) {
    // remove partial, add real
    css::RemoveItem(m_total_bonus_partial, c);
    css::AddItem(m_total_bonus, c);
  } else if (!m_partial) {
    // remove real, add partial
    css::RemoveItem(m_total_bonus, c);
    css::AddItem(m_total_bonus_partial, c);
  }
  m_partial = b;
}

SetBonusListType SetBonuses::toPartial(SetBonusListType& bonus_list)
{
  SetBonusListType partial_bonus_list;
  for (auto& entry : bonus_list) {
    std::string set_name = entry.first;
    size_t space_pos = set_name.find(" ");
    int n = atoi(set_name.substr(space_pos).c_str());
    set_name = set_name.substr(0, space_pos);
    // std::cout << "set_name: " << set_name << ", n: " << n << std::endl;
    Item to_split = entry.second;
    float w_sum = 0.0f;
    for (int i = 1; i <= n; ++i) {
      // w_sum += i*i;
      w_sum += i;
    }
    for (int i = 1; i <= n; ++i) {
      Item partial_item;
      AddToItemWithMul(to_split, (i)/w_sum, &partial_item);
      std::stringstream ss;
      ss << set_name << " " << i;
      std::string splitted_name = ss.str();
      if (partial_bonus_list.find(splitted_name) == partial_bonus_list.end()) {
        partial_item.name = splitted_name;
        partial_bonus_list[splitted_name] = partial_item;
        // std::cout << "partial bonus: " << partial_item.name << std::endl;
      } else {
        AddToItemWithMul(partial_item, 1.0f, &partial_bonus_list[splitted_name]);
      }
    }
  }
  return partial_bonus_list;
}

void SetBonuses::AddItem(const Item& item)
{
  addItem(item, m_bonus_list, &m_total_bonus, &m_bonus_names, &m_sets);
  addItem(item, m_bonus_list_partial, &m_total_bonus_partial, &m_bonus_names_partial, &m_sets_partial);
}

void SetBonuses::addItem(const Item& item, const SetBonusListType& bonus_list, Item *total_bonus, std::set<std::string>* bonus_names,
                         std::map<std::string, std::set<std::string>>* sets)
{
  for (auto set_name : getSetNames(item.name)) {
    bool verbose = false;
    if (verbose) std::cout << "adding " << item.name << " with set name: " << set_name << std::endl;
    if (sets->find(set_name) == sets->end()) {
      std::set<std::string> s;
      (*sets)[set_name] = s;
    }
    (*sets)[set_name].insert(item.name);
    int items_of_set = static_cast<int>((*sets)[set_name].size());
    std::stringstream ss;
    ss << set_name << " " << items_of_set;
    std::string bonus_name = ss.str();
    if (verbose) std::cout << "items_of_set: " << items_of_set << ", bonus_name: " << bonus_name << std::endl;
    if (bonus_list.find(bonus_name) != bonus_list.end()) {
      Item bonus = bonus_list.at(bonus_name);
      bonus_names->insert(bonus_name);
      ss.str("");
      for (auto t_bonus_name : (*bonus_names)) {
        ss << t_bonus_name << " ";
      }
      total_bonus->slot = "Set bonuses";
      total_bonus->name = ss.str();
      if (verbose) CoutItem(bonus);
      AddToItemWithMul(bonus, 1.0f, total_bonus);
    }
  }
}
 

void SetBonuses::RemoveItem(const Item& item)
{
  removeItem(item, m_bonus_list, &m_total_bonus, &m_bonus_names, &m_sets);
  removeItem(item, m_bonus_list_partial, &m_total_bonus_partial, &m_bonus_names_partial, &m_sets_partial);
}

void SetBonuses::removeItem(const Item& item, const SetBonusListType& bonus_list, Item *total_bonus, std::set<std::string>* bonus_names,
                            std::map<std::string, std::set<std::string>>* sets)
{
  for (auto set_name : getSetNames(item.name) ) {
    auto set_it = sets->find(set_name);
    if (set_it == sets->end()) {
      std::cout << "!!!! Trying to remove from non existing set: " << set_name << "?? come on." << std::endl;
      return;
    }

    auto item_it = (*sets)[set_name].find(item.name);
    if (item_it == (*sets)[set_name].end()) {
      std::cout << "!!!! Trying to remove non existing item: " << item.name << " from set: " << set_name << "??? come on." << std::endl;
      return;
    }
    int items_of_set = static_cast<int>((*sets)[set_name].size());
    (*sets)[set_name].erase(item_it);
    std::stringstream ss;
    ss << set_name << " " << items_of_set;
    std::string bonus_name = ss.str();
    if (bonus_list.find(bonus_name) != bonus_list.end()) {
      Item bonus = bonus_list.at(bonus_name);
      auto bonus_name_it = bonus_names->find(bonus_name);
      if (bonus_name_it == bonus_names->end()) {
        std::cout << "!!!! Trying to remove non existing bonus name??? come on." << std::endl;
        return;
      }
      bonus_names->erase(bonus_name_it);
      ss.str("");
      for (auto t_bonus_name : (*bonus_names)) {
        ss << t_bonus_name << " ";
      }
      total_bonus->name = ss.str();
      AddToItemWithMul(bonus, -1.0f, total_bonus);
    }
  }
}

}  // namespace css
