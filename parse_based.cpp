#include "parse_based.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "assumptions.h"
#include "log_entry.h"
#include "priest_character.h"
#include "spells_priest_holy.h"
#include "stats.h"

namespace css
{

void HandleLogEntry(const LogEntry& e, std::map<std::string, float>* d, std::map<std::string, float>* damage_taken)
{
  // NOTE: this is probably not actually needed due to how maps work in practice
  // if (d->find(e.player) != d->end()) {
    // (*d)[e.player] = 0.0f;
    // (*damage_taken)[e.player] = 0.0f;
  // }
  
  (*d)[e.player] += -1.0f*e.hp_diff;
  if ((*d)[e.player] < 0.0f) {
    (*d)[e.player] = 0.0f;
  }

  if (e.hp_diff < 0.0f) {
    (*damage_taken)[e.player] += -1.0f*e.hp_diff;
  }
}

void DamageTakenDecay(const float time_step, std::map<std::string, float>* damage_taken)
{
  constexpr float w_per_s = 0.2f;  // -> around last 5 seconds of damage taken present at each time;

  // TODO do the math and make this more accurate cba now
  const float mul = time_step*w_per_s;
  for (auto& entry : *damage_taken) {
    entry.second *= (1.0f - mul);
  }
}

struct MyCast : public LogEntry {
  float cost;
  bool done;
  std::string spell_name;
  int spell_rank;
};


void PickBestPreCast(const PriestCharacter &c, float mana, int64_t time, const std::map<std::string, float>& deficits, //
                     const std::map<std::string, float>& damage_taken, const std::vector<Spell>& spells,               //
                     MyCast* my_cast)
{
  if (damage_taken.empty()) return;
  float max_val = 0.0f;
  std::string player = "";

  for (const auto& dt : damage_taken) {
    float deficit = 0.0f;
    if (deficits.find(dt.first) != deficits.end()) {
      deficit = deficits.at(dt.first);
    }

    // Damage taken reflects roughly last 5 seconds
    // Pick one with max (deficit + dps_in)
    float val = dt.second * 0.2f + deficit;
    if (val > max_val) {
      max_val = val;
      player = dt.first;
    }
  }
  
  // Pick spell with healing closest to max_val
  const Spell* best_spell = &spells.front();
  float min_d = fabs(best_spell->healing - max_val);
  for (const auto& spell : spells) {
    float d = fabs(spell.healing - max_val);
    if (d < min_d) {
      best_spell = &spell;
      min_d = d;
    }
  }

  my_cast->player = player;
  my_cast->hp_diff = best_spell->healing;
  my_cast->time = best_spell->cast_time*1e3 + time;
  my_cast->cost = best_spell->cost;
  my_cast->done = false;
  my_cast->spell_name = best_spell->name;
  my_cast->spell_rank = best_spell->rank;

}

// TODO: should keep track of the biggest deficit target i guess or sth, for perf
void PickBestCastIfAny(const PriestCharacter& c, float mana, int64_t time, const std::map<std::string, float>& deficits, //
                       const std::vector<Spell>& spells, MyCast* my_cast)
{
  if (deficits.empty()) return;
  // just pick the target with biggest deficit
  std::string player = deficits.cbegin()->first;
  float deficit = deficits.cbegin()->second;
  for (const auto &d : deficits) {
    if (d.second > deficit) {
      player = d.first;
      deficit = d.second;
    }
  }

  my_cast->hp_diff = 0;
  for (const Spell& spell : spells) {
    if (deficit > spell.healing && spell.healing > my_cast->hp_diff && mana > spell.cost) {
      my_cast->player = player;
      my_cast->hp_diff = spell.healing;
      my_cast->time = spell.cast_time*1e3 + time;
      my_cast->cost = spell.cost;
      my_cast->done = false;
      my_cast->spell_name = spell.name;
      my_cast->spell_rank = spell.rank;
    }
  }
}

LogResult SimpleLogHealing(const PriestCharacter& c, const std::vector<LogEntry>& log, float time_step,
                                         float oh_limit, float time_left_mul)
{
  LogResult out;
  if (log.empty()) return out;

  std::map<std::string, float> deficits;
  std::map<std::string, float> damage_taken;
  const int64_t time_step_ms = std::max<int64_t>(1, time_step * 1e3);

  // take first log entry -> combat start
  bool announce = false;
  bool done = false;
  int64_t time = log[0].time;
  const int64_t start_time = time;
  const int64_t end_time = log.back().time;
  int64_t log_ix = 0;
  const int64_t log_s = static_cast<int64_t>(log.size());

  MyCast my_cast;
  my_cast.time = 0;
  my_cast.hp_diff = 0.0f;
  my_cast.player = "Paisti";
  my_cast.cost = 0.0f;
  my_cast.done = true;
  my_cast.spell_name = "Unreal";
  my_cast.spell_rank = 1;


  std::vector<Spell> spells_fast;
  spells_fast.push_back(FlashHeal(c, 1));
  spells_fast.push_back(FlashHeal(c, 2));
  spells_fast.push_back(FlashHeal(c, 3));
  spells_fast.push_back(FlashHeal(c, 4));
  spells_fast.push_back(FlashHeal(c, 5));
  spells_fast.push_back(FlashHeal(c, 6));
  spells_fast.push_back(FlashHeal(c, 7));

  // cheaper / longer spells that will more likely get cancelled and thus conserve mana
  std::vector<Spell> spells_hpm;
  spells_hpm.push_back(Heal(c, 2));
  spells_hpm.push_back(Heal(c, 4));
  spells_hpm.push_back(GreaterHeal(c, 1));
  spells_hpm.push_back(GreaterHeal(c, 4));

  for (const auto& spell : spells_fast) {
    std::string spell_id = spell.name + " " + std::to_string(spell.rank);
    out.spell_id_to_spell[spell_id] = spell;
  }

  for (const auto& spell : spells_hpm) {
    std::string spell_id = spell.name + " " + std::to_string(spell.rank);
    out.spell_id_to_spell[spell_id] = spell;
  }

  Stats s(c);
  const float max_mana = s.getMaxMana();
  float mana = max_mana;
  int64_t prev_cast = 0;
  int64_t prev_tick = start_time;
  int64_t prev_log_time_ms = start_time;
  int64_t in_combat_thr_ms = 2e3;
  int64_t prev_damage_taken_ms = 0;

  const bool precast = global::assumptions.precast;
  const bool swap_cast = global::assumptions.swap_cast;

  while (log_ix < log_s) {
    // while log entry stamp =< time, handle log entries
    while (log_ix < log_s && time >= log[log_ix].time) {
      HandleLogEntry(log[log_ix], &deficits, &damage_taken);
      if (log[log_ix].time - prev_log_time_ms < in_combat_thr_ms) {
        out.in_combat_sum += static_cast<float>(log[log_ix].time - prev_log_time_ms)/1e3;
      }
      // big pause -> you probably did drink really
      if (log[log_ix].time - prev_log_time_ms > 30e3) {
        mana = max_mana;
      }
      prev_log_time_ms = log[log_ix].time;
      if (log[log_ix].hp_diff < 0) {
        prev_damage_taken_ms = log[log_ix].time;
      }
      log_ix++;
    }
    const float relative_time_left = 1.0 - static_cast<double>(time - start_time)/(end_time - start_time);
    const float time_since_start_s = static_cast<float>(time - start_time)/1e3;
  
    // cancel oh
    if (!my_cast.done && my_cast.time > time) {
      if (deficits.find(my_cast.player) != deficits.end() && deficits[my_cast.player] < my_cast.hp_diff*(1.0f - oh_limit)) {
        // TODO if last tick or better heal available

        if (swap_cast) {
          MyCast better_cast;
          if (mana/max_mana > time_left_mul * relative_time_left) {
            PickBestCastIfAny(c, mana, time, deficits, spells_fast, &better_cast);
          } else {
            PickBestCastIfAny(c, mana, time, deficits, spells_hpm, &better_cast);
          }

          bool better_target_found = deficits[better_cast.player] > deficits[my_cast.player]*2.0f;

          if (better_target_found) {
            my_cast = better_cast;
          }
        }

        bool last_tick = time + time_step > my_cast.time;
        if (last_tick) {
          my_cast.done = true;
          if (announce) std::cout << "@ time: " << time_since_start_s << ", cancelled cast " << my_cast.spell_name 
            << " r" << my_cast.spell_rank << " for " << my_cast.hp_diff << " on " << my_cast.player 
                << ", would oh: " << my_cast.hp_diff - deficits[my_cast.player] << std::endl;
        }
      }
    }

    // if cast is finished, resolve and collect healing that was not overhealing
    if (!my_cast.done && my_cast.time <= time) {
      if (deficits.find(my_cast.player) != deficits.end()) {
        float deficit = deficits[my_cast.player];
        float healing = std::min(my_cast.hp_diff, deficit);
        out.heal_sum += healing;
        std::string spell_id = my_cast.spell_name + " " + std::to_string(my_cast.spell_rank);
        out.spell_casts[spell_id]++;
        out.spell_heal_sums[spell_id] += healing;
        deficits[my_cast.player] -= healing;
        mana -= my_cast.cost;
        my_cast.done = true;
        prev_cast = time;
        if (announce) std::cout << "@ time: " << time_since_start_s << " s, " << my_cast.spell_name << " r" << my_cast.spell_rank 
            << " healed: " << my_cast.player << " for " << healing << " (" 
            << my_cast.hp_diff - healing << " oh), " << deficit << " -> " << deficit - healing << ", mana left: " << mana << std::endl;
      }
    }

    // if not casting, pick best target and start casting
    if (my_cast.done) {
      // More mana than time left

      // Low time_left_mul -> keep casting fast heals
      if (mana/max_mana > time_left_mul * relative_time_left) {
        PickBestCastIfAny(c, mana, time, deficits, spells_fast, &my_cast);
        // If could not find target, pick precast target
        if (my_cast.done && precast) {
          PickBestPreCast(c, mana, time, deficits, damage_taken, spells_fast, &my_cast);
        }
      } else {
        // Same stuff for the hpm spells
        PickBestCastIfAny(c, mana, time, deficits, spells_hpm, &my_cast);
        if (my_cast.done && precast) {
          PickBestPreCast(c, mana, time, deficits, damage_taken, spells_hpm, &my_cast);
        }
      }
    }

    // mana regen
    if (time >= prev_tick + 2e3 ) {
      if (time >= prev_cast + 5e3) {
        mana += s.getManaRegenTickOutOfFsr();
      } else {
        mana += s.getManaRegenTickUnderFsr();
      }
      prev_tick = time;

      // You are probably drinking. We only drink champagne here.
      if (time - prev_damage_taken_ms > 10e3) {
        mana += max_mana*0.04;
      }
      mana = std::min(max_mana, mana);
    }

    // Decay for damage taken so recent damage is more heavily weighted
    if (precast) {
      DamageTakenDecay(time_step, &damage_taken);
    }

    // time goes on
    time += time_step_ms;
  }
  if (announce) std::cout << "Total time in combat: " << out.in_combat_sum << " s." << std::endl;
  return out;
}

LogsType PrunedLog(const std::vector<LogEntry>& log, const std::string& remove_player)
{
  if (log.empty()) return std::vector<std::vector<LogEntry>>{};

  int64_t max_diff_ms = global::assumptions.max_log_entry_diff_in_combat * 1e3;
  int64_t min_combat_len_ms = global::assumptions.min_combat_length*1e3;
  std::vector<std::vector<LogEntry>> out;
  int64_t prev_t_ms = log[0].time;
  std::vector<LogEntry> this_combat;
  int64_t start_time = prev_t_ms;
  for (const auto& e : log) {
    if (e.player.find(" ") == std::string::npos && e.player != remove_player) {
      this_combat.push_back(e);
    }
    if (e.time - prev_t_ms > max_diff_ms) {
      if (this_combat.back().time - this_combat.front().time > min_combat_len_ms) {
        out.push_back(std::vector<LogEntry>{});
        std::cout << "Combat from " << (this_combat.front().time - start_time)/1e3 << " to " << (this_combat.back().time - start_time)/1e3 << " s" << std::endl;
        for (auto& e : this_combat) {
          out.back().push_back(e);
        }
      }
      this_combat.clear();
    }
    prev_t_ms = e.time;
  }
  return out;
}


LogsType GetLogs(const std::string& log_fn)
{
  std::ifstream is(log_fn.c_str());
  std::string line;
  float diff_sum = 0.0f;
  float heal_sum = 0.0f;
  float damage_sum = 0.0f;
  int64_t ms_start = 0;
  int64_t ms_end = 0;
  
  std::vector<LogEntry> log;
  int64_t lines = 0;
  while (std::getline(is, line)) {
    LogEntry e;
    if (LineToLogEntryIfAny(line, &e)) {
      // std::cout << "ignored line: " << line << std::endl;
      diff_sum += e.hp_diff;
      if (ms_start == 0) {
        ms_start = e.time;
      }
      if (e.hp_diff < 0) {
        damage_sum -= e.hp_diff;
      } else {
        heal_sum += e.hp_diff;
      }
      ms_end = e.time;
      log.push_back(e);
    }
    lines++;
  }
  float time_s = (ms_end - ms_start)*1e-3f;
  is.close();
  std::cout << "Saw " << lines << " lines of log." << std::endl;
  std::cout << "diff_sum: " << diff_sum << std::endl;
  std::cout << "/40: " << diff_sum/40.0f << std::endl;
  std::cout << "heal_sum: " << heal_sum << " -> hps: " << heal_sum/time_s << std::endl;
  std::cout << "damage_sum: " << damage_sum << " -> dtps: " << damage_sum/time_s << std::endl;
  std::cout << "time_s: " << time_s << std::endl;

  std::cout << "Pruning log from: " << log.size() << " entries." << std::endl;
  std::string remove_player = global::assumptions.skip_player;
  auto logs = PrunedLog(log, remove_player);
  int total_size = 0;
  for (auto one_log : logs) {
    total_size += one_log.size();
  }
  std::cout << "Pruned to: " << total_size << " entries." << std::endl;
  return logs;
}

LogResult HpsForLogs(const PriestCharacter& c, float oh_limit, float time_left_mul, const LogsType& logs)
{
  LogResult out;
  float heal_sum = 0.0f;
  float time_sum = 0.0f;

  const float time_step = global::assumptions.time_step;
  const float time_min_s = global::assumptions.min_combat_length;
  for (const auto& log : logs) {
    float log_time = (log.back().time - log.front().time) / 1e3;
    if (log_time > time_min_s) {
      auto res = SimpleLogHealing(c, log, time_step, oh_limit, time_left_mul);
      if (res.in_combat_sum > time_min_s) {
        out.in_combat_sum += res.in_combat_sum;
        out.heal_sum += res.heal_sum;
        for (const auto& entry : res.spell_casts) {
          out.spell_casts[entry.first] += entry.second;
        }
        for (const auto& entry : res.spell_heal_sums) {
          out.spell_heal_sums[entry.first] += entry.second;
        }
        if (out.spell_id_to_spell.empty()) {
          out.spell_id_to_spell = res.spell_id_to_spell;
        }
        // std::cout << "HPS: " << res.first/res.second << std::endl;
      }
    }
  }
  // std::cout << "oh_limit: " << oh_limit << ", time_left_mul: " << time_left_mul << " -> total hps: " << heal_sum/time_sum << std::endl;
  return out;
}

std::pair<float, float> FindBestOhLimitAndTimeLeftMul(const PriestCharacter& c, const LogsType& logs)
{
  const float time_min_s = 30.0f;
  float best_hps = 0.0f;
  float best_oh_limit = 0.0f;
  float best_time_left_mul = 0.0f;

  for (float oh_limit = 0.1f; oh_limit <= 0.95f; oh_limit += 0.1f) {
    for (float time_left_mul = 0.1f; time_left_mul <= 3.0f; time_left_mul += 0.2f) {
      auto res = HpsForLogs(c, oh_limit, time_left_mul, logs);
      float hps = res.heal_sum/res.in_combat_sum;
      if (hps > best_hps) {
        best_hps = hps;
        best_oh_limit = oh_limit;
        best_time_left_mul = time_left_mul;
      }
    }
  }
  // std::cout << "best hps: " << best_hps << ", oh_limit: " << best_oh_limit << ", time_left_mul: " << best_time_left_mul << std::endl;
  return {best_oh_limit, best_time_left_mul};

}

void ParseBased(const std::string& log_fn)
{
  auto logs = GetLogs(log_fn);

  float time_step = 0.2f;
  auto c = BaseLvl60HolyDiscHealing();
  c.sp = 700;
  c.intelligence = 400;
  c.spirit = 400;
  c.mp5 = 70;
  c.talents.meditation = 6;  // 3p T2
  FindBestOhLimitAndTimeLeftMul(c, logs);
}

}  // namespace css
