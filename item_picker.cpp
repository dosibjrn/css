#include "item_picker.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include "dps.h"
#include "hps.h"
#include "item_table.h"
#include "stats.h"

namespace css
{

float ItemPicker::valuePvpShadow(const PriestCharacter& c) const
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

float ItemPicker::valuePvpHealing(const PriestCharacter& c) const
{
  // Similar to what was done prev: dps*dps*ehp*emana
  float hps = HpsPvp(c);
  Stats s(c);
  float duration = 100.0f;  // s
  float fsr_frac = 2.0f/3.0f;
  // float duration = 30.0f;  // s
  // float fsr_frac = 2.5f/3.0f;
 
  float emana = s.getEffectiveMana(duration, fsr_frac);
  float ehp = s.getEffectiveHp();
  return hps*hps*emana*ehp/1e12;
}

float ItemPicker::valuePveHealing(const PriestCharacter& c) const
{
  int count = 0;
  float hps_sum = 0.0f;
  int n_combats = m_pve_healing_combat_lengths.size();
  Stats stats(c);

  auto muls = m_mana_to_regen_muls;
  auto counts = bestCounts(c, m_curr_pve_healing_counts, &muls);
  for (int i = 0; i < n_combats; ++i) {
    float mana_to_regen = muls[i]*stats.getMaxMana();
    hps_sum += Hps(c, PveHealingSequence(c, counts[i]), m_pve_healing_combat_lengths[i], mana_to_regen);
    count++;
  }
  return hps_sum/count;
}

float ItemPicker::value(const PriestCharacter &c) const
{
  switch (m_value_choice) {
    case ValueChoice::pvp_shadow :
      return valuePvpShadow(c);
    case ValueChoice::pvp_healing :
      return valuePvpHealing(c);
    case ValueChoice::pve_healing :
      return valuePveHealing(c);
    default:
      return valuePvpShadow(c);
  }
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

ItemPicker::ItemPicker(const PriestCharacter& c, std::string item_table_name, ValueChoice value_choice)
  : m_c_in(c)
  , m_c_curr(c)
  , m_item_table_name(item_table_name)
  , m_value_choice(value_choice)
{
  if (m_value_choice == ValueChoice::pve_healing) {
    int n_combats = m_pve_healing_combat_lengths.size();
    m_curr_pve_healing_counts.resize(n_combats);
    std::vector<float> init = {10.0, 5.0, 3.0, 3.0, 3.0, 1.0};
    for (int i = 0; i < n_combats; ++i) {
      m_curr_pve_healing_counts[i] = init;
    }
  }
  // Calculate();
}

void ItemPicker::intermediateCout(int iteration)
{
  std::cout << "***** INTERMEDIATE RESULTS *****" << std::endl;
  std::cout << "Iteration: " << iteration << std::endl;
  CoutBestItems();
  std::cout << "------------------" << std::endl;
  std::cout << "Best value: " << getBestValue() << std::endl;
  std::cout << "------------------" << std::endl;
  std::cout << "Char stats: " << std::endl;
  CoutCharacterStats();
  auto c = getCharacter();
  std::cout << "------------------" << std::endl;

  std::cout << "Best counts:" << std::endl;
  CoutBestCounts();
  std::cout << "------------------" << std::endl;
  std::cout << "~~~~ end of intermediate results ~~~~" << std::endl;

}

void ItemPicker::Calculate()
{
  auto t0 = std::chrono::high_resolution_clock::now();
  m_c_curr = m_c_in;
  m_items.clear();
  ItemTable item_table(m_item_table_name);
  int static_for_all_slots = 0;
  int max_iterations = 1000;
  if (m_value_choice == ValueChoice::pve_healing) {
    max_iterations = 2000;
  }
  std::vector<std::string> slots = item_table.getItemSlots();
  for (const auto& slot : slots) {
    Item i;
    i.slot = slot;
    m_items[slot] = i;
  }

  m_val_best = 0.0f;

  int iteration = 0;
  bool verbose = false;
  int max_static_for_all_slots = 20;
  while (static_for_all_slots < max_static_for_all_slots
         && iteration < max_iterations) {
    m_curr_pve_healing_counts = bestCounts(m_c_curr, m_curr_pve_healing_counts, &m_mana_to_regen_muls);

    auto t1 = std::chrono::high_resolution_clock::now();
    bool disable_bans = false;
    if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() > 45.0) {
      disable_bans = true;
    }
    if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() > 60.0) {
      t0 = t1;
      intermediateCout(iteration);
    }
    if (iteration % (max_iterations/50) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    static_for_all_slots++;
    if (static_for_all_slots > max_static_for_all_slots/2) {
      disable_bans = true;
    }
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
                 && (slot_ix % (iteration + 1 % slots.size()) == 0) && !disable_bans) {
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
        if (res_val > m_val_best) {
          m_c_best = m_c_curr;
          m_items_best = m_items;
          m_val_best = res_val;
          m_pve_healing_counts_best = bestCounts(m_c_best, m_curr_pve_healing_counts, &m_mana_to_regen_muls);
          if (1) {
            std::cout << std::endl << "*** NEW BEST: " << m_val_best << " ***" << std::endl;
            CoutCurrentValues();
          }
        }
      }
      slot_ix++;
    }  // for slots
    // m_items = items_best;
    // m_c_curr = c_best;
    iteration++;
  }
  m_items = m_items_best;
  m_c_curr = m_c_best;
}

