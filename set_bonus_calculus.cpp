#include "set_bonus_calculus.h"

namespace css
{

namespace {

std::vector<std::vector<Item>> AllUniqueSetsOf(const std::vector<Item>& set_items, int n)
{
  std::vector<std::vector<Item>> options_for_set;
  int set_size = static_cast<int>(set_items.size());
  std::vector<int> picks(n);
  while (picks[0] < set_size) {
    int prev = picks[0];
    bool good_to_add = false;
    for (int ix = 1; ix < n; ++ix) {
      auto& pick = picks[ix];
      if (pick <= prev) {
        if (pick == set_size) {
          pick = 0;
          picks[ix - 1]++;
        }
        ++pick;
        prev = pick;
        good_to_add = false;
      }
      if (good_to_add) {
        options_for_set.push_back(std::vector<Item>);
        for (auto pick : picks) {
          options.for_set.back(push_back(set_items[pick]));
        }
        picks[n - 1]++;
      }
    }
  }
  return options_for_set;
}

bool OverlappingSlots(const std::vector<Item>& a, const std::vector<Item>& b) {
  for (const auto& a_entry : a) {
    for (const auto& b_entry : b) {
      if (a_entry.slot == b_entry.slot) {
        return true;
      }
    }
  }
  return false;
}

std::vector<std::vector<Item>> AllMatchingBonuses(const ItemTable& table, const SetBonuses& sb)
{
  
  std::vector<std::vector<std::vector<Item>>> options_for_sets; // sets are indexed first, for each set, we have a number of options of size matching its num
  // pick a set bonus, get all for that, combine all of the
  auto bonus_names = sb.getBonusNames();

  int number_of_sets = static_cast<int>(bonus_names.size());

  // populate the options_for_sets to contain all options for each individual set
  int set_ix = 0;
  for (auto name : bonus_names) {
    auto res = splitBonusName(name);
    int n = res.second;
    std::string set_name = res.first; 
    auto set_items = table.getSetItems(set_name);
    options_for_sets.push_back(AllUniqueSetsOf(set_items, n));
  }

  // do all legal combinations from options_for_sets == no overlapping slots
  // pick first, take next, continue if failed in that level of loop. these can not be real levels of loops tho

  std::vector<std::vector<Item>> legal_combos;
  std::vector<std::vector<Item>> legal_combos_next;

  // add all with non overlapping slots to legal combos
  for (int set_number = 0; set_number < number_of_sets; ++set_number) {
    for (const auto& option : options_for_sets[set_number]) {
      if (set_number == 0) {
        legal_combos.push_back(option);
      } else {
        for (auto& legal_combo : legal_combos) {
          // if not overlapping with option, add option to legal_combos_next
          if (!OverlappingSlots(legal_combo, option)) {
            auto joined = legal_combo;
            joined.insert(joined.end(), option.begin(), option.end());
            legal_combos_next.push_back(joined);
          }
        }
        legal_combos = legal_combos_next;
        legal_combos_next.clear();
      }
    }
  }
  return legal_combos;
}

}  // namespace

std::vector<Item> BestMatchingBonuses(const ItemTable& table, 
                                      const std::function<float(const std::vector<Item>&)& val_func, 
                                      const SetBonuses& sb)
{
  const auto options = AllMatchingBonuses(table, sb);
  float best_val = 0.0f;
  int best_ix = 0;

  int i = 0;
  for (const auto& option : options) {
    float val = val_func(option);
    if (val > best_val) {
      best_val = val;
      best_ix = i;
    }
    i++;
  }
  return options.emtpy() ? std::vector<Item>{} : options[best_ix];
}

}  // namespace css
