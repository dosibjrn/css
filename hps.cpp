#include "hps.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>

#include "spells_priest_disc.h"
#include "spells_priest_holy.h"
#include "stats.h"

namespace css
{

namespace {
// handle hots
void HandleHots(float time, std::vector<Spell>* hots, float* heal_sum)
{
  for (Spell& hot : *hots) {
    if (time >= hot.start + hot.ticked*hot.tick_after && hot.ticked < hot.num_ticks) {
      *heal_sum += hot.healing;
      *heal_sum += hot.shield;
      hot.ticked++;
    }
  }
}

void HandleManaRegen(float time, float last_cast_time, const Stats& stats, float* mana, int* regen_ticks)
{
  while ((*regen_ticks + 1)*2.0f <= time) {
    if (last_cast_time + 5.0f < time) {
      *mana += stats.getManaRegenTickOutOfFsr();
    } else {
      *mana += stats.getManaRegenTickUnderFsr();
    }
    (*regen_ticks)++;
  }
}
}  // namespace

float Hps(const PriestCharacter& c, const std::vector<Spell>& spell_sequence, float end_at_s, float mana_to_regen)
{
  std::vector<Spell> hots;
  float heal_sum = 0.0f;
  float time = 0.0f;
  float last_cast_time = 0.0f;
  int regen_ticks = 0;
  Stats stats(c);
  float mana = stats.getMaxMana();

  float pi_end = -180.0f + 15.0f;
  if (c.talents.power_infusion && time >= pi_end + 180.0f - 15.0f) {
    pi_end = time + 15.0f;
    time += 1.5f;
    mana -= 0.2*c.base_mana;
  }

  int ix = 0;
  while (1) {
    if (c.talents.power_infusion && time >= pi_end + 180.0f - 15.0f) {
      pi_end = time + 15.0f;
      time += 1.5f;
      mana -= 0.2*c.base_mana;
    }

    Spell spell = spell_sequence[ix];
    if (time + 3.0f < pi_end) {
      spell = GreaterHeal(c, -1);
      ix--;
    }
    float heal_sum_start = heal_sum;
    if (spell.cost > mana) {
      while (mana < std::max(spell.cost, mana_to_regen)) {
        time += 2.0f;
        HandleHots(time, &hots, &heal_sum);
        HandleManaRegen(time, last_cast_time, stats, &mana, &regen_ticks);
        if (time >= end_at_s) {
          return heal_sum/time;
        }
      }
    }
    mana -= spell.cost;
    time += spell.cast_time;
    last_cast_time = time;
    if (spell.instant) {
      last_cast_time -= spell.cast_time;
    }
    if (spell.over_time) {
      hots.push_back(spell);
      hots.back().start = 0;
    } 
    if (!spell.over_time) {
      heal_sum += spell.healing;
      heal_sum += spell.shield;
    }

    HandleHots(time, &hots, &heal_sum);
    HandleManaRegen(time, last_cast_time, stats, &mana, &regen_ticks);
    if (time >= end_at_s) {
      return heal_sum/time;
    }
    ix++;
    if (ix >= static_cast<int>(spell_sequence.size())) {
      ix = 0;
    }
    if (time <= pi_end) {
      heal_sum += (heal_sum - heal_sum_start)* 0.2f;
    }
  }
}

float HpsPvp(const PriestCharacter& c)
{
  std::vector<Spell> spells;
  int max_rank = -1;
  spells.push_back(Renew(c, max_rank));
  spells.push_back(Renew(c, max_rank));  // 3
  spells.push_back(Shield(c, max_rank));
  spells.push_back(FlashHeal(c, max_rank));  // 6
  spells.push_back(FlashHeal(c, max_rank)); 
  spells.push_back(FlashHeal(c, max_rank));  // 9
  spells.push_back(Shield(c, max_rank));
  spells.push_back(FlashHeal(c, max_rank));  // 12
  spells.push_back(FlashHeal(c, max_rank));
  spells.push_back(FlashHeal(c, max_rank));  // 15
  spells.push_back(FlashHeal(c, max_rank));
  spells.push_back(FlashHeal(c, max_rank));
  spells.push_back(FlashHeal(c, max_rank));

  Stats s(c);
  return Hps(c, spells, 17.0f, 0.1f*s.getMaxMana());
}


std::vector<Spell> PveHealingSequence(const PriestCharacter& c)
{
  std::vector<Spell> spells;
  int max_rank = -1;
  int poh_targets = 5;
  spells.push_back(Heal(c, 2));
  spells.push_back(Heal(c, 2));
  spells.push_back(GreaterHeal(c, 1));
  spells.push_back(Renew(c, max_rank));
  spells.push_back(GreaterHeal(c, max_rank));

  spells.push_back(Heal(c, 2));
  spells.push_back(Heal(c, 2));
  spells.push_back(GreaterHeal(c, 1));
  spells.push_back(PrayerOfHealing(c, max_rank, poh_targets));
  spells.push_back(FlashHeal(c, max_rank));

  spells.push_back(Heal(c, 2));
  spells.push_back(Heal(c, 2));
  spells.push_back(GreaterHeal(c, 1));
  spells.push_back(Renew(c, max_rank));
  spells.push_back(GreaterHeal(c, max_rank));

  spells.push_back(Heal(c, 2));
  spells.push_back(Heal(c, 2));
  spells.push_back(GreaterHeal(c, 1));
  spells.push_back(Renew(c, max_rank));
  spells.push_back(FlashHeal(c, max_rank));

  spells.push_back(Heal(c, 2));
  spells.push_back(Heal(c, 2));
  spells.push_back(GreaterHeal(c, 1));
  spells.push_back(GreaterHeal(c, max_rank));

  return spells;
}

Spell IxToSpell(const PriestCharacter& c, int choice_ix)
{
  int max_rank = -1;
  int poh_targets = 5;
  switch (choice_ix) {
    case 0:
      return Heal(c, 2);
    case 1:
      return GreaterHeal(c, 1);
    case 2:
      return GreaterHeal(c, max_rank);
    case 3:
      return FlashHeal(c, max_rank);
    case 4:
      return Renew(c, max_rank);
    case 5:
      return PrayerOfHealing(c, max_rank, poh_targets);
    default:
      return Heal(c, 2);
  }
}

int SpellToIx(const Spell& spell)
{
  auto s = spell.name;
  auto l = spell.level_req;
  if (s == "Heal") return 0;
  if (s == "Greater Heal" && l != 60) return 1;
  if (s == "Greater Heal" && l == 60) return 2;
  if (s == "Flash Heal") return 3;
  if (s == "Renew") return 4;
  if (s == "Prayer of Healing") return 5;
  return 0;
}



// See above: spellCounts order: h2, g1, gm, fh, re, poh
// We need spell counts in item picker
// Those will be adjusted whenever a new set is found: 
// try increment, continue as long as score is >=
// try decrement, continue as long as score is >=

std::vector<Spell> PveHealingSequence(const PriestCharacter& c, const std::vector<float>& spell_counts)
{
  float total_count = std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f);
  bool verbose = false;

