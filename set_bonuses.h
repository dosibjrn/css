#pragma once

#include <map>
#include <set>
#include <string>

#include "item.h"

namespace css
{

typedef std::map<std::string, Item> SetBonusListType;

class SetBonuses {
 public:
  SetBonuses();
  Item getTotalBonus() const { if (m_partial) { return m_total_bonus_partial; } else { return m_total_bonus; } } 

  void SetPartial(bool b) { m_partial = b; }

  void AddItem(const Item& i);
  void RemoveItem(const Item& i);

 private:
  void addItem(const Item& i, const SetBonusListType& bonus_list, Item *total_bonus, std::set<std::string>* bonus_names,
               std::map<std::string, std::set<std::string>>* sets);
  void removeItem(const Item& i, const SetBonusListType& bonus_list, Item *total_bonus, std::set<std::string>* bonus_names,
               std::map<std::string, std::set<std::string>>* sets);

  SetBonusListType toPartial(SetBonusListType& bonus_list);
  SetBonusListType getSetBonusList();

  bool m_partial = true;

  // Actual set bonuses
  Item m_total_bonus;
  std::set<std::string> m_bonus_names;
  std::map<std::string, std::set<std::string>> m_sets;
  SetBonusListType m_bonus_list;

  // Parital/splitted bonuses helping early optimization
  Item m_total_bonus_partial;
  std::set<std::string> m_bonus_names_partial;
  std::map<std::string, std::set<std::string>> m_sets_partial;

  SetBonusListType m_bonus_list_partial;
};

}  // namespace css
