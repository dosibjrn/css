#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "item.h"

namespace css
{

typedef std::map<std::string, Item> SetBonusListType;

struct PriestCharacter;

std::vector<std::string> getSetNames(const std::string& item_name);

class SetBonuses {
 public:
  SetBonuses();
  bool HasBonus(const std::string& bonus_name) const;
  int NumPieces(const std::string& set_name) const;

  Item getTotalBonus() const { if (m_partial) { return m_total_bonus_partial; } else { return m_total_bonus; } } 

  bool getPartial() const { return m_partial; }
  void SetPartialAndUpdateCharacter(bool b, PriestCharacter *c);

  void AddItem(const Item& item);
  void RemoveItem(const Item& item);

  std::set<std::string> getBonusNames() const {
    return m_partial ? m_bonus_names_partial : m_bonus_names;
  }

 private:
  void addItem(const Item& item, const SetBonusListType& bonus_list, Item *total_bonus, std::set<std::string>* bonus_names,
               std::map<std::string, std::set<std::string>>* sets);
  void removeItem(const Item& item, const SetBonusListType& bonus_list, Item *total_bonus, std::set<std::string>* bonus_names,
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

std::pair<std::string, int> splitBonusName(const std::string& set_name);

}  // namespace css