  if (verbose) {
    std::cout << "Counts to sequence, counts: "
        << spell_counts[0] << " " << spell_counts[1] << " "
        << spell_counts[2] << " " << spell_counts[3] << " "
        << spell_counts[4] << " " << spell_counts[5] << std::endl;
  }



  int n_spell_types = spell_counts.size();
  std::vector<float> added_spell_counts(n_spell_types);
  float added_total_count = 0.0f;

  std::vector<Spell> spells;
  // std::cout << "Spells: " << std::endl;
  while (static_cast<float>(spells.size()) < total_count) {
    int choice_ix = 0;
    float best_freq_diff = -1.0f;
    for (int i = 0; i < n_spell_types; ++i) {
      if (added_spell_counts[i] < spell_counts[i]) {
        float freq_diff = spell_counts[i]/total_count - added_spell_counts[i]/std::max<float>(1.0f, added_total_count);
        // std::cout << "i: " << i << "spell_counts[i]: " << spell_counts[i] << ", total_count: " << total_count
            // << ", spell_counts[i]/total_count: " << spell_counts[i]/total_count
            // << ", added_spell_counts[i]: " << added_spell_counts[i] << ", added_total_count: " << added_total_count
            // << ", added_spell_counts[i]/std::max<float>(1.0f, added_total_count): "
            // << added_spell_counts[i]/std::max<float>(1.0f, added_total_count) << ", freq_diff: " << freq_diff
            // << std::endl;
        if (freq_diff > best_freq_diff) {
          best_freq_diff = freq_diff;
          choice_ix = i;
        }
      }
    }
    added_spell_counts[choice_ix] += 1.0f;
    added_total_count += 1.0f;
    spells.push_back(IxToSpell(c, choice_ix));
    if (verbose) std::cout << "Spell name: " << IxToSpell(c, choice_ix).name << ", ix: " << choice_ix
      << ", best_freq_diff: " << best_freq_diff << std::endl;
  }
  if (spells.size() != total_count) {
    std::cout << "spells.size(): " << spells.size() << ", total_count: " << total_count << std::endl;
  }
  assert(spells.size() == total_count);
  return spells;
}


