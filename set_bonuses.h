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
  Item getTotalBonus() const { return m_total_bonus; } 

  void SetPartial(bool b);

  void AddItem(const Item& i);
  void RemoveItem(const Item& i);

 private:
  SetBonusListType toPartial(SetBonusListType& bonus_list);
  SetBonusListType getSetBonusList(bool partial);
  Item m_total_bonus;
  std::set<std::string> m_bonus_names;
  std::map<std::string, std::set<std::string>> m_sets;

  SetBonusListType m_bonus_list;
};

}  // namespace css
