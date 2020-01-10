#include "hps.h"

#include <cassert>
#include <cmath>
#include <chrono>
#include <iostream>
#include <numeric>

#include "assumptions.h"
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

int HandleManaRegen(float time, float last_cast_time, const Stats& stats, float* mana, int* regen_ticks)
{
  int full_ticks = 0;
  while ((*regen_ticks + 1)*2.0f <= time) {
    if (last_cast_time + 5.0f < time) {
      *mana += stats.getManaRegenTickOutOfFsr();
      full_ticks++;
    } else {
      *mana += stats.getManaRegenTickUnderFsr();
    }
    (*regen_ticks)++;
  }

  if (*mana > stats.getMaxMana()) {
    *mana = stats.getMaxMana();
  }
  return full_ticks;
}

float RemainingManaAsHealing(const PriestCharacter& c, float in_full_regen, float mana)
{
  Spell heal = GreaterHeal(c, 4);
  // Spell heal = FlashHeal(c, -1);
  float hps = heal.healing/heal.cast_time;
  float spell_cost_per_s = heal.cost/heal.cast_time;

  Stats stats(c);
  float regen_cost_per_s = 0.5*(stats.getManaRegenTickSpiritTapOutOfFsr() - stats.getManaRegenTickUnderFsr());

  float healing_out = 0.0;
  for (float t = 0.0; t <= in_full_regen; t += 0.1f) {
    if (mana > t*(spell_cost_per_s + regen_cost_per_s)) {
      healing_out = t*hps;
    }
  }
  return healing_out;
}
}  // namespace

