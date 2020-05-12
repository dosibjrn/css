#pragma once

#include <functional>
#include <vector>

#include "item_table.h"
#include "set_bonuses.h"

namespace css
{

std::vector<Item> BestMatchingBonuses(const ItemTable& table, 
                                      const std::function<float(const std::vector<Item>&)>& val_func, 
                                      const SetBonuses& sb);

}  // namespace css

