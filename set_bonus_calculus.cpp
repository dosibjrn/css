#include "set_bonus_calculus.h"

#include <exception>
#include <iostream>
#include <sstream>

namespace css
{

namespace {

bool SomethingOver(int max_num, const std::vector<int>& nums) {
  for (const int& num : nums) {
    if (num > max_num) return true;
  }
  return false;
}

// returns false if can't
bool Increase(int max_num, std::vector<int>* nums)
{
  // increase last by one
  nums->back()++;

  const int size = static_cast<int>(nums->size());
  while (SomethingOver(max_num, *nums)) {
    for (int i = size - 1; i > 0; --i) {
      // if a number is over max_num -> set to 0 and previous to +1
      if (nums->at(i) > max_num) {
        nums->at(i - 1)++;
        // then set numbers after that to increase by 1
        for (int j = i; j < size; ++j) {
          nums->at(j) = nums->at(j - 1) + 1;
        }
      }
    }
    if (nums->front() > max_num) {
      return false;
    }
  }
  return true;
}

std::vector<std::vector<int>> UniqueNumberSets(int max_num, int num_nums)
{
  std::vector<std::vector<int>> out;
  std::vector<int> current;
  // construct first
  for (int i = 0; i < num_nums; ++i) {
    current.push_back(i);
  }
  out.push_back(current);
  while (Increase(max_num, &current)) {
    out.push_back(current);
  }

  constexpr bool debug = false;
  if (debug) {
    std::cout << "with max_num: " << max_num << ", num_nums: " << num_nums << " got index sets:" << std::endl;
    for (const auto& index_set : out) {
      std::cout << "    ";
      for (int ix : index_set) {
        std::cout << ix << " ";
      }
      std::cout << std::endl;
    }
  }


  return out;
}

void ToOptionsForSet(const std::vector<Item>& set_items, std::vector<std::vector<int>>& index_sets,
                     std::vector<std::vector<Item>>* options_for_set)
{
  for (const auto& index_set : index_sets) {
    std::vector<Item> item_set;
    for (int ix : index_set) {
      item_set.push_back(set_items[ix]);
    }
    options_for_set->push_back(item_set);
  }
}

int CountTrinketsAndRings(const std::vector<Item>& items) {
  int count = 0;
  for (const auto& item : items) {
    if (item.slot == "finger" || item.slot == "finger 2" || item.slot == "trinket" || item.slot == "trinket 2") {
      count++;
    }
  }
  return count;
}

std::vector<std::vector<Item>> AllNumberingsForTrinketsAndRings(const std::vector<Item>& set_items) {
  std::vector<std::vector<Item>> out;
  // ok, there are more...
  // TODO better
  out.push_back(set_items);
  std::set<std::string> taken;
  for (auto& item : out.back()) {
    if (taken.find(item.slot) != taken.end()) {
      item.slot = item.slot + " 2";
    }
    taken.insert(item.slot);
  }

  // swap fingers

  // swap trinkets
  return out;
}

std::vector<std::vector<Item>> AllUniqueSetsOf(const std::vector<Item>& set_items, int n)
{
  std::vector<std::vector<Item>> options_for_set;
  int set_size = static_cast<int>(set_items.size());
  auto index_sets = UniqueNumberSets(set_size - 1, n);

  if (CountTrinketsAndRings(set_items) != 0) {
    // finger, finger 2, trinket, trinket 2 -> this is a bit special
    auto all_numberings = AllNumberingsForTrinketsAndRings(set_items);
    std::vector<std::vector<Item>> joined;
    for (auto set_items_numbered : all_numberings) {
      ToOptionsForSet(set_items_numbered, index_sets, &options_for_set);
    }
  } else {
    ToOptionsForSet(set_items, index_sets, &options_for_set);
  }
  return options_for_set;
}

bool OverlappingSlots(const std::vector<Item>& a, const std::vector<Item>& b) {
  for (const Item& a_item : a) {
    for (const Item& b_item : b) {
      if ((a_item.slot == b_item.slot) && (a_item.name != b_item.name)) {
        return true;
      }
    }
  }
  return false;
}

std::vector<std::vector<Item>> AllMatchingBonuses(const ItemTable& table, const SetBonuses& sb)
{
  
  std::stringstream ss;
  constexpr bool debug = false;
  std::vector<std::vector<std::vector<Item>>> options_for_sets; // sets are indexed first, for each set, we have a number of options of size matching its num
  // pick a set bonus, get all for that, combine all of the
  auto bonus_names = sb.getBonusNames();

  int number_of_sets = static_cast<int>(bonus_names.size());

  ss << "initial number of sets: " << number_of_sets << std::endl;

  // populate the options_for_sets to contain all options for each individual set
  std::set<std::string> sets;
  for (auto name : bonus_names) {
    auto res = splitBonusName(name);
    std::string set_name = res.first; 
    sets.insert(set_name);
  }

  for (auto set_name : sets) {
    int n = sb.NumPieces(set_name);
    if (n > 1) {
      auto set_items = table.getSetItems(set_name);
      options_for_sets.push_back(AllUniqueSetsOf(set_items, n));
      ss << "  # options for set: " << set_name << ", n: " << n << " : " << options_for_sets.back().size() << std::endl;
    }
  }

  // do all legal combinations from options_for_sets == no overlapping slots
  // pick first, take next, continue if failed in that level of loop. these can not be real levels of loops tho

  std::vector<std::vector<Item>> legal_combos;
  std::vector<std::vector<Item>> legal_combos_next;

  number_of_sets = static_cast<int>(options_for_sets.size());
  ss << "non-duplicate number of sets: " << number_of_sets << std::endl;
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

      }
    }
    if (set_number != 0) {
      ss << "set_number: " << set_number << "/" << number_of_sets << ", swapping " << legal_combos_next.size() << " legal combos from prev of: " << legal_combos.size() << std::endl;
      if (legal_combos_next.empty()) {
        std::cout << ss.str();
        std::cout << "!!!!! NOTICED AN EMPTY SET; SHOULD NOT BE POSSIBLE !!!!" << std::endl;
      }
      std::swap(legal_combos, legal_combos_next);
      legal_combos_next.clear();
    }
  }
  ss << "Found total of: " << legal_combos.size() << " legal combos." << std::endl;
  if (debug) std::cout << ss.str();
  return legal_combos;
}

}  // namespace

std::vector<Item> BestMatchingBonuses(const ItemTable& table, 
                                      const std::function<float(const std::vector<Item>&)>& val_func, 
                                      const SetBonuses& sb)
{
  const auto options = AllMatchingBonuses(table, sb);
  float best_val = 0.0f;
  int best_ix = 0;

  int i = 0;
  constexpr bool debug = false;
  for (const auto& option : options) {
    float val = val_func(option);
    if (debug) {
      std::cout << "With items: ";
      for (const Item& item : option) {
        std::cout << item.name << " ";
      }
      std::cout << " got val: " << val << std::endl;
    }
    if (val > best_val) {
      best_val = val;
      best_ix = i;
    }
    i++;
  }
  return options.empty() ? std::vector<Item>{} : options[best_ix];
}

}  // namespace css
