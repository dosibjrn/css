#include "set_bonus_calculus.h"

namespace css
{

namespace {

std::vector<std::vector<Item>> AllMatchingBonuses(const ItemTable& table, const SetBonuses& sb)
{
  std::vector<std::vector<Item>> out;
  
  std::vector<std::vector<std::vector<Item>>> options_for_sets; // sets are indexed first, for each set, we have a number of options of size matching its num
  // pick a set bonus, get all for that, combine all of the
  auto bonus_names = sb.getBonusNames();

  // populate the options_for_sets to contain all options for each individual set
  int set_ix = 0;
  for (auto name : bonus_names) {
    auto res = splitBonusName(name);
    int n = res.second;
    std::string set_name = res.first; 
    auto set_items = table.getSetItems(set_name);
    options_for_sets.push_back(std::vector<std::vector<Item>>);
    auto& options_for_set = options_for_sets.back();

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
  }

  // do all legal combinations from options_for_sets == no overlapping slots
  // pick first, take next, continue if failed in that level of loop. these can not be real levels of loops tho

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
