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
};


// TODO: should keep track of the biggest deficit target i guess or sth
void PickBestCastIfAny(const PriestCharacter& c, float mana, int64_t time, const std::map<std::string, float>& deficits, //
                       const Spell& spell, MyCast* my_cast)
{
  if (deficits.empty()) return;
  if (mana < spell.cost) return;
  // just pick the target with biggest deficit
  std::string player = deficits.cbegin()->first;
  float deficit = deficits.cbegin()->second;
  for (const auto &d : deficits) {
    if (d.second > deficit && d.second > spell.healing) {
      player = d.first;
      deficit = d.second;
    }
  }
  if (deficit > spell.healing) {
    my_cast->player = player;
    my_cast->hp_diff = spell.healing;
    my_cast->time = spell.cast_time*1e3 + time;
    my_cast->cost = spell.cost;
    my_cast->done = false;
  }
}

float SimpleLogHealing(const PriestCharacter& c, const std::vector<LogEntry>& log, float time_step)
{
  if (log.empty()) return 0.0f;

  std::map<std::string, float> deficits;
  const int64_t time_step_ms = std::max<int64_t>(1, time_step * 1e3);

  // take first log entry -> combat start
  bool done = false;
  int64_t time = log[0].time;
  const int64_t start_time = time;
  int64_t log_ix = 0;
  const int64_t log_s = static_cast<int64_t>(log.size());

  MyCast my_cast;
  my_cast.time = 0;
  my_cast.hp_diff = 0.0f;
  my_cast.player = "Paisti";
  my_cast.cost = 0.0f;
  my_cast.done = true;

  const auto best_spell_eu = FlashHeal(c, 7); 

  float heal_sum = 0.0f;
  Stats s(c);
  float mana = s.getMaxMana();
  while (log_ix < log_s) {
    // while log entry stamp =< time, handle log entries
    while (log_ix < log_s && time >= log[log_ix].time) {
      HandleLogEntry(log[log_ix], &deficits);
      log_ix++;
    }
  
    // if cast is finished on next step, cancel if overhealing
    if (!my_cast.done && my_cast.time <= time + time_step_ms) {
      if (deficits.find(my_cast.player) != deficits.end() && deficits[my_cast.player] < my_cast.hp_diff) {
        my_cast.done = true;
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
      }
    }

    // if not casting, pick best target and start casting
    if (my_cast.done) {
      PickBestCastIfAny(c, mana, time, deficits, best_spell_eu, &my_cast);
    }

    // mana regen

    // time goes on
    time += time_step_ms;
  }
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
  }
  float time_s = (ms_end - ms_start)*1e-3f;
  is.close();
  std::cout << "diff_sum: " << diff_sum << std::endl;
  std::cout << "/40: " << diff_sum/40.0f << std::endl;
  std::cout << "heal_sum: " << heal_sum << " -> hps: " << heal_sum/time_s << std::endl;
  std::cout << "damage_sum: " << damage_sum << " -> dtps: " << damage_sum/time_s << std::endl;
  std::cout << "time_s: " << time_s << std::endl;

  float time_step = 0.2f;
  auto c = BaseLvl60HolyDiscHealing();
  c.sp = 700;
  c.intelligence = 400;
  c.spirit = 400;
  c.mp5 = 70;
  c.talents.meditation = 6;  // 3p T2
  float hps = SimpleLogHealing(c, log, time_step);
  std::cout << "SimpleLogHealing, hps: " << hps << std::endl;
}

}  // namespace css
