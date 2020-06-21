#include "set_bonus_calculus.h"

#include <gtest/gtest.h>

namespace css {

class SetBonusCalculusTest : public testing::Test {
 protected:
  // Remember that SetUp() is run immediately before a test starts.
  // This is a good place to record the start time.
  void SetUp() override {
  }

  // TearDown() is invoked immediately after a test finishes.  Here we
  // check if the test was too slow.
  void TearDown() override {
  }

  ItemTable table = {"none.csv"};
  SetBonuses sb;
};

TEST_F(SetBonusCalculusTest, GTestWorks)
{
  ASSERT_EQ(1,1);
}

TEST_F(SetBonusCalculusTest, DummyDevout4pStatter4p) {
  // Construct dummy ItemTable
  // Add 4 shittiest "devout" to start with
  Item i;
  i.name = "devout head";
  i.sp = 20;
  i.slot = "head";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout shoulders";
  i.sp = 21;
  i.slot = "shoulders";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout chest";
  i.sp = 22;
  i.slot = "chest";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout wrists";
  i.sp = 23;
  i.slot = "wrists";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout hands";
  i.sp = 24;
  i.slot = "hands";
  table.AddItem(i);

  i.name = "devout waist";
  i.sp = 25;
  i.slot = "waist";
  table.AddItem(i);

  i.name = "devout legs";
  i.sp = 26;
  i.slot = "legs";
  table.AddItem(i);

  i.name = "devout feet";
  i.sp = 27;
  i.slot = "feet";
  table.AddItem(i);

  i.name = "statters head";
  i.sp = 30;
  i.slot = "head";
  table.AddItem(i);

  i.name = "statters shoulders";
  i.sp = 30;
  i.slot = "shoulders";
  table.AddItem(i);

  i.name = "statters chest";
  i.sp = 30;
  i.slot = "chest";
  table.AddItem(i);

  i.name = "statters wrists";
  i.sp = 30;
  i.slot = "wrists";
  table.AddItem(i);

  i.name = "statters hands";
  i.sp = 30;
  i.slot = "hands";
  table.AddItem(i);

  i.name = "statters waist";
  i.sp = 30;
  i.slot = "waist";
  table.AddItem(i);

  i.name = "statters legs";
  i.sp = 30;
  i.slot = "legs";
  table.AddItem(i);

  i.name = "statters feet";
  i.sp = 35;
  i.slot = "feet";
  table.AddItem(i);

  std::function<float(const std::vector<Item>&, std::vector<Item>*)> val_func = //
      [this](const std::vector<Item>& items_in, std::vector<Item>* more_new_items) {
        float sp_sum = 0.0f;
        int devout_count = 0;
        std::set<std::string> taken_slots;
        for (const Item& i : items_in) {
          sp_sum += i.sp;
          if (!getSetNames(i.name).empty()) {
            devout_count++;
            taken_slots.insert(i.slot);
          }
        }
        if (devout_count >= 4) {
          sp_sum += 23.0f;
        }
        // add other items
        more_new_items->clear();
        auto slots = table.getItemSlots();
        for (const auto& slot : slots) {
          if (taken_slots.find(slot) != taken_slots.end()) {
            continue;
          }
          auto items = table.getItems(slot);
          for (const auto& item : items) {
            if (getSetNames(item.name).empty()) {
              more_new_items->push_back(item);
              sp_sum += item.sp;
            }
          }
        }
        return sp_sum;
      };
 
  auto items = BestMatchingBonuses(table, val_func, sb);

  float sp_sum = 0.0f;
  int devout_count = 0;
  for (const Item& i : items) {
    sp_sum += i.sp;
    if (!getSetNames(i.name).empty()) {
      devout_count++;
    }
  }
  if (devout_count >= 4) {
    sp_sum += 23.0f;
  }

  // 3p 30 = 90; 35 = 125; 26,25,24,23 = 75.. 98, 125 + 98 = 223 + 23 = 246
  // devout: wrist hands waist legs
  ASSERT_EQ(sp_sum, 246.0f);

}

TEST_F(SetBonusCalculusTest, DummyDevout2pStatter1p) {
  // Construct dummy ItemTable
  // Add 4 shittiest "devout" to start with
  Item i;
  i.name = "devout head";
  i.sp = 20;
  i.slot = "head";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout shoulders";
  i.sp = 21;
  i.slot = "shoulders";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout chest";
  i.sp = 22;
  i.slot = "chest";
  table.AddItem(i);

  i.name = "statters head";
  i.sp = 30;
  i.slot = "head";
  table.AddItem(i);

  i.name = "statters shoulders";
  i.sp = 30;
  i.slot = "shoulders";
  table.AddItem(i);

  i.name = "statters chest";
  i.sp = 30;
  i.slot = "chest";
  table.AddItem(i);

  std::function<float(const std::vector<Item>&, std::vector<Item>*)> val_func = //
      [this](const std::vector<Item>& items_in, std::vector<Item>* more_new_items) {
        float sp_sum = 0.0f;
        int devout_count = 0;
        std::set<std::string> taken_slots;
        for (const Item& i : items_in) {
          sp_sum += i.sp;
          if (!getSetNames(i.name).empty()) {
            devout_count++;
            taken_slots.insert(i.slot);
          }
        }
        if (devout_count >= 2) {
          sp_sum += 23.0f;
        }
        // add other items
        more_new_items->clear();
        auto slots = table.getItemSlots();
        for (const auto& slot : slots) {
          if (taken_slots.find(slot) != taken_slots.end()) {
            continue;
          }
          auto items = table.getItems(slot);
          for (const auto& item : items) {
            if (getSetNames(item.name).empty()) {
              more_new_items->push_back(item);
              sp_sum += item.sp;
              break;
            }
          }
        }
        return sp_sum;
      };
 
  auto items = BestMatchingBonuses(table, val_func, sb);

  float sp_sum = 0.0f;
  int devout_count = 0;
  for (const Item& i : items) {
    sp_sum += i.sp;
    if (!getSetNames(i.name).empty()) {
      devout_count++;
    }
  }
  if (devout_count >= 2) {
    sp_sum += 23.0f;
  }

  // 30, 21, 22, 23 == 66 + 30 = 96
  ASSERT_EQ(sp_sum, 96.0f);

}

TEST_F(SetBonusCalculusTest, DummyDevout4pStatter4pAndHazz) {
  // Construct dummy ItemTable
  // Add 4 shittiest "devout" to start with
  Item i;
  i.name = "devout head";
  i.sp = 20;
  i.slot = "head";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout shoulders";
  i.sp = 21;
  i.slot = "shoulders";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout chest";
  i.sp = 22;
  i.slot = "chest";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout wrists";
  i.sp = 23;
  i.slot = "wrists";
  table.AddItem(i);
  sb.AddItem(i);

  i.name = "devout hands";
  i.sp = 24;
  i.slot = "hands";
  table.AddItem(i);

  i.name = "devout waist";
  i.sp = 25;
  i.slot = "waist";
  table.AddItem(i);

  i.name = "devout legs";
  i.sp = 26;
  i.slot = "legs";
  table.AddItem(i);

  i.name = "devout feet";
  i.sp = 27;
  i.slot = "feet";
  table.AddItem(i);

  i.name = "statters head";
  i.sp = 30;
  i.slot = "head";
  table.AddItem(i);

  i.name = "statters shoulders";
  i.sp = 30;
  i.slot = "shoulders";
  table.AddItem(i);

  i.name = "statters chest";
  i.sp = 30;
  i.slot = "chest";
  table.AddItem(i);

  i.name = "statters wrists";
  i.sp = 30;
  i.slot = "wrists";
  table.AddItem(i);

  i.name = "statters hands";
  i.sp = 30;
  i.slot = "hands";
  table.AddItem(i);

  i.name = "statters waist";
  i.sp = 30;
  i.slot = "waist";
  table.AddItem(i);

  i.name = "statters legs";
  i.sp = 30;
  i.slot = "legs";
  table.AddItem(i);

  i.name = "statters feet";
  i.sp = 35;
  i.slot = "feet";
  table.AddItem(i);

  i.name = "hazza'rah's charm of healing";
  i.sp = 20;
  i.slot = "trinket";
  table.AddItem(i);
  sb.AddItem(i);

  std::function<float(const std::vector<Item>&, std::vector<Item>*)> val_func = //
      [this](const std::vector<Item>& items_in, std::vector<Item>* more_new_items) {
        float sp_sum = 0.0f;
        int devout_count = 0;
        std::set<std::string> taken_slots;
        for (const Item& i : items_in) {
          sp_sum += i.sp;
          if (!getSetNames(i.name).empty()) {
            devout_count++;
            taken_slots.insert(i.slot);
          }
        }
        if (devout_count >= 4) {
          sp_sum += 23.0f;
        }
        // add other items
        more_new_items->clear();
        auto slots = table.getItemSlots();
        for (const auto& slot : slots) {
          if (taken_slots.find(slot) != taken_slots.end()) {
            continue;
          }
          auto items = table.getItems(slot);
          for (const auto& item : items) {
            if (getSetNames(item.name).empty()) {
              more_new_items->push_back(item);
              sp_sum += item.sp;
            }
          }
        }
        return sp_sum;
      };
 
  auto items = BestMatchingBonuses(table, val_func, sb);

  float sp_sum = 0.0f;
  int devout_count = 0;
  for (const Item& i : items) {
    sp_sum += i.sp;
    if (!getSetNames(i.name).empty()) {
      devout_count++;
    }
  }
  if (devout_count >= 4) {
    sp_sum += 23.0f;
  }

  // 3p 30 = 90; 35 = 125; 26,25,24,23 = 75.. 98, 125 + 98 = 223 + 23 = 246
  // devout: wrist hands waist legs
  ASSERT_EQ(sp_sum, 246.0f);

}

}  // namespace css