float FindBestManaToRegenMul(const PriestCharacter& c, 
                             const std::vector<float>& spell_counts, 
                             float combat_length, 
                             float init_mana_to_regen_mul)
{
  float best_mul = init_mana_to_regen_mul;
  Stats stats(c);
  float best_score = Hps(c, PveHealingSequence(c, spell_counts), combat_length,
                         stats.getMaxMana()*best_mul);
  for (float mul = 0.0f; mul <= 0.95f; mul += 0.1f) {
    float score = Hps(c, PveHealingSequence(c, spell_counts), combat_length,
                         stats.getMaxMana()*mul);
    if (score > best_score) {
      best_mul = mul;
      best_score = score;
    }
  }
  bool verbose = false;
  if (verbose) {
    std::cout << "combat_length: " << combat_length
        << "best_mul: " << best_mul
        << ", spell counts: "
        << spell_counts[0] << " " << spell_counts[1] << " "
        << spell_counts[2] << " " << spell_counts[3] << " "
        << spell_counts[4] << " " << spell_counts[5] << "/"
        << 0.1f*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)
        << ", score: " << best_score << std::endl;
  }
  return best_mul;
}

float FindBestManaToRegenMulNew(const PriestCharacter& c, 
                             const std::vector<float>& spell_counts, 
                             float combat_length, 
                             float init_mana_to_regen_mul)
{
  // return 0.5f;
  float best_mul = init_mana_to_regen_mul;
  Stats stats(c);
  float best_score = Hps(c, PveHealingSequence(c, spell_counts), combat_length,
                         stats.getMaxMana()*best_mul);
  float step_size = 0.05f;
  for (float mul = best_mul; mul <= 0.95f; mul += step_size) {
    float score = Hps(c, PveHealingSequence(c, spell_counts), combat_length,
                         stats.getMaxMana()*mul);
    if (score >= best_score) {
      best_mul = mul;
      best_score = score;
    } else {
      break;
    }
  }

  for (float mul = best_mul; mul >= 0.0f; mul -= step_size) {
    float score = Hps(c, PveHealingSequence(c, spell_counts), combat_length,
                         stats.getMaxMana()*mul);
    if (score >= best_score) {
      best_mul = mul;
      best_score = score;
    } else {
      break;
    }
  }
  bool verbose = false;
  if (verbose) {
    std::cout << "combat_length: " << combat_length
        << "best_mul: " << best_mul
        << ", spell counts: "
        << spell_counts[0] << " " << spell_counts[1] << " "
        << spell_counts[2] << " " << spell_counts[3] << " "
        << spell_counts[4] << " " << spell_counts[5] << "/"
        << 0.1f*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)
        << ", score: " << best_score << std::endl;
  }
  return best_mul;
}

namespace globals
{
  std::chrono::time_point<std::chrono::system_clock> first_call_start;
  double find_best_pve_healing_counts_time_sum = 0.0;
  int64_t visit_count = 0;
}  // namespace globals


