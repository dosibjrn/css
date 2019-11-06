#include "item_picker.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include "dps.h"
#include "item_table.h"
#include "stats.h"

namespace css
{

float ItemPicker::value(const PriestCharacter &c) const
{
  // Similar to what was done prev: dps*dps*ehp*emana
  float dps = ShadowDps(c);
  Stats s(c);
  float duration = 100.0f;  // s
  float fsr_frac = 2.0f/3.0f;
  // float duration = 30.0f;  // s
  // float fsr_frac = 2.5f/3.0f;
 
  float emana = s.getEffectiveMana(duration, fsr_frac);
  float ehp = s.getEffectiveHp();
  return dps*dps*emana*ehp/1e12;
}

template<typename T>
void Shuffle(std::vector<T>* v)
{
  int64_t s = v->size();
  std::vector<int64_t> ixs;
  for (int64_t i = 0; i < s; ++i) ixs.push_back(i); 

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(ixs.begin(), ixs.end(), std::default_random_engine(seed));
  std::vector<T> tmp(s);
  std::swap(tmp, *v);
  for (int64_t i = 0; i < s; ++i) {
    v->at(i) = tmp[ixs[i]];
  }
}

ItemPicker::ItemPicker(const PriestCharacter& c, std::string item_table_name)
  : m_c_in(c)
  , m_c_curr(c)
  , m_item_table_name(item_table_name)
{
  Recalculate();
}

void ItemPicker::Recalculate()
{
  m_c_curr = m_c_in;
  m_items.clear();
  ItemTable item_table(m_item_table_name);
  int static_for_all_slots = 0;
  int max_iterations = 1000;
  std::vector<std::string> slots = item_table.getItemSlots();
  for (const auto& slot : slots) {
    Item i;
    i.slot = slot;
    m_items[slot] = i;
  }

  float val_best = 0.0f;
  std::map<std::string, Item> items_best;
  PriestCharacter c_best;

  int iteration = 0;
  bool verbose = false;
  while (static_for_all_slots < 20 && iteration < max_iterations) {
    if (iteration % (max_iterations/50) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    static_for_all_slots++;
    Shuffle(&slots);
    // std::random_shuffle(slots.begin(), slots.end());
    int slot_ix = 0;
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
      } else if (slot == "finger") {
        taken = m_items["finger 2"].name;
      } else if (slot == "trinket") {
        taken = m_items["trinket 2"].name;
      } else if (iteration < max_iterations/2 
                 && (slot_ix % (iteration + 1 % slots.size()) == 0)) {
        taken = i_curr.name;
      }
      
      Item i_best = pickBest(m_c_curr, i_curr, items_for_slot, taken);
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
        if (iteration < max_iterations/2) {
          taken = m_items["main hand"].name;
        }
        Item best_main_hand_item = pickBest(c_empty_hands, no_item, main_hand_items, taken);

        std::vector<Item> off_hand_items = item_table.getItems("off hand");
        if (iteration < max_iterations/2) {
          taken = m_items["off hand"].name;
        }
 
        Item best_off_hand_item = pickBest(c_empty_hands, no_item, off_hand_items, taken);

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
        // if (verbose) {
          // std::cout << " *********** " << std::endl;
          // coutItem(i_curr);
          // std::cout << " --- vs --- " << std::endl;
          // coutItem(i_best);
          // std::cout << "^^^^^^^^^^^^^" << std::endl;     
        // }
        if (i_curr.name.substr(0, 4) != i_best.name.substr(0, 4)) {
          if (verbose) {
            float prev_val = value(m_c_curr);
            std::cout << slot << " : " << i_curr.name << " -> " << i_best.name << " => val: " << prev_val;
          }

          static_for_all_slots = 0;
          m_items[slot] = i_best;
          removeItem(i_curr, &m_c_curr);
          addItem(i_best, &m_c_curr);
          if (verbose) {
            float curr_val = value(m_c_curr);
            std::cout << " -> " << curr_val << std::endl;
          }
        }
        float res_val = value(m_c_curr);
        if (res_val > val_best) {
          c_best = m_c_curr;
          items_best = m_items;
          val_best = res_val;
          // if (verbose) {
          if (1) {
            std::cout << std::endl << "*** NEW BEST: " << val_best << " ***" << std::endl;
          }
        }
      }
      slot_ix++;
    }  // for slots
    // m_items = items_best;
    // m_c_curr = c_best;
    iteration++;
  }
  m_items = items_best;
  m_c_curr = c_best;
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



Item ItemPicker::pickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name) const
{
  // create char without item
  PriestCharacter c_no_item = c;
  removeItem(current_item, &c_no_item);
  // for each item
  Item no_item;
  Item best_item = no_item;
  float best_value = value(c_no_item);
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
    if (isBanned(item.name)) {
      val = 0.0f;
    }
    // if (item.slot == "back") {
      // std::cout << item.name << ": " << value(c_no_item) << " -> " << val << std::endl;
    // }
    if (val > best_value || isLocked(item.name)) {
      best_value = val;
      best_item = item;
    }
  }
  return best_item;
}

}  // namespace css