std::pair<float, float> HpsWithRegen(const PriestCharacter& c, const std::vector<Spell>& spell_sequence, float end_at_s, Regen regen)
{
  int casts = regen.casts;
  int ticks = regen.ticks;
  int ticks_oom = regen.ticks_oom;
  std::vector<Spell> hots;
  float heal_sum = 0.0f;
  float time = 0.0f;
  float last_cast_time = 0.0f;
  int regen_ticks = 0;
  Stats stats(c);
  float mana = stats.getMaxMana();
  float in_full_regen = 0.0f;
  bool verbose = false;
  float heal_sum_before_oom_or_end = 0.0f;

  float pi_end = -180.0f + 15.0f;
  if (c.talents.power_infusion && time >= pi_end + 180.0f - 15.0f) {
    pi_end = time + 15.0f;
    time += 1.5f;
    mana -= 0.2*c.base_mana;
  }

  int ix = 0;
  int curr_casts = 0;
  int total_ticks_oom = 0;
  while (1) {
    if (curr_casts >= casts) {
      curr_casts = 0;
      bool break_main_loop = false;
      for (int curr_tick = 0; curr_tick < ticks; ++curr_tick) {
        time += 2.0f;
        float pre = heal_sum;
        HandleHots(time, &hots, &heal_sum);
        if (heal_sum > pre && verbose) std::cout << "hots while regen, sum: " << heal_sum << ", mana: " << mana 
          << ", time: " << time << ", tick: " << curr_tick << "/" << ticks << std::endl;
        in_full_regen += 2.0*HandleManaRegen(time, last_cast_time, stats, &mana, &regen_ticks);
        if (time >= end_at_s) {
          break_main_loop = true;
          break;
        }
      }
      if (break_main_loop) {
        break;
      }
    }
    curr_casts++;

    if (c.talents.power_infusion && time >= pi_end + 180.0f - 15.0f && mana > 0.2*c.base_mana) {
      pi_end = time + 15.0f;
      time += 1.5f;
      mana -= 0.2*c.base_mana;
      if (verbose) std::cout << "Pi started, time: " << time << ", pi_end: " << pi_end << ", mana: " << mana << std::endl;
    }

    Spell spell = spell_sequence[ix];
    if (time + 3.0f < pi_end) {
      spell = GreaterHeal(c, -1);
      spell.healing *= 1.2;
      ix--;
    }
    if (spell.cost > mana) {
      if (heal_sum_before_oom_or_end == 0.0f) {
        heal_sum_before_oom_or_end = heal_sum;
      }
      bool break_main_loop = false;
      int ticks_in_loop = 0;
      while (mana < spell.cost || ticks_in_loop < ticks_oom) {
        time += 2.0f;
        float pre = heal_sum;
        HandleHots(time, &hots, &heal_sum);
        if (heal_sum > pre && verbose) std::cout << "hots while regen, sum: " << heal_sum << ", mana: " << mana //
          << ", spell.cost: " << spell.cost << ", time: " << time << std::endl;
        in_full_regen += 2.0*HandleManaRegen(time, last_cast_time, stats, &mana, &regen_ticks);
        ticks_in_loop++;
        total_ticks_oom++;
        if (time >= end_at_s) {
          break_main_loop = true;
          break;
        }
      }
      if (break_main_loop) {
        break;
      }
    }
    mana -= spell.cost;
    if (mana < 0 && verbose) {
      std::cout << "Went to negative mana with spell: " << spell.name << std::endl;
    }
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
      if (verbose) std::cout << "spell: " << spell.name << ", sum: " << heal_sum << ", mana: " << mana << ", time: " << time << std::endl;
    }

    float pre = heal_sum;
    HandleHots(time, &hots, &heal_sum);
    if (heal_sum > pre && verbose) std::cout << "hots, sum: " << heal_sum << ", mana: " << mana << ", time: " << time << std::endl;
    in_full_regen += 2.0*HandleManaRegen(time, last_cast_time, stats, &mana, &regen_ticks);
    if (time >= end_at_s) {
      break;
    }
    ix++;
    if (ix >= static_cast<int>(spell_sequence.size())) {
      ix = 0;
    }
  }

  float regen_penalty_mul = 1.0f;
  if (in_full_regen/time > global::assumptions.full_regen_limit) {
    regen_penalty_mul = (1.0 - (in_full_regen/time - global::assumptions.full_regen_limit));
  }

  float from_rem_mana_a = RemainingManaAsHealing(c, in_full_regen, mana);

  if (c.talents.power_infusion && time >= pi_end + 180.0f - 15.0f && mana > 0.2*c.base_mana) {
    pi_end = time + 15.0f;
    in_full_regen -= 1.5f;
    mana -= 0.2*c.base_mana;
  }

  float from_rem_mana_b = RemainingManaAsHealing(c, in_full_regen, mana);
  if (time < pi_end) {
    from_rem_mana_b *= 1.2;
  }
  float from_rem_mana = std::max(from_rem_mana_a, from_rem_mana_b);
  if (verbose) std::cout << "mana: " << mana << ", in_full_regen: " << in_full_regen << ", from_rem_mana_a: " << from_rem_mana_a
      << ", from_rem_mana_b: " << from_rem_mana_b << ", heal_sum: " << heal_sum << " -> ";
  if (heal_sum_before_oom_or_end == 0.0f) {
    heal_sum_before_oom_or_end = heal_sum;
  }
  heal_sum += from_rem_mana;
  if (verbose) std::cout << heal_sum << ", time: " << time << ", hps: " << heal_sum/time << std::endl;

  // float info = heal_sum_before_oom_or_end/heal_sum;
  float oom_penalty_mul = 1.0f - (total_ticks_oom*2.0f)/(time*5);

  float info = oom_penalty_mul*regen_penalty_mul;
  heal_sum *= info;

  return {heal_sum/time, info};
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
  Regen regen;
  regen.casts = 100;
  regen.ticks = 0;
  regen.ticks_oom = 0;
  return HpsWithRegen(c, spells, 17.0f, regen).first;
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
  int poh_targets = 3;
  switch (choice_ix) {
    case 0:
      return Heal(c, 2);
    case 1:
      return Heal(c, 4);
    case 2:
      return GreaterHeal(c, 1);
    case 3:
      return GreaterHeal(c, 4);
    case 4:
      return FlashHeal(c, max_rank);
    case 5:
      return Renew(c, max_rank);
    case 6:
      return PrayerOfHealing(c, max_rank, poh_targets);
    default:
      return Heal(c, 2);
  }
}

