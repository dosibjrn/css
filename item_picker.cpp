#include "item_picker.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <omp.h>

#include <Eigen/Dense>
  // if (verbose) std::cout << "No item value: " << best_value << std::endl;

#include "dps.h"
#include "hps.h"
#include "item_table.h"
#include "item_operations.h"
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
  // float ehp = s.getEffectiveHp();
  float ehp = s.getEffectiveHpPvp();
  return dps*dps*emana*ehp*ehp
      *((100.0f + 2.5f*std::min(6.0f, c.spell_hit))/100.0f)
      /1e15f;
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
  // float ehp = s.getEffectiveHp();
  float ehp = s.getEffectiveHpPvp();
  // heal free, heal until you die (ehp as mana pool), hp separately
  return hps*hps*emana*ehp*ehp
      // Note: this is just based on hydra's post raid bis where the 2 hit pieces are picked over everything else
      *((100.0f + 2.5f*std::min(6.0f, c.spell_hit))/100.0f)
      /1e15f;
}

float ItemPicker::valuePveHealing(const PriestCharacter& c) const
{
  // TODO: consider a separate type...
  if (!m_logs.empty()) {
    auto res = HpsForLogs(c, m_oh_limit, m_time_left_mul, m_logs); 
    if (global::assumptions.use_deficit_time_sum) {
      return (m_baseline_deficit_time_sum - res.deficit_time_sum)/res.in_combat_sum;
    } else {
      return res.heal_sum/res.in_combat_sum;
    }
  } else {
    float weight_sum = 0.0;
    float hps_sum = 0.0f;
    int n_combats = static_cast<int>(m_pve_healing_combat_lengths.size());
    Stats stats(c);

    auto regens = m_curr_regens;
    auto counts = m_curr_pve_healing_counts;
    if (m_pve_healing_optimizes_counts) {
      counts = bestCounts(c, counts, &regens);
    }
    for (int i = 0; i < n_combats; ++i) {
      Regen regen = regens[i];
      // regen = FindBestRegen(c, counts[i], m_pve_healing_combat_lengths[i], regen); 
      auto res = HpsWithRegen(c, PveHealingSequence(c, counts[i]), m_pve_healing_combat_lengths[i], regen);
      float w = 1.0f;
      if (i < global::assumptions.pve_combat_weights.size()) {
        w = global::assumptions.pve_combat_weights[i];
      }
      hps_sum += w*res.first;
      weight_sum += w; 
    }
    return hps_sum/weight_sum;
  }
}

