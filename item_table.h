#pragma once

#include <vector>
#include <string>

#include "item.h"

namespace css
{

public class ItemTable {
 public:
  ItemTable(std::string csv_file_name);
  std::vector<Item> items(std::string slot);

 private:
  void prepareColumnIndexes();
  int strengthColumnIndex();
  int agilityColumnIndex();
  int intColumnIndex();
  int staminaColumnIndex();
  int spiritColumnIndex();
  int spColumnIndex();
  int spShadowColumnIndex();
  int spHealingColumnINdex();

  std::vector<std::vector<Item>> m_items;
};

}  // namespace css