int SpellToIx(const Spell& spell)
{
  auto s = spell.name;
  auto l = spell.level_req;
  auto r = spell.rank;
  if (s == "Heal" && r == 2) return 0;
  if (s == "Heal" && r == 4) return 1;
  if (s == "Greater Heal" && l != 60) return 2;
  if (s == "Greater Heal" && l == 60) return 3;
  if (s == "Flash Heal") return 4;
  if (s == "Renew") return 5;
  if (s == "Prayer of Healing") return 6;
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

Regen FindBestRegen(const PriestCharacter& c, 
                    const std::vector<float>& spell_counts, 
                    float combat_length, 
                    Regen current_regen) 
{
  // constexpr int max_casts = 100;
  // constexpr int max_ticks = 100;
  constexpr int max_casts = 20;
  constexpr int max_ticks = 10;
  // constexpr int max_ticks_oom = 4*max_ticks;
  constexpr int max_ticks_oom = max_ticks;
  // constexpr int max_ticks_oom = 0;
 
  current_regen.ticks = std::min(max_ticks, current_regen.ticks);
  current_regen.casts = std::min(max_casts, current_regen.casts);
  current_regen.ticks_oom = std::min(max_ticks_oom, current_regen.ticks_oom);
  Stats stats(c);
  float best_score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                                  current_regen).first;
 
  int step_size = 1;
  int best_casts = current_regen.casts;
  int best_ticks = current_regen.ticks;
  int best_ticks_oom = current_regen.ticks_oom;
  // casts +
  for (int casts = best_casts; casts <= max_casts; casts += step_size) {
    float score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                               Regen(casts, best_ticks, best_ticks_oom)).first;
    if (score >= best_score) {
      best_casts = casts;
      best_score = score;
    } else {
      break;
    }
  }

  // casts -
  for (int casts = best_casts; casts > 0; casts -= step_size) {
    float score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                               Regen(casts, best_ticks, best_ticks_oom)).first;
    if (score >= best_score) {
      best_casts = casts;
      best_score = score;
    } else {
      break;
    }
  }

  // ticks +
  for (int ticks = best_ticks; ticks <= max_ticks; ticks += step_size) {
    float score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                               Regen(best_casts, ticks, best_ticks_oom)).first;
    if (score >= best_score) {
      best_ticks = ticks;
      best_score = score;
    } else {
      break;
    }
  }

  // ticks -
  for (int ticks = best_ticks; ticks >= 1; ticks -= step_size) {
    float score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                               Regen(best_casts, ticks, best_ticks_oom)).first;
    if (score >= best_score) {
      best_ticks = ticks;
      best_score = score;
    } else {
      break;
    }
  }

  // ticks_oom +
  for (int ticks_oom = best_ticks_oom; ticks_oom <= max_ticks_oom; ticks_oom += step_size) {
    float score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                               Regen(best_casts, best_ticks, ticks_oom)).first;
    if (score >= best_score) {
      best_ticks_oom = ticks_oom;
      best_score = score;
    } else {
      break;
    }
  }

  // ticks_oom -
  for (int ticks_oom = best_ticks_oom; ticks_oom >= 0; ticks_oom -= step_size) {
    float score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length,
                               Regen(best_casts, best_ticks, ticks_oom)).first;
    if (score >= best_score) {
      best_ticks_oom = ticks_oom;
      best_score = score;
    } else {
      break;
    }
  }

  bool verbose = false;
  if (verbose) {
    std::cout << "combat_length: " << combat_length
        << "best_casts: " << best_casts
        << "best_ticks: " << best_ticks
        << "best_ticks_oom: " << best_ticks_oom
        << ", spell counts: "
        << spell_counts[0] << " " << spell_counts[1] << " "
        << spell_counts[2] << " " << spell_counts[3] << " "
        << spell_counts[4] << " " << spell_counts[5] << "/"
        << 0.1f*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)
        << ", score: " << best_score << std::endl;
  }
  return Regen(best_casts, best_ticks, best_ticks_oom);
}

