#include "parse_based.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "log_entry.h"
#include "priest_character.h"
#include "spells_priest_holy.h"
#include "stats.h"

namespace css
{

void HandleLogEntry(const LogEntry& e, std::map<std::string, float>* d)
{
  if (d->find(e.player) != d->end()) {
    (*d)[e.player] = 0.0f;
  }
  
  (*d)[e.player] = -1.0f*e.hp_diff;
  if ((*d)[e.player] < 0.0f) {
    (*d)[e.player] = 0.0f;
  }
}

struct MyCast : public LogEntry {
  float cost;
  bool done;
  std::string spell_name;
  int spell_rank;
};


// TODO: should keep track of the biggest deficit target i guess or sth
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

std::pair<float, float> SimpleLogHealing(const PriestCharacter& c, const std::vector<LogEntry>& log, float time_step,
                                         float oh_limit, float time_left_mul)
{
  if (log.empty()) return {0.0f, 0.0f};

  std::map<std::string, float> deficits;
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

  const auto best_spell_eu = FlashHeal(c, 7); 
  const auto lame_spell = Heal(c, 2);

  std::vector<Spell> spells_fast;
  spells_fast.push_back(FlashHeal(c, 1));
  spells_fast.push_back(FlashHeal(c, 4));
  spells_fast.push_back(FlashHeal(c, 7));

  // cheaper / longer spells that will more likely get cancelled and thus conserve mana
  std::vector<Spell> spells_hpm;
  spells_hpm.push_back(Heal(c, 2));
  spells_hpm.push_back(Heal(c, 4));
  spells_hpm.push_back(GreaterHeal(c, 1));
  spells_hpm.push_back(GreaterHeal(c, 4));

  float heal_sum = 0.0f;
  Stats s(c);
  const float max_mana = s.getMaxMana();
  float mana = max_mana;
  int64_t prev_cast = 0;
  int64_t prev_tick = start_time;
  float in_combat_sum_s = 0.0f;
  int64_t prev_log_time_ms = start_time;
  int64_t in_combat_thr_ms = 2e3;
  int64_t prev_damage_taken_ms = 0;
  while (log_ix < log_s) {
    // while log entry stamp =< time, handle log entries
    while (log_ix < log_s && time >= log[log_ix].time) {
      HandleLogEntry(log[log_ix], &deficits);
      if (log[log_ix].time - prev_log_time_ms < in_combat_thr_ms) {
        in_combat_sum_s += static_cast<float>(log[log_ix].time - prev_log_time_ms)/1e3;
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

    const float time_since_start_s = static_cast<float>(time - start_time)/1e3;
  
    // cancel oh
    if (!my_cast.done && my_cast.time > time) {
      if (deficits.find(my_cast.player) != deficits.end() && deficits[my_cast.player] < my_cast.hp_diff*(1.0f - oh_limit)) {
        my_cast.done = true;
        if (announce) std::cout << "@ time: " << time_since_start_s << ", cancelled cast " << my_cast.spell_name 
            << " r" << my_cast.spell_rank << " for " << my_cast.hp_diff << " on " << my_cast.player 
            << ", would oh: " << my_cast.hp_diff - deficits[my_cast.player] << std::endl;
      }
    }

    // if cast is finished, resolve and collect healing that was not overhealing
    if (!my_cast.done && my_cast.time <= time) {
      if (deficits.find(my_cast.player) != deficits.end()) {
        float deficit = deficits[my_cast.player];
        float healing = std::min(my_cast.hp_diff, deficit);
        heal_sum += healing;
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
      const float relative_time_left = 1.0 - static_cast<double>(time - start_time)/(end_time - start_time);

      // Low time_left_mul -> keep casting fast heals
      if (mana/max_mana > time_left_mul * relative_time_left) {
        PickBestCastIfAny(c, mana, time, deficits, spells_fast, &my_cast);
      } else {
        PickBestCastIfAny(c, mana, time, deficits, spells_hpm, &my_cast);
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

    // time goes on
    time += time_step_ms;
  }
  if (announce) std::cout << "Total time in combat: " << in_combat_sum_s << " s." << std::endl;
  return {heal_sum, in_combat_sum_s};
}

std::vector<std::vector<LogEntry>> PrunedLog(const std::vector<LogEntry>& log, const std::string& remove_player)
{
  if (log.empty()) return std::vector<std::vector<LogEntry>>{};

  int64_t max_diff_ms = 2e3;
  int64_t min_combat_len_ms = 30e3;
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


void ParseBased(const std::string& log_fn)
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
  std::string remove_player = "Paisti-Gehennas";
  auto logs = PrunedLog(log, remove_player);
  std::cout << "Pruned to: " << log.size() << " entries." << std::endl;
  

  float time_step = 0.2f;
  auto c = BaseLvl60HolyDiscHealing();
  c.sp = 700;
  c.intelligence = 400;
  c.spirit = 400;
  c.mp5 = 70;
  c.talents.meditation = 6;  // 3p T2


  // const float oh_limit = 0.75f;
  // const float time_left_mul = 2.0f;
  const float time_min_s = 30.0f;
  float best_hps = 0.0f;
  float best_oh_limit = 0.0f;
  float best_time_left_mul = 0.0f;

  for (float oh_limit = 0.1f; oh_limit <= 0.95f; oh_limit += 0.1f) {
    for (float time_left_mul = 0.1f; time_left_mul <= 3.0f; time_left_mul += 0.2f) {
      heal_sum = 0.0f;
      float time_sum = 0.0f;
      for (const auto& log : logs) {
        float log_time = (log.back().time - log.front().time) / 1e3;
        if (log_time > time_min_s) {
          auto res = SimpleLogHealing(c, log, time_step, oh_limit, time_left_mul);
          if (res.second > time_min_s) {
            time_sum += res.second;
            heal_sum += res.first;
            // std::cout << "HPS: " << res.first/res.second << std::endl;
          }
        }
      }
      float hps = heal_sum/time_sum; 
      std::cout << "oh_limit: " << oh_limit << ", time_left_mul: " << time_left_mul << " -> total hps: " << heal_sum/time_sum << std::endl;
      if (hps > best_hps) {
        best_hps = hps;
        best_oh_limit = oh_limit;
        best_time_left_mul = time_left_mul;
      }
    }
  }
  std::cout << "best hps: " << best_hps << ", oh_limit: " << best_oh_limit << ", time_left_mul: " << best_time_left_mul << std::endl;
}

}  // namespace css
