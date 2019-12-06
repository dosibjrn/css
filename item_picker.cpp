#include "item_picker.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

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

  auto regens = m_curr_regens;
  auto counts = m_curr_pve_healing_counts;
  for (int i = 0; i < n_combats; ++i) {
    Regen regen = regens[i];
    hps_sum += HpsWithRegen(c, PveHealingSequence(c, counts[i]), m_pve_healing_combat_lengths[i], regen);
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


ItemPicker::ItemPicker(const PriestCharacter& c, std::string item_table_name, ValueChoice value_choice)
  : m_c_in(c)
  , m_c_curr(c)
  , m_item_table_name(item_table_name)
  , m_value_choice(value_choice)
{
  unsigned my_seed = std::chrono::system_clock::now().time_since_epoch().count();
  // unsigned my_seed = 123456789;
  my_seed = 99851579;
  std::cout << "Shuffle seed: " << my_seed << std::endl;


  m_generator.seed(my_seed);

  if (m_value_choice == ValueChoice::pve_healing) {
    int n_combats = m_pve_healing_combat_lengths.size();
    m_curr_pve_healing_counts.resize(n_combats);
    // std::vector<float> init = {10.0, 5.0, 3.0, 3.0, 3.0, 0.0};
    std::vector<float> init = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    for (int i = 0; i < n_combats; ++i) {
      m_curr_pve_healing_counts[i] = init;
    }
    m_curr_regens.resize(n_combats);
    for (Regen& regen : m_curr_regens) {
      regen = Regen(10, 10);
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
  CoutDiffsToStart();
  std::cout << "------------------" << std::endl;
  std::cout << "~~~~ end of intermediate results ~~~~" << std::endl;

}

bool SameItems(const std::map<std::string, Item>& a, const std::map<std::string, Item>& b) {
  int a_size = a.size();
  int b_size = b.size();

  if (a_size != b_size) {
    return false;
  }


  std::vector<std::string> names_a;
  std::vector<std::string> names_b;
  for (auto entry : a) {
    names_a.push_back(entry.second.name);
  }
  for (auto entry : b) {
    names_b.push_back(entry.second.name);
  }
  std::sort(names_a.begin(), names_a.end());
  std::sort(names_b.begin(), names_b.end());
  for (int i = 0; i < a_size; ++i) {
    if (names_a[i] != names_b[i]) {
      return false;
    }
  }
  return true;
}

void ItemPicker::PickBestForSlots(const ItemTable &item_table, bool disable_bans, int iteration, int max_iterations, //
                                  int* static_for_all_slots, int* iters_without_new_best)
{
  std::vector<std::string> slots = item_table.getItemSlots();
  if (m_items.empty()) {
    for (const auto& slot : slots) {
      Item i;
      i.slot = slot;
      m_items[slot] = i;
    }
  }
  bool verbose = false;
  shuffle(&slots);
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
      if (iteration < max_iterations/2 && !disable_bans) {
        taken = m_items["main hand"].name;
      }
      Item best_main_hand_item = pickBest(c_empty_hands, no_item, main_hand_items, taken);

      std::vector<Item> off_hand_items = item_table.getItems("off hand");
      if (iteration < max_iterations/2 && !disable_bans) {
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

      float prev_val = 0.0f;
      std::string prev_items;
      if (disable_bans) {
        prev_val = value(m_c_curr);
        prev_items = m_items["two-hand"].name + " " + m_items["one-hand"].name + " " + m_items["main hand"].name + " "
            + m_items["off hand"].name;
      }

      if (val_two_hand > val_main_and_off 
          || (isLocked(best_two_hand_item.name) && (!isLocked(best_main_hand_item.name) && !isLocked(best_off_hand_item.name)))) {
        m_c_curr = c_two_hand;
        m_items["two-hand"] = best_two_hand_item;
        m_items["one-hand"] = no_item;
        m_items["main hand"] = no_item;
        m_items["off hand"] = no_item;
      } 
      if (val_two_hand <= val_main_and_off 
           || ((isLocked(best_main_hand_item.name) || isLocked(best_off_hand_item.name)) && !isLocked(best_two_hand_item.name))) {
        m_c_curr = c_main_and_off;
        m_items["two-hand"] = no_item;
        m_items["one-hand"] = no_item;
        m_items["main hand"] = best_main_hand_item;
        m_items["off hand"] = best_off_hand_item;
      }
      if (disable_bans) {
        float post_val = value(m_c_curr);
        if (post_val > prev_val) {
          std::string post_items = m_items["two-hand"].name + " " + m_items["one-hand"].name + " " + m_items["main hand"].name + " "
              + m_items["off hand"].name;
          std::cout << slot << " : " << prev_items << " -> " << post_items << " => val: " << prev_val << " -> "
              << post_val << std::endl;
        }
      }
    } else { // if not two hand
      float prev_val = 0.0f;
      if (disable_bans) {
        prev_val = value(m_c_curr);
        PriestCharacter changed = m_c_curr;
        removeItem(i_curr, &changed);
        addItem(i_best, &changed);
        float post_val = value(changed);
        if (post_val > prev_val) {
          std::cout << slot << " : " << i_curr.name << " -> " << i_best.name << " => val: " << prev_val << " -> "
              << post_val << std::endl;
        }
      }
      if (i_curr.name != i_best.name) {
        if (verbose) {
          prev_val = value(m_c_curr);
          std::cout << slot << " : " << i_curr.name << " -> " << i_best.name << " => val: " << prev_val;
        }

        *static_for_all_slots = 0;
        m_items[slot] = i_best;
        removeItem(i_curr, &m_c_curr);
        addItem(i_best, &m_c_curr);
        if (verbose) {
          float curr_val = value(m_c_curr);
          std::cout << " -> " << curr_val << std::endl;
        }
      }
    }
    float res_val = value(m_c_curr);
    if (res_val > m_val_best) {
      if (m_value_choice == ValueChoice::pve_healing) {
        m_curr_pve_healing_counts = bestCounts(m_c_curr, m_curr_pve_healing_counts, &m_curr_regens);
        res_val = value(m_c_curr);
      }
      if (res_val > m_val_best) {
        *iters_without_new_best = 0;
        m_c_best = m_c_curr;
        m_items_best = m_items;
        m_val_best = res_val;
        m_best_regens = m_curr_regens;
        m_best_pve_healing_counts = m_curr_pve_healing_counts;

        if (1) {
          std::cout << std::endl << "*** NEW BEST: " << m_val_best << " ***" << std::endl;
          // CoutCurrentValues();
          if (iteration > 5 && (disable_bans || !m_items_prev_intermediate_results.empty())) CoutCurrentValuesAlt();
        }
      }
    }
    slot_ix++;
  }  // for slots
  // CoutDiffsToStart();
}

void ItemPicker::CoutDiffsToStart() const
{
  float val = value(m_c_best);
  std::vector<std::string> order = {"head", "neck", "shoulders", "back", "chest", "wrists", "two-hand", "main hand", "one-hand", "off hand", "ranged",
    "hands", "waist", "legs", "feet", "finger", "finger 2", "trinket", "trinket 2"};
  std::cout << "Upgrades: " << std::endl;
  std::vector<std::pair<std::string, float>> diffs;
  for (auto slot : order) {
    PriestCharacter c_tmp = m_c_best;
    try { 
      auto best_item = m_items_best.at(slot);
      auto start_item = m_items_start.at(slot);
      removeItem(best_item, &c_tmp);
      float val_no_item = value(c_tmp);
      addItem(start_item, &c_tmp);
      float val_start = value(c_tmp);
      // std::cout << start_item.name << "(" << val_start - val_no_item << ") -> " << best_item.name << "(" << val - val_no_item << ") : " << val - val_start << std::endl;
      std::stringstream ss;
      ss << start_item.name << "(" << val_start - val_no_item << ") -> " << best_item.name << "(" << val - val_no_item << ") : " << val - val_start;
      diffs.push_back(std::make_pair<std::string,float>(ss.str(), val - val_start));
    } catch (const std::exception &e) {
      std::cout << "CoutDiffsToStart is probably missing entry for slot: " << slot << " : " << e.what() << std::endl;
    }
  }
  std::sort(diffs.begin(), diffs.end(), [](const std::pair<std::string, float> &a, const std::pair<std::string, float>& b) -> bool { return a.second > b.second; });
  for (auto diff : diffs) {
    std::cout << diff.first << std::endl;
  }
}

void ItemPicker::Calculate()
{

  m_items_start = m_items_best;
  auto t0 = std::chrono::high_resolution_clock::now();
  // m_c_curr = m_c_in;
  ItemTable item_table(m_item_table_name);
  int static_for_all_slots = 0;
  int max_iterations = 1000;
  int n_dots = 50;
  if (m_value_choice == ValueChoice::pve_healing) {
    max_iterations = 2000;
    n_dots = max_iterations;
  }

  m_val_best = 0.0f;

  int iteration = 0;
  bool verbose = false;
  constexpr int max_static_for_all_slots = 10;
  const int max_iters_without_new_best = max_iterations/10;
  int iters_without_new_best = 0;
  bool disable_bans = false;
  int iters_no_bans = 0;
  int max_iters_no_bans = 10;
  while ((static_for_all_slots < max_static_for_all_slots
         && iteration < max_iterations
         && iters_without_new_best < max_iters_without_new_best)
         || iters_no_bans < max_iters_no_bans) {
    if (m_value_choice == ValueChoice::pve_healing) {
      m_curr_pve_healing_counts = bestCounts(m_c_curr, m_curr_pve_healing_counts, &m_curr_regens);
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() > 60.0 && disable_bans) {
      t0 = t1;
      intermediateCout(iteration);
      if (SameItems(m_items_prev_intermediate_results, m_items_best)) {
        std::cout << "Same items for intermediate results again, ending optimization. " << std::endl;
        break;
      }
      m_items_prev_intermediate_results = m_items_best;
      static_for_all_slots = 0;
    }

    disable_bans = false;
    if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() > 45.0
        || static_for_all_slots > max_static_for_all_slots/2
        || iters_without_new_best > max_iters_without_new_best/2) {
      disable_bans = true;
      iters_no_bans++;
      if (iters_no_bans == 1) {
        std::cout << "Bans off." << std::endl;
      }
    } else {
      if (iters_no_bans > 0) {
        std::cout << "Bans on." << std::endl;
      }
      iters_no_bans = 0;
    }
    if (iters_no_bans > max_iters_no_bans/2) {
      float curr_val = value(m_c_curr);
      if (curr_val < m_val_best) {
        std::cout << "Copied previous best." << std::endl;
        m_c_curr = m_c_best;
        m_items = m_items_best;
        m_curr_pve_healing_counts = m_best_pve_healing_counts;
        m_curr_regens = m_best_regens;
      }
    }
    if (iteration % (max_iterations/n_dots) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    static_for_all_slots++;
    iters_without_new_best++;
    PickBestForSlots(item_table, disable_bans, iteration, max_iterations, //
                     &static_for_all_slots, &iters_without_new_best);
    iteration++;
  }
  m_items = m_items_best;
  m_c_curr = m_c_best;
  std::ofstream os("ended_with.txt");
  for (auto item : m_items) {
    // os.writeline(item.second.name);
    std::string name = item.second.name;
    if (name.size() > 3) {
      os << item.second.name << std::endl;
    }
  }
  os.close();
}

std::vector<std::vector<float>> ItemPicker::bestCounts(const PriestCharacter& c,
                                                       const std::vector<std::vector<float>>& init_counts,
                                                       std::vector<Regen>* regens) const
{

  std::vector<std::vector<float>> counts_out = init_counts;
  int n_combats = m_pve_healing_combat_lengths.size();
  Stats stats(c);
  for (int combat_ix = 0; combat_ix < n_combats; ++combat_ix) {
    counts_out[combat_ix]
        = FindBestPveHealingCounts(c, init_counts[combat_ix], 
                                   m_pve_healing_combat_lengths[combat_ix],
                                   &((*regens)[combat_ix]));
  }
  return counts_out;
}

void ItemPicker::CoutBestCounts() const
{
  if (m_value_choice == ValueChoice::pve_healing) {
    int n_combats = m_pve_healing_combat_lengths.size();
    int n_spells = m_best_pve_healing_counts[0].size();
    for (int combat_ix = 0; combat_ix < n_combats; combat_ix++) {
      float dura = m_pve_healing_combat_lengths[combat_ix];
      std::cout << "For " << dura << " s fights:" << std::endl;
      std::cout << "    casts: " << m_best_regens[combat_ix].casts << ", ticks: " << m_best_regens[combat_ix].ticks //
          << std::endl;
      for (int spell_ix = 0; spell_ix < n_spells; spell_ix++) {
        Spell s = IxToSpell(m_c_curr, spell_ix);
        float count = m_best_pve_healing_counts[combat_ix][spell_ix];
        std::cout << "    " << s.name << ", rank: " << s.rank << ", count: " << count << std::endl;
      }
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
  // bool verbose = current_item.slot == "ranged";
  bool verbose = false;
  // create char without item
  PriestCharacter c_no_item = c;
  removeItem(current_item, &c_no_item);
  // for each item
  Item no_item;
  Item best_item = no_item;
  float best_value = value(c_no_item);
  if (verbose) std::cout << "No item value: " << best_value << std::endl;
  bool locked_seen = false;
  for (const Item& item : items_for_slot) {
  // set char to state without item
    if (item.name == taken_name) {
      if (verbose) std::cout << item.name << " taken." << std::endl;
      continue;
    }
    if (isLocked(item.name)) {
      if (!locked_seen) {
        best_value = 0.0f;
        best_item = no_item;
        if (verbose) std::cout << "First locked -> No item value: " << best_value << std::endl;
      }
      locked_seen = true;
    }
    PriestCharacter c_tmp = c_no_item;
    // add effect of new item
    addItem(item, &c_tmp);
    // calculate objective function value
    float val = value(c_tmp);
    if (isBanned(item.name)) {
      val = 0.0f;
    }
    if (val > best_value 
        && (!locked_seen || isLocked(item.name))) {
      
      if (verbose) std::cout << item.name << " val: " << val << " is new best, old was: " << best_item.name << " with val: " << best_value  << std::endl;
      best_value = val;
      best_item = item;

    } else {
      if (verbose) std::cout << item.name << " val: " << val << " is not new best, best is: " << best_item.name << " with val: " << best_value  << std::endl;
    }
  }
  if (verbose && locked_seen && best_item.name != "dragon finger of intellect") {
    for (int i = 0; i < 10; ++i) {
      std::cout << "!!!!!!!!!!!!! B O O M !!!!!!!!!!" << std::endl;
    }
  }
  return best_item;
}

void ItemPicker::CoutCurrentValuesAlt() const
{
  int diff = 50;
  PriestCharacter c = m_c_best;
  float val_start = value(c); 
  std::vector<std::string> stat_names = {"int",           "spi",     "sta",      "mp5",  "sp",  "sp_shadow",  "sp_healing"};
  std::vector<int*> stat_ptrs =       {&c.intelligence, &c.spirit, &c.stamina, &c.mp5, &c.sp, &c.sp_shadow, &c.sp_healing};
  int n_vals = stat_names.size();
  int ref_ix = 4;


  *(stat_ptrs[ref_ix]) += diff;
  float ref_val_diff = value(c) - val_start;

  std::cout << "Current relative stat values for matching or exceeding +" << diff << " pts of " << stat_names[ref_ix] //
      << std::endl;
  std::vector<float> matching_diffs(n_vals);
  for (int i = 0; i < n_vals; ++i) {
    c = m_c_best;
    float diff_required = 0.0f;
    while (1) {
      diff_required++;
      *(stat_ptrs[i]) += 1.0f;
      float val = value(c);
      if (val - val_start >= ref_val_diff) {
        break;
      }
      if (diff_required > diff && val <= val_start) {
        diff_required = 1e20;
        break;
      }
    }
    float relative_value = diff/diff_required*100.0f;
    std::cout << stat_names[i] << ": " << relative_value << std::endl;
  }
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