namespace global
{
  std::chrono::time_point<std::chrono::system_clock> first_call_start;
  double find_best_pve_healing_counts_time_sum = 0.0;
  int64_t visit_count = 0;
}  // namespace global

std::vector<float> InitialSpellCounts()
{
    return global::assumptions.initial_spell_counts;
}

std::vector<float> SpellMaxFreqs()
{
  return global::assumptions.spell_max_freqs;
}

Regen InitialRegen()
{
  auto v = global::assumptions.initial_regen_vals;
  return Regen(v[0], v[1], v[2]);
}

float FreqPenalty(const std::vector<float>& counts)
{
  const float sum = std::accumulate(counts.begin(), counts.end(), 0.0f);
  auto max_freqs = SpellMaxFreqs();
  const int s = counts.size();
  float penalty = 0.0f;
  constexpr float penalty_per_count = 100.0f;  // hps
  for (int i = 0; i < s; ++i) {
    if (counts[i] > max_freqs[i]*sum) {
      penalty += penalty_per_count*(counts[i] - max_freqs[i]*sum);
    }
  }
  return penalty;
}


std::vector<float> FindBestPveHealingCounts(const PriestCharacter& c, 
                                              const std::vector<float>& initial_spell_counts,
                                              float combat_length,
                                              Regen *regen)
{
  bool fixed = global::assumptions.use_fixed_spell_counts;
  if (fixed) {
    std::vector<float> spell_counts = global::assumptions.fixed_spell_counts;
    *regen = FindBestRegen(c, spell_counts, combat_length, *regen);
    return spell_counts;
  }
  auto t0 = std::chrono::system_clock::now();
  if (global::find_best_pve_healing_counts_time_sum == 0.0f) {
    global::first_call_start = std::chrono::system_clock::now();
  }
  bool verbose = false;
  std::vector<float> spell_counts = initial_spell_counts;
  int n_spell_types = spell_counts.size();
  Stats stats(c);
  std::vector<float> best_spell_counts = spell_counts;
  float best_score = HpsWithRegen(c, PveHealingSequence(c, best_spell_counts), combat_length,
                                  *regen).first;
  bool converged = false;
  while (!converged) {
    auto counts_at_start = best_spell_counts;
    auto regen_at_start = *regen;
    Regen regen_curr = FindBestRegen(c, spell_counts, combat_length, regen_at_start);
    
    // Maybe search for best mana to regen mul on each score call? More robust steps if perf problem?
    for (int ix = 0; ix < n_spell_types; ++ix) {
      float score = 0.0f;

      // Try finding better sequence by increasing count for spell
      spell_counts = best_spell_counts;
      while (1) {
        spell_counts[ix] += 1.0f;
        if (spell_counts[ix] > 100.0f) {
          spell_counts[ix] -= 1.0f;
          break;
        }
        // TODO check freqs as separate function
        regen_curr = FindBestRegen(c, spell_counts, combat_length, regen_curr);
        score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length, regen_curr).first;
        score -= FreqPenalty(spell_counts);
        if (score > best_score) {
          if (verbose) {
            auto max_freqs = SpellMaxFreqs();
            std::cout << "new best sequence for combat_length: " << combat_length << ", score: " << score
                << ", casts: " << regen_curr.casts << ", ticks: " << regen_curr.ticks
                << ", counts: ";
            for (int cout_spell_ix = 0; cout_spell_ix < n_spell_types; ++cout_spell_ix) {
              std::cout << spell_counts[cout_spell_ix] << " / " << floor(max_freqs[cout_spell_ix]*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)) << ", ";
            }
            std::cout << std::endl;
          }
          best_spell_counts = spell_counts;
          best_score = score;
          *regen = regen_curr;
        } else {
          break;
        }
      }


      // Try finding better sequence by decreasing count for spell
      spell_counts = best_spell_counts;
      while (1) {
        spell_counts[ix] -= 1.0f;
        if (spell_counts[ix] < 0.0f) {
          spell_counts[ix] += 1.0f;
          break;
        }
        if (std::accumulate(spell_counts.begin(), (spell_counts.end() - 1), 0.0f) < 1.0f) {
          spell_counts[0] += 1.0f;
          break;
        }
        if (std::accumulate(spell_counts.begin(), (spell_counts.end()), 0.0f) < 1.0f) {
          spell_counts[0] += 1.0f;
          break;
        }
        regen_curr = FindBestRegen(c, spell_counts, combat_length, regen_curr);
        score = HpsWithRegen(c, PveHealingSequence(c, spell_counts), combat_length, regen_curr).first;
        score -= FreqPenalty(spell_counts);
        if (score > best_score) {
          if (verbose) {
            auto max_freqs = SpellMaxFreqs();
            std::cout << "new best sequence for combat_length: " << combat_length << ", score: " << score
                << ", casts: " << regen_curr.casts << ", ticks: " << regen_curr.ticks
                << ", counts: ";
            for (int cout_spell_ix = 0; cout_spell_ix < n_spell_types; ++cout_spell_ix) {
              std::cout << spell_counts[cout_spell_ix] << " / " << floor(max_freqs[cout_spell_ix]*std::accumulate(spell_counts.begin(), spell_counts.end(), 0.0f)) << ", ";
            }
            std::cout << std::endl;
          }
          best_spell_counts = spell_counts;
          best_score = score;
          *regen = regen_curr;
        } else {
          break;
        }
      }
    }
    bool same = true;
    for (int ix = 0; ix < n_spell_types; ++ix) {
      same &= best_spell_counts[ix] == counts_at_start[ix];
    }
    same &= regen_at_start.casts == regen->casts;
    same &= regen_at_start.ticks == regen->ticks;
    converged = same;
  }

  auto t1 = std::chrono::system_clock::now();
  double this_round_s = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()*1e-9;
  global::find_best_pve_healing_counts_time_sum += this_round_s;

  bool cout_time = global::visit_count++ % 1000 == 0;
  if (cout_time) {
    double total_s = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - global::first_call_start).count()*1e-9;
    std::cout << "[Perf] spent in FindBestPveHealingCounts: " << global::find_best_pve_healing_counts_time_sum << " s";
    std::cout << ", that's: " << global::find_best_pve_healing_counts_time_sum/total_s*100.0 << " \% of total." << std::endl;
  }


  bool cout_result = false;
  if (cout_result) {
    std::cout << "len: " << combat_length
        << ", ca: " << regen->casts << ", t: " << regen->ticks
        << ", co: "
        << best_spell_counts[0] << " " << best_spell_counts[1] << " "
        << best_spell_counts[2] << " " << best_spell_counts[3] << " "
        << best_spell_counts[4] << " " << best_spell_counts[5]
        << ", s: " << best_score << std::endl;
  }
#if 0
  // should not need this check...
  double sum = 0.0;

  for (auto& count : best_spell_counts) {

    if (count < 0) {
      count = 0.0;
    }
    sum += count;
    std::cout << count << " ";
  }
  if (sum < 1.0) {
    best_spell_counts[0] = 1.0;
    std::cout << ", first to 1.";
  }
  std::cout << std::endl;
#endif

  return best_spell_counts;
}

}  // namespace css