std::vector<float> FindBestPveHealingCounts(const PriestCharacter& c, 
                                              const std::vector<float>& initial_spell_counts,
                                              float combat_length,
                                              float *mana_to_regen_mul)
{
  // bool quick = true;
  bool quick = false;
  if (quick) {
    *mana_to_regen_mul = 0.0f;
    return {25,5,5,0,4,0};
  }
  auto t0 = std::chrono::system_clock::now();
  if (globals::find_best_pve_healing_counts_time_sum == 0.0f) {
    globals::first_call_start = std::chrono::system_clock::now();
  }
  bool verbose = false;
  int n_spell_types = 6;
  std::vector<float> spell_counts = initial_spell_counts;
  assert(n_spell_types == static_cast<int>(spell_counts.size()));
  int poh_ix = 5;
  Stats stats(c);
  std::vector<float> best_spell_counts = spell_counts;
  float best_score = Hps(c, PveHealingSequence(c, best_spell_counts), combat_length,
                         stats.getMaxMana()*(*mana_to_regen_mul));
  bool converged = false;
  constexpr float poh_max_freq = 0.0f;
  // spell_counts[poh_ix] = std::min(spell_counts[poh_ix], 
                                  // poh_max_freq*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f));
  while (!converged) {
    auto counts_at_start = best_spell_counts;
    auto mul_at_start = *mana_to_regen_mul;
    float mtr_mul = FindBestManaToRegenMul(c, spell_counts, combat_length, mul_at_start);
    
    // Maybe search for best mana to regen mul on each score call? More robust steps if perf problem?
    for (int ix = 0; ix < n_spell_types; ++ix) {
      float score = 0.0f;

      // Try finding better sequence by increasing count for spell
      spell_counts = best_spell_counts;
      do {
        spell_counts[ix] += 1.0f;
        if (spell_counts[ix] > 100.0f) {
          break;
        }
        mtr_mul = FindBestManaToRegenMul(c, spell_counts, combat_length, mtr_mul);
        score = Hps(c, PveHealingSequence(c, spell_counts), combat_length, stats.getMaxMana()*mtr_mul);
        if (spell_counts[poh_ix] > poh_max_freq*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)) {
          score = 0.0f;
        }
        if (score > best_score) {
          if (verbose) {
            std::cout << "new best sequence for combat_length: " << combat_length << ", score: " << score << ", counts: "
                << spell_counts[0] << " " << spell_counts[1] << " "
                << spell_counts[2] << " " << spell_counts[3] << " "
                << spell_counts[4] << " " << spell_counts[5] << "/"
                << 0.1f*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f) << std::endl;
          }
          best_spell_counts = spell_counts;
          best_score = score;
          *mana_to_regen_mul = mtr_mul;
        } else {
          break;
        }
      } while (1);


      // Try finding better sequence by decreasing count for spell
      spell_counts = best_spell_counts;
      do {
        spell_counts[ix] -= 1.0f;
        if (spell_counts[ix] < 0.0f) {
          break;
        }
        if (std::accumulate(spell_counts.begin(), (spell_counts.end() - 1), 0.0f) < 1.0f) {
          break;
        }
        mtr_mul = FindBestManaToRegenMul(c, spell_counts, combat_length, mtr_mul);
        score = Hps(c, PveHealingSequence(c, spell_counts), combat_length, stats.getMaxMana()*mtr_mul);
        if (spell_counts[poh_ix] > poh_max_freq*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)) {
          score = 0.0f;
        }
        if (score > best_score) {
          if (verbose) {
            std::cout << "new best sequence for combat_length: " << combat_length << ", score: " << score << ", counts: "
                << spell_counts[0] << " " << spell_counts[1] << " "
                << spell_counts[2] << " " << spell_counts[3] << " "
                << spell_counts[4] << " " << spell_counts[5] << "/"
                << 0.1f*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f) << std::endl;
          }
          best_spell_counts = spell_counts;
          best_score = score;
          *mana_to_regen_mul = mtr_mul;
        } else {
          break;
        }
      } while (1);
    }
    bool same = true;
    for (int ix = 0; ix < n_spell_types; ++ix) {
      same &= best_spell_counts[ix] == counts_at_start[ix];
    }
    same &= mul_at_start == *mana_to_regen_mul;
    converged = same;
  }

  auto t1 = std::chrono::system_clock::now();
  double this_round_s = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()*1e-9;
  globals::find_best_pve_healing_counts_time_sum += this_round_s;

  bool cout_time = globals::visit_count++ % 1000 == 0;
  if (cout_time) {
    double total_s = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - globals::first_call_start).count()*1e-9;
    std::cout << "[Perf] spent in FindBestPveHealingCounts: " << globals::find_best_pve_healing_counts_time_sum << " s";
    std::cout << ", that's: " << globals::find_best_pve_healing_counts_time_sum/total_s*100.0 << " \% of total." << std::endl;
  }

  return best_spell_counts;
}

float HpsPve(const PriestCharacter& c, float combat_length)
{
  auto spells = PveHealingSequence(c);
  Stats s(c);
  return Hps(c, spells, combat_length, 0.1f*s.getMaxMana());
}



}  // namespace css

