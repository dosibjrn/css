#pragma once

#include <map>
#include <set>
#include <string>

#include "item.h"

namespace css
{

class SetBonuses {
 public:
  SetBonuses(){}
  Item getTotalBonus() const { return m_total_bonus; } 

  void AddItem(const Item& i);
  void RemoveItem(const Item& i);

 private:
  Item m_total_bonus;
  std::set<std::string> m_bonus_names;
  std::map<std::string, std::set<std::string>> m_sets;
};

}  // namespace css