std::vector<PveInfo> ItemPicker::getPveInfo(const PriestCharacter& c) const
{
  if (m_value_choice != ValueChoice::pve_healing) {
    return std::vector<PveInfo>(m_pve_healing_combat_lengths.size());
  }
  int n_combats = static_cast<int>(m_pve_healing_combat_lengths.size());
  Stats stats(c);

  auto regens = m_curr_regens;
  auto counts = m_curr_pve_healing_counts;
  std::vector<PveInfo> out;
  for (int i = 0; i < n_combats; ++i) {
    Regen regen = regens[i];
    auto res = HpsWithRegen(c, PveHealingSequence(c, counts[i]), m_pve_healing_combat_lengths[i], regen);
    out.push_back(res.second);
  }
  return out;
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
  : m_c_start(c)
  , m_c_curr(c)
  , m_item_table_name(item_table_name)
  , m_value_choice(value_choice)
{
  unsigned my_seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
  std::cout << "Shuffle seed: " << my_seed << std::endl;
  m_generator.seed(my_seed);

  if (m_value_choice == ValueChoice::pve_healing) {
      int n_combats = static_cast<int>(m_pve_healing_combat_lengths.size());
    m_curr_pve_healing_counts.resize(n_combats);
    std::vector<float> init = InitialSpellCounts();
    for (int i = 0; i < n_combats; ++i) {
      m_curr_pve_healing_counts[i] = init;
    }
    m_curr_regens.resize(n_combats);
    for (Regen& regen : m_curr_regens) {
      regen = InitialRegen();
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

bool ItemPicker::isBanned(std::string s) const {
  if (m_banned.find(s) != m_banned.end()) {
    return true;
  }
  size_t comma_pos = s.find(",");
  if (comma_pos == std::string::npos) {
    return false;
  } else {
    std::string first = s.substr(0, comma_pos);
    std::string second = s.substr(comma_pos + 2);  // ", "
    return isBanned(first) || isBanned(second);
  }
}

bool SameItems(const std::map<std::string, Item>& a, const std::map<std::string, Item>& b) {
  int a_size = static_cast<int>(a.size());
  int b_size = static_cast<int>(b.size());

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

void ItemPicker::updateIfNewBest(float val, bool disable_bans, int iteration, int* iters_without_new_best)
{
  float* best = &m_val_best_bans_on;
  if (disable_bans) {
    best = &m_val_best;
  }
  if (val > *best) {
    if (m_value_choice == ValueChoice::pve_healing) {
      if (m_logs.empty()) {
        m_curr_pve_healing_counts = bestCounts(m_c_curr, m_curr_pve_healing_counts, &m_curr_regens);
      }
      val = value(m_c_curr);
    }
    if (val > *best) {
      if (disable_bans) {
        *iters_without_new_best = 0;
      }
      m_c_best = m_c_curr;
      m_items_best = m_items;
      *best = val;
      m_best_regens = m_curr_regens;
      m_best_pve_healing_counts = m_curr_pve_healing_counts;
      m_pve_info = getPveInfo(m_c_best);
      global::assumptions.target_dps_in.clear();
      for (auto info : m_pve_info) {
          global::assumptions.target_dps_in.push_back(info.hps * 0.8f);
      }

      if (1) {
        std::cout << std::endl << "*** NEW BEST (" << !disable_bans << "): " << *best << std::endl;
        if (iteration > 5 && (disable_bans || !m_items_prev_intermediate_results.empty())) CoutCurrentValues();
      }
    }
  }
}

void ItemPicker::PickBestForSlots(const ItemTable &item_table, bool disable_bans, int iteration, int max_iterations, //
                                  int* static_for_all_slots, int* iters_without_new_best)
{
  {
    m_c_curr.set_bonuses.SetPartialAndUpdateCharacter(!disable_bans, &m_c_curr);
    m_c_best.set_bonuses.SetPartialAndUpdateCharacter(!disable_bans, &m_c_best);
  }
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
    if (i_best.name == "" && i_best.slot != "two-hand" && iteration < 10) {
      // m_curr_pve_healing_counts = bestCounts(m_c_curr, m_curr_pve_healing_counts, &m_curr_regens);
      m_pve_healing_optimizes_counts = true;
      // bool penalty = global::assumptions.penalize_oom;
      // global::assumptions.penalize_oom = false;
      std::cout << "Initial best item for slot: " << slot << " : " << i_best.name << ", redoing with count optimization" << std::endl;
      i_best = pickBest(m_c_curr, i_curr, items_for_slot, taken);
      bool debug = false;
      if (debug) {
        m_pve_info = getPveInfo(m_c_curr);
        m_c_best = m_c_curr;
        m_best_pve_healing_counts = m_curr_pve_healing_counts;
        m_best_regens = m_curr_regens;
        CoutBestCounts();
      }
      // global::assumptions.penalize_oom = penalty;
      m_pve_healing_optimizes_counts = false;
    }

    if (slot == "two-hand") {
      Item best_two_hand_item = i_best;
      PriestCharacter c_empty_hands = m_c_curr;
      RemoveItem(m_items["two-hand"], &c_empty_hands);
      RemoveItem(m_items["one-hand"], &c_empty_hands);
      RemoveItem(m_items["main hand"], &c_empty_hands);
      RemoveItem(m_items["off hand"], &c_empty_hands);

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
      AddItem(best_off_hand_item, &c_main_and_off);
      AddItem(best_main_hand_item, &c_main_and_off);
      float val_main_and_off = value(c_main_and_off);

      PriestCharacter c_two_hand = c_empty_hands;
      AddItem(best_two_hand_item, &c_two_hand);
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
        RemoveItem(i_curr, &changed);
        AddItem(i_best, &changed);
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
        RemoveItem(i_curr, &m_c_curr);
        AddItem(i_best, &m_c_curr);
        if (verbose) {
          float curr_val = value(m_c_curr);
          std::cout << " -> " << curr_val << std::endl;
        }
      }
    }
    // recalculateSpecialEffects(&m_c_curr);
    float res_val = value(m_c_curr);
    updateIfNewBest(res_val, disable_bans, iteration, iters_without_new_best);
    slot_ix++;
  }  // for slots
  // CoutDiffsToStart();
}

void SwapIfSame(Item best, Item* start_a, Item* start_b) {
  if (start_a->name == best.name) {
    auto t = *start_a;
    *start_a = *start_b;
    *start_b = t;
  }

}

void ItemPicker::CoutDiffsToStart()
{
  auto c_save = m_c_best;
  m_c_best.set_bonuses.SetPartialAndUpdateCharacter(true, &m_c_best);
  float val = value(m_c_best);
  std::vector<std::string> order = {"head", "neck", "shoulders", "back", "chest", "wrists", "two-hand", "ranged",
    "hands", "waist", "legs", "feet", "finger", "finger 2", "trinket", "trinket 2"};
  std::cout << "Upgrades: " << std::endl;
  std::vector<std::pair<std::string, float>> diffs;
  auto items_start = m_items_start;
  SwapIfSame(m_items_best.at("finger 2"), &items_start["finger"], &items_start["finger 2"]);
  SwapIfSame(m_items_best.at("finger"), &items_start["finger 2"], &items_start["finger"]);
  SwapIfSame(m_items_best.at("trinket 2"), &items_start["trinket"], &items_start["trinket 2"]);
  SwapIfSame(m_items_best.at("trinket"), &items_start["trinket 2"], &items_start["trinket"]);
 
  for (auto slot : order) {
    PriestCharacter c_tmp = m_c_best;
    try { 
      auto best_item = m_items_best.at(slot);
      auto start_item = items_start.at(slot);
      if (slot == "two-hand") {
        AddToItemWithMul(m_items_best.at("main hand"), 1.0f, &best_item);
        best_item.name += ", " + m_items_best.at("main hand").name;
        AddToItemWithMul(m_items_best.at("one-hand"), 1.0f, &best_item);
        best_item.name += ", " + m_items_best.at("one-hand").name;
        AddToItemWithMul(m_items_best.at("off hand"), 1.0f, &best_item);
        best_item.name += ", " + m_items_best.at("off hand").name;

        AddToItemWithMul(items_start.at("main hand"), 1.0f, &start_item);
        start_item.name += ", " + items_start.at("main hand").name;
        AddToItemWithMul(items_start.at("one-hand"), 1.0f, &start_item);
        start_item.name += ", " + items_start.at("one-hand").name;
        AddToItemWithMul(items_start.at("off hand"), 1.0f, &start_item);
        start_item.name += ", " + items_start.at("off hand").name;
      }

      RemoveItem(best_item, &c_tmp);
      float val_no_item = value(c_tmp);
      AddItem(start_item, &c_tmp);
      float val_start = value(c_tmp);
      // std::cout << start_item.name << "(" << val_start - val_no_item << ") -> " << best_item.name << "(" << val - val_no_item << ") : " << val - val_start << std::endl;
      if (best_item.name != "" || start_item.name != "") {
        std::stringstream ss;
        ss << start_item.name << "(" << val_start - val_no_item << ") -> " << best_item.name << "(" << val - val_no_item << ") : " << val - val_start;
        ss << " = +" << 100.0f*(val - val_start)/val_start << "%";
        diffs.push_back(std::make_pair<std::string,float>(ss.str(), val - val_start));
      }
    } catch (const std::exception &e) {
      std::cout << "CoutDiffsToStart is probably missing entry for slot: " << slot << " : " << e.what() << std::endl;
    }
  }
  std::sort(diffs.begin(), diffs.end(), [](const std::pair<std::string, float> &a, const std::pair<std::string, float>& b) -> bool { return a.second > b.second; });
  for (auto diff : diffs) {
    std::cout << diff.first << std::endl;
  }
  m_c_best = c_save;
}

float ItemPicker::ValueIncreaseWeightsBased(const Item& item)
{
  if (m_weights.empty()) return 0.0f;

  float increase = 0.0f;
  increase += item.intelligence*m_weights[0];
  increase += item.spirit*m_weights[1];
  increase += (item.sp + item.sp_healing)*m_weights[2];
  increase += item.mp5*m_weights[3]; 
  increase += item.spell_crit*m_weights[4]; 
  return increase;
}

namespace {
std::string plusIfPos(float f) {
  return f >= 0.0 ? "+" : "";
}
}  // namespace

void ItemPicker::CoutAllUpgrades(bool partial, bool from_start)
{
  auto pve_count_tmp = m_curr_pve_healing_counts;
  auto pve_regen_tmp = m_curr_regens;
  auto c_save = m_c_best;

  if (from_start) {
    m_c_best = m_c_start;
  }

  m_c_best.set_bonuses.SetPartialAndUpdateCharacter(partial, &m_c_best);

  if (from_start) {
    m_curr_pve_healing_counts = m_start_pve_healing_counts;
    m_curr_regens = m_start_regens;
  } else {
    m_curr_pve_healing_counts = m_best_pve_healing_counts;
    m_curr_regens = m_best_regens;
  }

  ItemTable item_table(m_item_table_name);
  std::vector<std::string> order = {"head", "neck", "shoulders", "back", "chest", "wrists", "two-hand", "ranged",
    "hands", "waist", "legs", "feet", "finger", "finger 2", "trinket", "trinket 2"};
  std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
  if (from_start) {
    std::cout << "| Upgrades at start level, from start item to candidate, partial bonuses: " << partial << ": |" << std::endl;
  } else {
    std::cout << "| Upgrades at best in slot level, from start item to candidate, partial bonuses: " << partial << ": |" << std::endl;
  }
  std::cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . " << std::endl;
  auto items_start = m_items_start;
  for (auto slot : order) {
    std::vector<std::pair<std::string, float>> diffs;
    PriestCharacter c_tmp = m_c_best;

    Item curr_item;
    if (m_items_best.find(slot) != m_items_best.end()) {
        curr_item = m_items_best.at(slot);
    }
    if (from_start && m_items_start.find(slot) != m_items_start.end()) {
        curr_item = m_items_start.at(slot);
    }
    RemoveItem(curr_item, &c_tmp);

    float val_no_item = value(c_tmp);

    Item start_item;
    if (items_start.find(slot) != items_start.end()) {
        start_item = items_start.at(slot);
    }
    AddItem(start_item, &c_tmp);
    float val_start = value(c_tmp);

    auto items_for_slot = item_table.getItems(slot);
    if (slot == "two-hand") {
      auto off_hands = item_table.getItems("off hand");
      auto main_hands = item_table.getItems("main hand");
      auto one_hands = item_table.getItems("one-hand");
      main_hands.insert(main_hands.begin(), one_hands.begin(), one_hands.end());
      for (auto off_hand : off_hands) {
        for (auto main_hand : main_hands) {
          // construct one/main-hand + off-hand pairs into new items with "two-hand" slot type
          Item both = main_hand;
          both.name.append(" and " + off_hand.name);
          AddToItemWithMul(off_hand, 1.0f, &both);
          both.slot = "two-hand";

          // add those to list
          items_for_slot.push_back(both);
        }
      }
    }

    RemoveItem(start_item, &c_tmp);

    // output outline:
    // back, ebin cloak (8.25) -> :
    //     lege cloak (12.5) : +2.5%
    //     uber cloak (10.5) : +1.5%
    // here the (val) are cand_diff
    // the percentages are (cand_diff - start_diff)/val_start
    float start_diff = val_start - val_no_item;
    std::cout << "- . - ' - . - ' - . - ' - . - ' - . - ' - . -" << std::endl;
    std::cout << slot << ", " << start_item.name << " (" << start_diff << ")";
    float val_alt_start = val_no_item + ValueIncreaseWeightsBased(start_item);
    if (!m_weights.empty()) {
      std::cout << ", alt: " << val_alt_start - val_no_item;
    }
    std::cout <<  " -> :" << std::endl;

    // loop through said list
    for (auto item : items_for_slot) {
      AddItem(item, &c_tmp);
      float val_candidate = value(c_tmp);
      RemoveItem(item, &c_tmp);
      float val_candidate_alt = val_no_item + ValueIncreaseWeightsBased(item);

      // if val > val_start -> add to list
      auto setNames = getSetNames(item.name);
      bool relevantSet = false;
      for (auto name : setNames) {
        if (m_c_best.set_bonuses.getTotalBonus().name.find(name) != std::string::npos) {
          relevantSet = true;
        }
      }

      if ((val_candidate > val_start || val_candidate_alt > val_alt_start || (relevantSet && !m_weights.empty())) && !isBanned(item)) {
        float cand_diff = val_candidate - val_no_item;
        std::stringstream ss;
        ss << "    " << item.name << " (" << cand_diff << ") : " << plusIfPos(cand_diff - start_diff) << (cand_diff - start_diff)/val_start*100.0f << " %";
        if (!m_weights.empty()) {
          float alt_diff = ValueIncreaseWeightsBased(item);
          ss << ", alt : " << plusIfPos(alt_diff) << alt_diff;
          cand_diff = start_diff + alt_diff;  // To fix sorting below
        }
        ss << ", from: " << item.source;
        diffs.push_back(std::make_pair<std::string, float>(ss.str(), cand_diff - start_diff));
      }
    }
    // Back to start state
    AddItem(curr_item, &c_tmp);

    // sort list
    std::sort(diffs.begin(), diffs.end(), [](const std::pair<std::string, float> &a, const std::pair<std::string, float>& b) -> bool { return a.second > b.second; });
    // cout slot
    for (auto diff : diffs) {
      std::cout << diff.first << std::endl;
    } 
  }  // for slots

  m_curr_pve_healing_counts = pve_count_tmp;
  m_curr_regens = pve_regen_tmp;
  m_c_best = c_save;
}

void ItemPicker::FinalCouts()
{
  CoutBestItems();
  std::cout << "------------------" << std::endl;
  std::cout << "Char stats: " << std::endl;
  CoutCharacterStats();
  std::cout << "------------------" << std::endl;
  std::cout << "Best counts:" << std::endl;
  CoutBestCounts();
  std::cout << "------------------" << std::endl;
  CoutCurrentValues(m_tag_name);
  std::cout << "------------------" << std::endl;
  std::cout << "Diffs to start: " << std::endl;
  CoutDiffsToStart();
  CoutAllUpgrades(true, false);
  CoutAllUpgrades(false, false);
  CoutAllUpgrades(true, true);
  CoutAllUpgrades(false, true);
  std::cout << "------------------" << std::endl;
  std::cout << "Best value: " << getBestValue() << std::endl;
}

void ItemPicker::Calculate()
{

  auto t0 = std::chrono::high_resolution_clock::now();

  // Save start state (from start_with.txt)
  m_c_start = m_c_best;
  m_start_pve_healing_counts = m_best_pve_healing_counts;
  m_items_start = m_items_best;
  m_start_regens = m_best_regens;

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
  //  bool verbose = false;
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
    if (!m_logs.empty()) {
      auto res = FindBestOhLimitAndTimeLeftMul(m_c_curr, m_logs);
      m_oh_limit = res.first;
      m_time_left_mul = res.second;
    } else if (m_value_choice == ValueChoice::pve_healing) {
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
        std::cout << "Temporary bans and partial set bonuses off." << std::endl;
      }
    } else {
      if (iters_no_bans > 0) {
        std::cout << "Temporary bans and partial set bonuses on." << std::endl;
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
  int n_combats = static_cast<int>(m_pve_healing_combat_lengths.size());
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
    if (m_logs.empty()) {
      int n_combats = static_cast<int>(m_pve_healing_combat_lengths.size());
      int n_spells = static_cast<int>(m_best_pve_healing_counts[0].size());
      for (int combat_ix = 0; combat_ix < n_combats; combat_ix++) {
        float dura = m_pve_healing_combat_lengths[combat_ix];
        std::cout << "For " << dura << " s fights:" << std::endl;
        std::cout << "    casts: " << m_best_regens[combat_ix].casts << ", ticks: " << m_best_regens[combat_ix].ticks //
            << ", ticks_oom: " << m_best_regens[combat_ix].ticks_oom << std::endl;
        for (int spell_ix = 0; spell_ix < n_spells; spell_ix++) {
          Spell s = IxToSpell(m_c_curr, spell_ix);
          float count = m_best_pve_healing_counts[combat_ix][spell_ix];
          std::cout << "    " << s.name << ", rank: " << s.rank << ", count: " << count << std::endl;
        }
        std::cout << "    pve info: "
            << "target_alive_mul: " << m_pve_info[combat_ix].target_alive_mul
            << ", oom_penalty_mul: " << m_pve_info[combat_ix].oom_penalty_mul
            << ", regen_penalty_mul: " << m_pve_info[combat_ix].regen_penalty_mul
            << std::endl;

        float hps = HpsWithRegen(m_c_best, 
                                 PveHealingSequence(m_c_best, m_best_pve_healing_counts[combat_ix]), 
                                 m_pve_healing_combat_lengths[combat_ix], 
                                 m_best_regens[combat_ix]).first;
        std::cout << "    hps: " << hps << std::endl; 
      }
    } else {
      auto res = HpsForLogs(m_c_best, m_oh_limit, m_time_left_mul, m_logs); 
      std::cout << "oh_limit: " << m_oh_limit << std::endl;
      std::cout << "time_left_mul: " << m_time_left_mul << std::endl;
      std::cout << "in combat total: " << res.in_combat_sum << std::endl;

      float cast_time_sum = 0.0f;
      float raw_heal_sum = 0.0f;
      for (const auto& entry : res.spell_casts) {
        std::string spell_id = entry.first;
        Spell spell = res.spell_id_to_spell[spell_id];

        float heal_sum = res.spell_heal_sums[spell_id];
        
        float cast_time = spell.cast_time;
        cast_time_sum += cast_time*entry.second;
        raw_heal_sum += entry.second*spell.healing;
        std::cout << spell_id << ", casts: " << entry.second << ", avg: " << heal_sum/entry.second << ", " 
            << heal_sum/res.heal_sum*100.0f << "\%" << ", cast time " << (entry.second*cast_time)/res.in_combat_sum*100.0f
            << "\% of in combat, overheal: " << (entry.second*spell.healing - heal_sum)/(entry.second*spell.healing)
            << std::endl;
        // TODO: overheal overall, overheal per spell, total time casting, total hpm,
      }
      std::cout << "Total time casting: " << cast_time_sum << ", which is: " << cast_time_sum/res.in_combat_sum*100.0f << "\% of time in combat." << std::endl;
      std::cout << "Total hps: " << res.heal_sum/res.in_combat_sum << ", raw hps: " << raw_heal_sum/res.in_combat_sum << ", oh: " << (raw_heal_sum - res.heal_sum)/(raw_heal_sum) << std::endl;

      std::cout << "Deficit time sum: " << res.deficit_time_sum << ", baseline: " << m_baseline_deficit_time_sum << std::endl;
      std::cout << "Deficit*time change / s: " << (m_baseline_deficit_time_sum - res.deficit_time_sum)/res.in_combat_sum << " hp" << std::endl;

      std::cout << "Number of combats: " << res.n_combats << std::endl;
      std::cout << "Average mana at start of combat: " << res.mana_at_start_sum/res.n_combats << std::endl;
      std::cout << "Average mana at end of combat: " << res.mana_at_end_sum/res.n_combats << std::endl;
      constexpr float hps_limit = 100.0f;
      std::cout << "Other players with hps > " << hps_limit << ":" << std::endl;
      for (const auto& entry : res.player_heal_sums) {
        float hps = entry.second / res.in_combat_sum;
        if (hps > hps_limit) {
          std::cout << "  " << entry.first << " : " << hps << " hps" << std::endl;
        }
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
    CoutItem(item);
  }
  std::cout << " ----------------------------" << std::endl;
  std::cout << " --- Set bonuses ---" << std::endl;
  CoutItem(m_c_best.set_bonuses.getTotalBonus());
}

void ItemPicker::CoutCharacterStats() const
{
  Stats s(m_c_best);
  s.CoutStats();
  std::cout << "HpsPvp: " << HpsPvp(m_c_curr) << std::endl;
  std::cout << "ShadowDps: " << ShadowDps(m_c_curr) << std::endl;
  std::cout << "Value pvp healing: " << valuePvpHealing(m_c_curr) << std::endl;
  std::cout << "Value pvp shadow: " << valuePvpShadow(m_c_curr) << std::endl;
  std::cout << "Effective hp-pvp: " << s.getEffectiveHpPvp() << std::endl;
  std::cout << "Effecctive mana for 100 s, 0.33 fsr: " << s.getEffectiveMana(100.f, 0.33f) << std::endl;
}



Item ItemPicker::pickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot, std::string taken_name, bool use_alt)
{
  // bool verbose = current_item.slot == "shoulders";
  bool verbose = false;
  // create char without item
  PriestCharacter c_no_item = c;
  RemoveItem(current_item, &c_no_item);
  // for each item
  Item no_item;
  Item best_item = no_item;
  float no_item_value = value(c_no_item);
  if (verbose) std::cout << "No item value: " << no_item_value << std::endl;
  float best_value = no_item_value;
  bool locked_seen = false;
  int n_items = static_cast<int>(items_for_slot.size());
  std::vector<float> vals(n_items);
#pragma omp parallel for
  for (int i = 0; i < n_items; ++i) {
    const Item& item = items_for_slot[i];
    PriestCharacter c_tmp = c_no_item;
    AddItem(item, &c_tmp);
    float val = value(c_tmp);
    vals[i] = val;
  }

  for (int i = 0; i < n_items; ++i) {
    if (getSetNames(items_for_slot[i].name).empty()) {
      m_stat_diffs_to_hps_diffs.push_back({items_for_slot[i], vals[i] - no_item_value}); 
    }
    if (!m_weights.empty() && use_alt) {
      const Item& item = items_for_slot[i];
      float val_alt = no_item_value + ValueIncreaseWeightsBased(item);;
      if (val_alt > vals[i]) {
        vals[i] = val_alt;
      }
    }
  }

  // for (const Item& item : items_for_slot) {
  for (int i = 0; i < n_items; ++i) {
    const Item& item = items_for_slot[i];
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
    // AddItem(item, &c_tmp);
    // calculate objective function value
    // float val = value(c_tmp);
    float val = vals[i];

    if (verbose) std::cout << item.name << " val: " << val << " , best so far: " << best_item.name << " with val: " << best_value  << std::endl;
    if ((isBanned(item.name) || isBanned(item.source)) && !isWhitelisted(item.name) ) {
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
  return best_item;
}

namespace
{

void MatchValues(const PriestCharacter& c, float* int_val, float* mp5_val, float* crit_val)
{
  float int_val_in = *int_val;
  float mp5_val_in = *mp5_val;
  float crit_val_in = *crit_val;


  float weighted_sum = 0.0f;
  float weight_sum = 0.0f;
  const auto& lens = global::assumptions.pve_combat_lengths;
  auto n_fights = lens.size(); 
  for (size_t i = 0; i < n_fights; ++i) {
    float len = lens[i];
    float w = 1.0f;
    if (i < global::assumptions.pve_combat_weights.size()) {
      w = global::assumptions.pve_combat_weights[i];
    }
    weighted_sum += w*len;
    weight_sum += w;
  }

  bool debug = false;
  float average_len = weighted_sum/weight_sum;
  float average_mana_from_mp5 = average_len/5.0f;
  float one_mana_worth = mp5_val_in/average_mana_from_mp5;
  if (debug) std::cout << "average_len: " << average_len << ", average_mana_from_mp5: " << average_mana_from_mp5 << ", one_mana_worth: " << one_mana_worth << std::endl;

  float int_to_mana = (15.0f * (1.0f + 0.02*c.talents.mental_strength));
  float int_to_crit = 1.0f/59.2f;

  float int_val_alt = one_mana_worth*int_to_mana + crit_val_in*int_to_crit;
  if (debug) std::cout << "int_val_alt: " << int_val_alt << ", vs int_val_in: " << int_val_in << std::endl;

  // Given the eq for int_val_alt above, we need to define crit_val/int_val, to obtain multiplier for scaled int val.
  // 1 = one_mana_worth*int_to_mana/int_val_alt + crit_val_in*int_to_crit/int_val_alt
  // 1 - one_mana_worth*int_to_mana/int_val_alt = crit_val_in*int_to_crit/int_val_alt
  // 1/int_to_crit - one_mana_worth*int_to_mana/(int_val_alt*int_to_crit) = crit_val_in/int_val_alt
  float int_val_to_crit_val = 1.0f/int_to_crit - one_mana_worth*int_to_mana/(int_val_alt*int_to_crit);

  // In same manner, one_mana_worth/int_val
  // int_val = one_mana_worth*int_to_mana + crit_val_in*int_to_crit;
  // 1 = one_mana_worth*int_to_mana/int_val + crit_val_in*int_to_crit/int_val;
  // 1 - crit_val_in*int_to_crit/int_val = one_mana_worth*int_to_mana/int_val
  // (1 - crit_val_in*int_to_crit/int_val)/int_to_mana = one_mana_worth/int_val
  float int_val_to_one_mana_worth = (1.0f - crit_val_in*int_to_crit/int_val_alt)/int_to_mana;

  // we have a fraction of int_to_crit coming from the crit part and int_to_mana fraction coming from mana.
  // 2/3 weight on averaging for the alt value, 1 for int
  // with given split derive the respective crit and mp5 vals so that everything matches.
  float int_val_weighted = 2.0/3.0f*int_val_alt + 1.0f/3.0f*int_val_in;
  float crit_val_weighted = int_val_weighted*int_val_to_crit_val;
  float one_mana_worth_weighted = int_val_weighted*int_val_to_one_mana_worth;
  float mp5_val_weighted = one_mana_worth_weighted*average_mana_from_mp5;
  if (debug) {
    std::cout << "int_val_to_crit_val: " << int_val_to_crit_val << ", int_val_to_one_mana_worth: " << int_val_to_one_mana_worth
        << ", int_val_weighted: " << int_val_weighted << ", crit_val_weighted: " << crit_val_weighted << ", mp5_val_weighted: " << mp5_val_weighted << std::endl;
    std::cout << "one_mana_worth_weighted: " << one_mana_worth_weighted << ", average_mana_from_mp5: " << average_mana_from_mp5 << std::endl;

    float int_val_again_from_weighted_crit_and_mp5 = one_mana_worth_weighted*int_to_mana + crit_val_weighted*int_to_crit;
    std::cout << "int_val_again_from_weighted_crit_and_mp5: " << int_val_again_from_weighted_crit_and_mp5 << std::endl;
  }
  *int_val = int_val_weighted;
  *mp5_val = mp5_val_weighted;
  *crit_val = crit_val_weighted;
}
}  // namespace

void ItemPicker::CoutCurrentValuesBasedOnRecordedDiffs(std::string tag_name)
{
  // Use old entries with least squares fit to find a 

  // A should probably be the item stat weights
  const int n_entries = static_cast<int>(m_stat_diffs_to_hps_diffs.size());
  const int start = std::max(0, n_entries - global::assumptions.n_last_entries_for_alt_stats);
  constexpr int n_stat_types = 6;

  // TODO we do not random init
  
  // n_entries rows, n_stat_types cols
  Eigen::MatrixXf A = Eigen::MatrixXf::Zero(n_entries - start, n_stat_types);
  Eigen::VectorXf b = Eigen::VectorXf::Zero(n_entries - start);
  for (int i = start; i < n_entries; ++i) {
    const int i_out = i - start;
    auto row = A.row(i_out);
    const Item& item = m_stat_diffs_to_hps_diffs[i].first;
    // row[0] = item.strength;
    // row[1] = item.agility;
    row[0] = item.intelligence;
    // row[3] = item.stamina;
    row[1] = item.spirit;


    row[2] = item.sp;
    // row[6] = item.sp_shadow;
    row[3] = item.sp_healing;
    row[4] = item.mp5;
    row[5] = item.spell_crit;
    // row[10] = item.spell_hit;


    // row[11] = item.arcane_res;
    // row[12] = item.nature_res;
    // row[13] = item.fire_res;
    // row[14] = item.frost_res;
    // row[15] = item.shadow_res;


    // row[16] = item.armor;
    // row[17] = item.defense;
    // row[18] = item.dodge;
    // row[19] = item.parry;

    b[i_out] = m_stat_diffs_to_hps_diffs[i].second;
  }

  // And this should be the weights
  std::cout << "Based on last: " << n_entries - start << " entries, out of total: " << n_entries << " entries, ";
  Eigen::VectorXf res = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
  std::cout << "The least-squares solution is:" << std::endl;
  m_weights.resize(4);
  m_weights[0] = res[0];
  m_weights[1] = res[1];
  m_weights[2] = 0.5f * (res[2] + res[3]);
  m_weights[3] = res[4];
  m_weights[4] = res[5];
  for (float &w : m_weights) {
    if (w < 0.0f) w = 0.0f;
  }
  std::cout << "Int: " << m_weights[0]/m_weights[2]*100.0f << std::endl;
  std::cout << "Spirit: " << m_weights[1]/m_weights[2]*100.0f << std::endl;
  std::cout << "Sp / Healing: " << m_weights[2]/m_weights[2]*100.0f << std::endl;
  std::cout << "Mp5: " << m_weights[3]/m_weights[2]*100.0f << std::endl;
  std::cout << "Crit: " << m_weights[4]/m_weights[2]*100.0f << std::endl;

  // clear for next
  // m_stat_diffs_to_hps_diffs.clear();
}

void ItemPicker::CoutCurrentValues(std::string tag_name)
{
  if (!m_logs.empty()) {
    CoutCurrentValuesBasedOnRecordedDiffs(tag_name);
    return;
  }
  auto saved = global::assumptions.penalize_oom;
  global::assumptions.penalize_oom = false;
  PriestCharacter c = m_c_best;
  float val_start = value(c); 
  std::vector<std::string> stat_names = {"Intellect",    "Spirit",   "Stamina",  "Agility",  "Mp5", "SpellPower", "ShadowSpellDamage", "Healing",    "SpellCritRating",
    "SpellHitRating",  "ArcaneResist",  "NatureResist",  "FireResist",  "FrostResist",  "ShadowResist",  "Armor",  "DefenseRating",  "DodgeRating",  "ParryRating"};
  std::vector<float*> stat_ptrs =       {&c.intelligence, &c.spirit, &c.stamina, &c.agility, &c.mp5, &c.sp,       &c.sp_shadow,        &c.sp_healing, &c.spell_crit,
    &c.spell_hit,      &c.arcane_res,   &c.nature_res,   &c.fire_res,   &c.frost_res,   &c.shadow_res,   &c.armor, &c.defense,       &c.dodge, &c.parry};
  std::vector<float> steps =            {1,               1,         1,          1,          1,     1,             1,                  1,             1,
    1,                 10,              10,              10,            10,             10,              100,      1,                1,              1};
  int n_vals = static_cast<int>(stat_names.size());
  int ref_ix = 5;

  int diff = static_cast<int>(50*steps[ref_ix]);
  *(stat_ptrs[ref_ix]) += diff;
  float ref_val_diff = value(c) - val_start;

  std::cout << "Current relative stat values for matching or exceeding +" << diff << " pts of " << stat_names[ref_ix] //
      << std::endl;
  std::vector<float> matching_diffs(n_vals);
  std::stringstream ss;
  ss << "( Pawn: v1: \"" << tag_name << "\": ";
  std::vector<float> relative_values(n_vals);
  for (int i = 0; i < n_vals; ++i) {
    c = m_c_best;
    float diff_required = 0.0f;
    int max_tries = 200;
    float obtained_val_diff = 0.0f;

    float diff_required_sum = 0.0f;
    float obtained_val_diff_sum = 0.0f;
    while (1) {
      diff_required += steps[i];
      *(stat_ptrs[i]) += steps[i]*1.0f;
      float val = value(c);

      if (val > val_start) {
        obtained_val_diff_sum += val - val_start;
        diff_required_sum += diff_required;
      }

      if (val - val_start >= ref_val_diff) {
        obtained_val_diff = val - val_start;
        break;
      }
      if (diff_required/steps[i] > diff/steps[ref_ix] && val <= val_start) {
        obtained_val_diff = val - val_start;
        diff_required = 1e20f;
        diff_required_sum += 1e20f;
        break;
      }
      if (diff_required/steps[i] > max_tries) {
        obtained_val_diff = val - val_start;
        break;
      }
    }
    // float relative_value = obtained_val_diff/ref_val_diff*diff/diff_required*100.0f;
    if (diff_required_sum > 0.0f) {
      float relative_value = obtained_val_diff_sum/ref_val_diff*diff/diff_required_sum*100.0f;
      relative_values[i] = relative_value;
    } else {
      relative_values[i] = 0.0f;
    }
  }

  int int_ix = 0;
  int mp5_ix = 4;
  int crit_ix = 8;

  if (m_value_choice == ValueChoice::pve_healing) {
    MatchValues(m_c_best, &relative_values[int_ix], &relative_values[mp5_ix], &relative_values[crit_ix]);
  }

  // Finally rescale
  const float ref_val = relative_values[ref_ix];
  for (int i = 0; i < n_vals; ++i) {
    float relative_value = relative_values[i]/ref_val * 100.0f;
    std::cout << stat_names[i] << ": " << relative_value << std::endl;
    ss << stat_names[i] << "=" << relative_value << ", ";
  }


  ss << "IsFist=X, Is2HMace=X, IsCrossbow=X, IsGun=X, IsShield=X, IsPolearm=X, Is2HAxe=X, IsBow=X, IsMail=X, IsPlate=X, IsLeather=X, IsAxe=X, Is2HAxe=X, IsSword=X, Is2HSword=X)";
  if (!tag_name.empty()) {
    std::string fn = tag_name + ".pawn_tag.txt";
    std::ofstream os(fn, std::ofstream::app);
    os << ss.str() << std::endl;
    os.close();
    std::cout << "Appended tag:" << std::endl;
    std::cout << ss.str() << std::endl;
    std::cout << "To file: " << fn << std::endl; 
  }
  global::assumptions.penalize_oom = saved;
}


void ItemPicker::AddLog(const std::string& log_fn)
{
  m_logs = GetLogs(log_fn);
  PriestCharacter c;
  // no mana -> no heals
  auto res = HpsForLogs(c, 0.5, 0.5, m_logs); 
  m_baseline_deficit_time_sum = res.deficit_time_sum;
}

}  // namespace css
