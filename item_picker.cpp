#include "item_picker.h"

#include <iostream>
#include <string>

#include "dps.h"
#include "item_table.h"
#include "stats.h"

namespace css
{

ItemPicker::ItemPicker(const PriestCharacter& c, std::string item_table_name)
{
  ItemTable item_table(item_table_name);
  bool static_for_all_slots = false;
  int max_iterations = 1000;
  m_c_curr = c;
  const std::vector<std::string> slots = item_table.getItemSlots();
  for (const auto& slot : slots) {
    Item i;
    i.slot = slot;
    m_items[slot] = i;
  }


  int iteration = 0;
  bool verbose = true;
  while (!static_for_all_slots && iteration < max_iterations) {
    static_for_all_slots = true;
    for (const auto& slot : slots) {
      // TODO handle off hand etc separately
      if (slot == "one-hand" || slot == "main hand" || slot == "off hand") {
        continue;
      }
      Item i_curr = m_items[slot];
      auto items_for_slot = item_table.getItems(slot);
      std::string taken = "";
      if (slot == "finger 2") {
        taken = m_items["finger"].name;
      } else if (slot == "trinket 2") {
        taken = m_items["trinket"].name;
      }
      
      Item i_best = PickBest(m_c_curr, i_curr, items_for_slot, taken);
      if (slot == "two-hand") {
        Item best_two_hand_item = i_best;
        PriestCharacter c_empty_hands = m_c_curr;
        removeItem(m_items["two-hand"], &c_empty_hands);
        removeItem(m_items["one-hand"], &c_empty_hands);
        removeItem(m_items["main hand"], &c_empty_hands);
        removeItem(m_items["off hand"], &c_empty_hands);

        Item no_item;
        std::vector<Item> main_hand_items = item_table.getItems("main hand");
        std::vector<Item> one_hand_items = item_table.getItems("one-hand");
        main_hand_items.insert(main_hand_items.begin(), one_hand_items.begin(), one_hand_items.end());
        Item best_main_hand_item = PickBest(c_empty_hands, no_item, main_hand_items, "");

        std::vector<Item> off_hand_items = item_table.getItems("off hand");
        Item best_off_hand_item = PickBest(c_empty_hands, no_item, off_hand_items, "");

        PriestCharacter c_main_and_off = c_empty_hands;
        addItem(best_off_hand_item, &c_main_and_off);
        addItem(best_main_hand_item, &c_main_and_off);
        float val_main_and_off = value(c_main_and_off);

        PriestCharacter c_two_hand = c_empty_hands;
        addItem(best_two_hand_item, &c_two_hand);
        float val_two_hand = value(c_two_hand);

        if (val_two_hand > val_main_and_off) {
          m_c_curr = c_two_hand;
          m_items["two-hand"] = best_two_hand_item;
          m_items["one-hand"] = no_item;
          m_items["main hand"] = no_item;
          m_items["off hand"] = no_item;
        } else {
          m_c_curr = c_main_and_off;
          m_items["two-hand"] = no_item;
          m_items["one-hand"] = no_item;
          m_items["main hand"] = best_main_hand_item;
          m_items["off hand"] = best_off_hand_item;
        }
      } else {
        if (verbose) {
          std::cout << " *********** " << std::endl;
          coutItem(i_curr);
          std::cout << " --- vs --- " << std::endl;
          coutItem(i_best);
          std::cout << "^^^^^^^^^^^^^" << std::endl;     
        }
        if (i_curr.name.substr(0, 4) != i_best.name.substr(0, 4)) {
          if (verbose) {
            float prev_val = value(m_c_curr);
            std::cout << slot << " : " << i_curr.name << " -> " << i_best.name << " => val: " << prev_val;
          }

          static_for_all_slots = false;
          m_items[slot] = i_best;
          removeItem(i_curr, &m_c_curr);
          addItem(i_best, &m_c_curr);
          if (verbose) {
            float curr_val = value(m_c_curr);
            std::cout << " -> " << curr_val << std::endl;
          }
        }
      }

    }
    iteration++;
  }
}

void ItemPicker::CoutBestItems()
{
  std::vector<std::string> order = {"head", "neck", "shoulders", "back", "chest", "wrists", "two-hand", "main hand", "one-hand", "off hand", "ranged",
    "hands", "waist", "legs", "feet", "finger", "finger 2", "trinket", "trinket 2"};
    // "hands", "waist", "legs", "feet", "finger", "trinket"};
  // for (auto map_entry : m_items) {
  for (std::string slot : order) {
    const Item &item = m_items[slot];
    std::cout << " ----------------------------" << std::endl;
    std::cout << " --- " << item.slot << " --- " << std::endl;
    coutItem(item);
  }
}

void ItemPicker::CoutCharacterStats() const
{
  Stats s(m_c_curr);
  s.CoutStats();
}

float ItemPicker::value(const PriestCharacter &c) const
{
  // Similar to what was done prev: dps*dps*ehp*emana
  float dps = ShadowDps(c);
  Stats s(c);
  float duration = 100.0f;  // s
  float fsr_frac = 2.0f/3.0f;
  float emana = s.getEffectiveMana(duration, fsr_frac);
  float ehp = s.getEffectiveHp();
  return dps*dps*emana*ehp/1e12;
}

Item ItemPicker::PickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name) const
{
  // create char without item
  PriestCharacter c_no_item = c;
  removeItem(current_item, &c_no_item);
  // for each item
  Item best_item = current_item;
  float best_value = value(c);
  for (const Item& item : items_for_slot) {
  // set char to state without item
    if (item.name == taken_name) {
      continue;
    }
    PriestCharacter c_tmp = c_no_item;
    // add effect of new item
    addItem(item, &c_tmp);
    // calculate objective function value
    float val = value(c_tmp);
    if (val > best_value) {
      best_value = val;
      best_item = item;
    }
  }
  return best_item;
}

}  // namespace css