std::vector<std::vector<float>> ItemPicker::bestCounts(const PriestCharacter& c,
                                                       const std::vector<std::vector<float>>& init_counts,
                                                       std::vector<float>* mana_to_regen_muls) const
{
  std::vector<std::vector<float>> counts_out = init_counts;
  int n_combats = m_pve_healing_combat_lengths.size();
  Stats stats(c);
  for (int combat_ix = 0; combat_ix < n_combats; ++combat_ix) {
    counts_out[combat_ix]
        = FindBestPveHealingCounts(c, init_counts[combat_ix], 
                                   m_pve_healing_combat_lengths[combat_ix],
                                   &((*mana_to_regen_muls)[combat_ix]));
  }
  return counts_out;
}

void ItemPicker::CoutBestCounts() const
{
  int n_combats = m_pve_healing_combat_lengths.size();
  int n_spells = m_pve_healing_counts_best[0].size();
  for (int combat_ix = 0; combat_ix < n_combats; combat_ix++) {
    float dura = m_pve_healing_combat_lengths[combat_ix];
    std::cout << "For " << dura << " fights:" << std::endl;
    std::cout << "    mana_to_regen_mul: " << m_mana_to_regen_muls[combat_ix] << std::endl;
    for (int spell_ix = 0; spell_ix < n_spells; spell_ix++) {
      Spell s = IxToSpell(m_c_curr, spell_ix);
      float count = m_pve_healing_counts_best[combat_ix][spell_ix];
      std::cout << "    " << s.name << ", rank: " << s.rank << ", count: " << count << std::endl;
    }
  }
}


void ItemPicker::CoutBestItems()
{
  std::vector<std::string> order = {"head", "neck", "shoulders", "back", "chest", "wrists", "two-hand", "main hand", "one-hand", "off hand", "ranged",
    "hands", "waist", "legs", "feet", "finger", "finger 2", "trinket", "trinket 2"};
    // "hands", "waist", "legs", "feet", "finger", "trinket"};
  // for (auto map_entry : m_items) {
  for (std::string slot : order) {
    const Item &item = m_items_best[slot];
    std::cout << " ----------------------------" << std::endl;
    std::cout << " --- " << item.slot << " --- " << std::endl;
    coutItem(item);
  }
}

void ItemPicker::CoutCharacterStats() const
{
  Stats s(m_c_best);
  s.CoutStats();
  std::cout << "HpsPvp: " << HpsPvp(m_c_curr) << std::endl;
  std::cout << "ShadowDps: " << ShadowDps(m_c_curr) << std::endl;
  std::cout << "Value pvp healing: " << valuePvpHealing(m_c_curr) << std::endl;
  std::cout << "Value pvp shadow: " << valuePvpShadow(m_c_curr) << std::endl;
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

void ItemPicker::CoutCurrentValues() const
{
  int diff = 50;
  PriestCharacter c = m_c_best;
  float val_start = value(c); 
  std::vector<std::string> stat_names = {"int",           "spi",     "sta",      "mp5",  "sp",  "sp_shadow",  "sp_healing"};
  std::vector<int*> stat_ptrs =       {&c.intelligence, &c.spirit, &c.stamina, &c.mp5, &c.sp, &c.sp_shadow, &c.sp_healing};
  int n_vals = stat_names.size();
  std::vector<float> val_results(n_vals);
  for (int i = 0; i < n_vals; ++i) {
    *(stat_ptrs[i]) += diff;
    val_results[i] = value(c) - val_start;
    c = m_c_curr;
  }
  int ref_ix = 4;
  float normalization_ref_val = val_results[4];
  float normalization_mul = 100.f/normalization_ref_val;
  std::cout << "Current relative stat values for " << diff << " pt diff for each:" << std::endl;
  std::vector<int> next_step_sizes(val_results.size());
  for (int i = 0; i < n_vals; ++i) {
    next_step_sizes[i] = normalization_ref_val/val_results[i]*diff;
    if (val_results[i] == 0) {
      next_step_sizes[i] = 1;
    }
    val_results[i] *= normalization_mul;
    std::cout << stat_names[i] << ": " << val_results[i] << ", next step size: " << next_step_sizes[i] << std::endl;
  }

  // same normalization_ref_val, same normalization_mul
  for (int i = 0; i < n_vals; ++i) {
    *(stat_ptrs[i]) += next_step_sizes[i];
    float result_rel = (value(c) - val_start)/(next_step_sizes[i]);
    result_rel *= diff*normalization_mul;
    c = m_c_curr;
    std::cout << stat_names[i] << ": " << result_rel << std::endl;
  }

  
  // take 20 of ref index and matching amount of everything else
  // std::vector<float> val_results_relative(val_results.size());

}

}  // namespace css
