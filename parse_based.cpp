#include "parse_based.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "assumptions.h"
#include "cooldown.h"
#include "log_entry.h"
#include "priest_character.h"
#include "spells_priest_holy.h"
#include "stats.h"
#include "water.h"

namespace css
{

bool GreaterHealHasHot(const PriestCharacter& c) {
  if (!global::assumptions.transc8_exists) {
    return false;
  }

  if (c.set_bonuses.HasBonus("transcendence 8")) {
    return true;
  }
  if (c.set_bonuses.getPartial()) {
    if (c.set_bonuses.NumPieces("transcendence") > 0) {
      return true;
    }
  }
  return false;
}

void HandleLogEntry(const LogEntry& e, std::map<std::string, float>* d, //
                    std::map<std::string, float>* damage_taken = nullptr,
                    std::map<std::string, float>* healing_done = nullptr)
{
  float overheal = 0.0f;
  (*d)[e.player] += -1.0f*e.hp_diff;
  if ((*d)[e.player] < 0.0f) {
    overheal = -1.0f*(*d)[e.player];
    (*d)[e.player] = 0.0f;
  }

  if (damage_taken && e.hp_diff < 0.0f) {
    (*damage_taken)[e.player] += -1.0f*e.hp_diff;
  }

  if (healing_done && e.hp_diff > 0.0f) {
    (*healing_done)[e.source] += e.hp_diff - overheal;
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
  float cast_time;
  bool done;
  std::string spell_name;
  int spell_rank;
  std::string spell_id;
};

struct Hot {
  std::deque<int64_t> tick_times;
  float per_tick;
  std::string target;
  std::string spell_name;
};

struct Hots {
  int64_t next_tick;
  std::vector<Hot> hots;
};

namespace {



MyCast SpellToMyCast(const Spell& s, const std::string& player, int64_t time)
{
  MyCast out;
  out.player = player;
  out.hp_diff = s.healing;
  out.time = static_cast<int64_t>(s.cast_time*1e3 + time + global::assumptions.cast_delay*1e3);
  out.cost = s.cost;
  out.cast_time = s.cast_time;
  out.done = false;
  out.spell_name = s.name;
  out.spell_rank = s.rank;
  out.spell_id = out.spell_name + " " + std::to_string(out.spell_rank);
  if (out.spell_name == "Greater Heal" && s.cast_time < 2.0) {
    out.spell_id += " hazz";
  }
  return out;
}

}  // namespace


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
  
  float add_to_gh = 0.0f;
  const bool gh_has_hot = GreaterHealHasHot(c);
  if (gh_has_hot) {
    Spell renew = Renew(c, 5);
    add_to_gh = 2.5f*renew.healing;
  }

  // Pick spell with healing closest to max_val
  const Spell* best_spell = &spells.front();
  float min_d = fabs(best_spell->healing - max_val);
  for (const auto& spell : spells) {
    float d = 0.0f;
    if (gh_has_hot && spell.name == "Greater Heal") {
      d = fabs(spell.healing + add_to_gh - max_val);
    } else {
      d = fabs(spell.healing - max_val);
    }

    if (d < min_d && spell.cost < mana) {
      best_spell = &spell;
      min_d = d;
    }
  }

  if (best_spell->cost < mana) {
    *my_cast = SpellToMyCast(*best_spell, player, time);
  }
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

  float add_to_gh = 0.0f;
  const bool gh_has_hot = GreaterHealHasHot(c);
  if (gh_has_hot) {
    Spell renew = Renew(c, 5);
    add_to_gh = 2.5f*renew.healing;
  }

  my_cast->hp_diff = 0;
  for (const Spell& spell : spells) {
    if (gh_has_hot && spell.name == "Greater Heal") {
      if (deficit > spell.healing + add_to_gh && spell.healing > my_cast->hp_diff && mana > spell.cost) {
        *my_cast = SpellToMyCast(spell, player, time);
      }
    } else if (deficit > spell.healing && spell.healing > my_cast->hp_diff && mana > spell.cost) {
      *my_cast = SpellToMyCast(spell, player, time);
    }
  }
}



void AddHotsIfAny(const PriestCharacter& c, int64_t time, const MyCast& my_cast, Hots *hots) {
  if (!(my_cast.spell_name == "Greater Heal")) {
    return;
  }

  if (!GreaterHealHasHot(c)) {
    return;
  }

  if (!(my_cast.done)) {
    return;
  }

  Spell renew = Renew(c, 5);
  Hot hot;
  int n = static_cast<int>(std::round(renew.num_ticks));
  for (int i = 1; i <= n; ++i) {
    hot.tick_times.push_back(static_cast<int64_t>(i*3e3 + time));
  }
  hot.per_tick = renew.healing*c.set_bonuses.NumPieces("transcendence")/8.0f;
  hot.spell_name = "8pT2";
  hot.target = my_cast.player;
  bool found = false;

  for (auto& old_hot : hots->hots) {
    if (old_hot.target == hot.target) {
      found = true;
      old_hot = hot;
    }
  }

  if (hot.tick_times.front() < hots->next_tick || hots->hots.empty() ) {
    hots->next_tick = hot.tick_times.front();
  }

  if (!found) hots->hots.push_back(hot);
}

// This implementation might actually be more or less there now?


bool ResolveHealIfTime(int64_t time, MyCast* my_cast, std::map<std::string, float>* deficits, //
                       std::map<std::string, float>* deficits_delayed, float* mana, int64_t* prev_cast,
                       int64_t* prev_cast_start, LogResult* out) 
{
  // if cast is finished, resolve and collect healing that was not overhealing
  if (!my_cast->done && my_cast->time <= time) {
    if (deficits->find(my_cast->player) != deficits->end()) {
      float deficit = (*deficits)[my_cast->player];
      float healing = std::min(my_cast->hp_diff, deficit);
      out->heal_sum += healing;
      const std::string spell_id = my_cast->spell_id;
      out->spell_casts[spell_id]++;
      out->spell_heal_sums[spell_id] += healing;
      (*deficits)[my_cast->player] -= healing;
      (*deficits_delayed)[my_cast->player] -= healing;
      *mana -= my_cast->cost;
      my_cast->done = true;
      *prev_cast = time;
      *prev_cast_start = time - static_cast<int64_t>(my_cast->cast_time*1e3);
      return true;
    }
  }
  return false;
}

MyCast PopNextTickToCast(Hots* hots)
{
  int next_ix = 0;
  Hot* next = &hots->hots[0];
  // Find the one with next timestamp
  int ix = 0;
  for (auto& hot : hots->hots) {
    if (next->tick_times[0] > hot.tick_times[0]) {
      next = &hot;
      next_ix = ix;
    }
    ix++;
  }

  // Copy properties to my cast, no mana costs at this stage
  MyCast out;
  out.time = next->tick_times[0];
  out.spell_name = next->spell_name;
  out.spell_id = next->spell_name;
  out.player = next->target;
  out.hp_diff = next->per_tick;
  out.done = false;
  out.cost = 0.0f;

  // update next_tick
  next->tick_times.pop_front();
  if (next->tick_times.empty()) {
    hots->hots.erase(hots->hots.begin() + next_ix);
  }


  bool set = false;
  for (auto& hot : hots->hots) {
    if (!set) {
      hots->next_tick = hot.tick_times[0];
      set = true;
    }
    if (hots->next_tick > hot.tick_times[0]) {
      hots->next_tick = hot.tick_times[0];
    }
  }
  return out;
}

void ResolveHotsIfTime(int64_t time, Hots* hots, std::map<std::string, float>* deficits, //
                       std::map<std::string, float>* deficits_delayed,
                       LogResult* out) 
{
  int64_t prev_cast_dummy = 0;
  int64_t prev_cast_start_dummy = 0;
  float mana_dummy = 0.0f;
  float heal_sum_was = out->heal_sum;
  while (!hots->hots.empty() && hots->next_tick <= time) {
    auto hot_as_cast = PopNextTickToCast(hots);
    ResolveHealIfTime(time, &hot_as_cast, deficits, deficits_delayed, &mana_dummy, &prev_cast_dummy, &prev_cast_start_dummy, out);
  }
  if (out->heal_sum > heal_sum_was) {
    // std::cout << "time: " << time << ", heal sum increase: " << out->heal_sum - heal_sum_was << std::endl;
    // std::cout << "    8pT2 heal_sum: " << out->spell_heal_sums["8pT2"] << std::endl;
  }
}


// If it was 8p t2, add hot here to hots, this same func is used for resolving
// TODO: Hots -> MyCast, call ResolveHealIfTime, repeat while done
void ResolveHealsIfTime(const PriestCharacter& c, int64_t time, MyCast* my_cast, std::map<std::string, float>* deficits, //
                       std::map<std::string, float>* deficits_delayed, float* mana, int64_t* prev_cast, int64_t* prev_cast_start,
                       LogResult* out, Hots *hots) 
{
  if (ResolveHealIfTime(time, my_cast, deficits, deficits_delayed, mana, prev_cast, prev_cast_start, out)) { 
    AddHotsIfAny(c, time, *my_cast, hots);
  }
  ResolveHotsIfTime(time, hots, deficits, deficits_delayed, out);
}



LogResult SimpleLogHealing(const PriestCharacter& c, const std::vector<LogEntry>& log, float time_step_s,
                                         float oh_limit, float time_left_mul, float *mana, Cooldowns* cds)
{
  LogResult out;
  if (log.empty()) return out;

  std::map<std::string, float> deficits;
  std::map<std::string, float> deficits_delayed;
  std::map<std::string, float> damage_taken;
  const int64_t time_step_ms = std::max<int64_t>(1, static_cast<int64_t>(time_step_s * 1e3f));

  // take first log entry -> combat start
  bool announce = false;
  int64_t time = log[0].time;
  const int64_t start_time = time;
  const int64_t end_time = log.back().time;
  int64_t log_ix = 0;
  int64_t log_ix_delayed = 0;
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
  if (GreaterHealHasHot(c)) {
    for (int i = 1; i <= 4; ++i) spells_hpm.push_back(GreaterHeal(c, 1));
  } else {
    spells_hpm.push_back(Heal(c, 2));
    spells_hpm.push_back(Heal(c, 4));
    spells_hpm.push_back(GreaterHeal(c, 1));
    spells_hpm.push_back(GreaterHeal(c, 4));
  }

  std::vector<Spell> spells_hazz;
  bool have_hazz = cds->find("hazza'rah's") != cds->end();
  if (have_hazz) {
    auto& cd = cds->at("hazza'rah's");
    cd.active = true;
    for (int i = 1; i <= 4; ++i) {
      Spell spell = GreaterHeal(c, i);
      ApplyCooldownEffects(*cds, &spell);
      spells_hazz.push_back(spell);
    }
    cd.active = false;
  }

  std::vector<Spell>* spell_list = &spells_fast;

  for (const auto& spell : spells_fast) {
    std::string spell_id = spell.name + " " + std::to_string(spell.rank);
    out.spell_id_to_spell[spell_id] = spell;
  }

  for (const auto& spell : spells_hpm) {
    std::string spell_id = spell.name + " " + std::to_string(spell.rank);
    out.spell_id_to_spell[spell_id] = spell;
  }

  for (const auto& spell : spells_hazz) {
    std::string spell_id = spell.name + " " + std::to_string(spell.rank) + " hazz";
    out.spell_id_to_spell[spell_id] = spell;
  }


  Hots hots;
  Stats s(c);
  const float max_mana = s.getMaxMana();
  int64_t prev_cast = 0;
  int64_t prev_cast_start = 0;
  int64_t prev_tick = start_time;
  int64_t in_combat_thr_ms = static_cast<int64_t>(global::assumptions.max_log_entry_diff_in_combat*1e3);
  int64_t prev_damage_taken_ms = 0;

  const bool precast = global::assumptions.precast;
  const bool swap_cast = global::assumptions.swap_cast;

  const int64_t reaction_time_ms = static_cast<int64_t>(global::assumptions.reaction_time*1e3);

  int64_t in_combat_sum_ms = 0;
  while (log_ix < log_s) {
    // while log entry stamp =< time, handle log entries
    bool in_combat = false;
    while (log_ix < log_s && time >= log[log_ix].time) {
      if (log[log_ix].time - prev_damage_taken_ms < in_combat_thr_ms) {
        in_combat = true;
      }

      if (log[log_ix].hp_diff < 0) {
        prev_damage_taken_ms = log[log_ix].time;
        in_combat = true;
      }

      HandleLogEntry(log[log_ix], &deficits, nullptr, &out.player_heal_sums);
      if (in_combat) {
        ResolveHealsIfTime(c, log[log_ix].time, &my_cast, &deficits, &deficits_delayed, mana, &prev_cast, &prev_cast_start, &out, &hots);
      } else {
        my_cast.done = true;
      }

      log_ix++;
    }
    while (log_ix_delayed < log_s && time - reaction_time_ms >= log[log_ix_delayed].time) {
      HandleLogEntry(log[log_ix_delayed], &deficits_delayed, &damage_taken);
      log_ix_delayed++;
    }

    if (time - prev_damage_taken_ms < in_combat_thr_ms) {
      in_combat_sum_ms += time_step_ms;
      in_combat = true;
    }

    if (in_combat) {
      const float relative_time_left = 1.0f - static_cast<float>(time - start_time)/(end_time - start_time);
      const float time_since_start_s = static_cast<float>(time - start_time)/1e3f;

      // picke spell list to use:
      spell_list = &spells_hpm;
      if ((*mana)/max_mana > time_left_mul * relative_time_left) {
        spell_list = &spells_fast;
      }

      if (have_hazz) {
        bool active = false;
        auto& cd = cds->at("hazza'rah's");
        if (cd.active && time < cd.effect_end_ms) active = true;
        if (cd.active && time > cd.effect_end_ms) cd.active = false;
        
        if (!cd.active && time > cd.off_cooldown_ms) {
          double total_deficit = 0.0;
          for (auto& entry : deficits_delayed) {
            total_deficit += entry.second;
          }
          if (total_deficit > global::assumptions.total_deficit_to_pop_trinkets) {
            cd.active = true;
            cd.off_cooldown_ms = time + static_cast<int64_t>(180*1e3); // 3 min cd this should be in cooldowns or sth TODO REFACTOR
            cd.effect_end_ms = time + static_cast<int64_t>(15*1e3); // 15 s effect
            active = true;
          }
        }
        if (active) {
          spell_list = &spells_hazz;
        }
      }

      // cancel oh
      if (!my_cast.done && my_cast.time > time) {
        if (deficits_delayed.find(my_cast.player) != deficits_delayed.end() && deficits_delayed[my_cast.player] < my_cast.hp_diff*(1.0f - oh_limit)) {
          // TODO if last tick or better heal available

          if (swap_cast) {
            MyCast better_cast;
            PickBestCastIfAny(c, *mana, time, deficits_delayed, *spell_list, &better_cast);

            bool better_target_found = deficits_delayed[better_cast.player] > deficits_delayed[my_cast.player]*2.0f;

            if (better_target_found) {
              my_cast = better_cast;
            }
          }

          bool last_tick = time + time_step_ms > my_cast.time;
          if (last_tick) {
            my_cast.done = true;
            if (announce) std::cout << "@ time: " << time_since_start_s << ", cancelled cast " << my_cast.spell_name 
              << " r" << my_cast.spell_rank << " for " << my_cast.hp_diff << " on " << my_cast.player 
                  << ", would oh: " << my_cast.hp_diff - deficits_delayed[my_cast.player] << std::endl;
          }
        }
      }

      // if cast is finished, resolve and collect healing that was not overhealing
      ResolveHealsIfTime(c, time, &my_cast, &deficits, &deficits_delayed, mana, &prev_cast, &prev_cast_start, &out, &hots); 

      // if not casting, pick best target and start casting
      float gcd = 1.5e3;
      if (my_cast.cast_time < 1.5 && my_cast.spell_name == "Flash Heal") {
        gcd = my_cast.cast_time*1e3;
      }
      if (my_cast.done && time > prev_cast_start + gcd) {
        PickBestCastIfAny(c, *mana, time, deficits_delayed, *spell_list, &my_cast);
        // If could not find target, pick precast target
        if (my_cast.done && precast) {
          PickBestPreCast(c, *mana, time, deficits_delayed, damage_taken, *spell_list, &my_cast);
        }
      }
    } else { // not in combat
      my_cast.done = true;
    }

    // mana regen
    if (time >= prev_tick + 2e3 ) {
      if (time >= prev_cast + 5e3) {
        *mana += s.getManaRegenTickOutOfFsr();
      } else {
        *mana += s.getManaRegenTickUnderFsr();
      }
      prev_tick = time;
    }

    if (!in_combat) {
      auto water = BestWater(c);
      *mana += water.per_tick/3.0f*time_step_s;
    } else {
      for (const auto& deficit : deficits) {
        if (deficit.second > 0) {
          out.deficit_time_sum += (deficit.second*deficit.second)*time_step_s;
        }
      }
    }
    *mana = std::min(max_mana, *mana);

    // Decay for damage taken so recent damage is more heavily weighted
    if (precast) {
      DamageTakenDecay(time_step_s, &damage_taken);
    }

    // time goes on
    time += time_step_ms;
  }
  out.in_combat_sum = static_cast<float>(in_combat_sum_ms)/1e3f;
  if (announce) std::cout << "Total time in combat: " << out.in_combat_sum << " s." << std::endl;
  return out;
}

LogsType PrunedLog(const std::vector<LogEntry>& log, const std::string& remove_player, int64_t* skipped_player_entries)
{

  if (skipped_player_entries != nullptr) {
    *skipped_player_entries = 0;
  }
  if (log.empty()) return std::vector<std::vector<LogEntry>>{};

  int64_t max_diff_ms = static_cast<int64_t>(global::assumptions.max_log_entry_diff_in_combat * 1e3);
  int64_t min_combat_len_ms = static_cast<int64_t>(global::assumptions.min_combat_length*1e3);
  std::vector<std::vector<LogEntry>> out;
  int64_t prev_t_ms = log[0].time;
  std::vector<LogEntry> this_combat;
  int64_t start_time = prev_t_ms;
  for (const auto& e : log) {
    if (e.time - prev_t_ms > max_diff_ms || &e == &log.back()) {
      if (!this_combat.empty() && this_combat.back().time - this_combat.front().time > min_combat_len_ms) {
        out.push_back(std::vector<LogEntry>{});
        std::cout << "Combat from " << (this_combat.front().time - start_time)/1e3 << " to " << (this_combat.back().time - start_time)/1e3 << " s" << std::endl;
        for (auto& e : this_combat) {
          out.back().push_back(e);
        }
      }
      this_combat.clear();
    }
    if (e.source == remove_player && skipped_player_entries != nullptr) {
      (*skipped_player_entries)++;
    }
    if (e.player.find(" ") == std::string::npos && e.source != remove_player) {
      this_combat.push_back(e);
    }
    if (e.hp_diff < 0) {
      prev_t_ms = e.time;
    }
  }
  return out;
}

std::vector<LogEntry> ReadBlizzardLog(const std::string& log_fn)
{
  std::cout << "Reading Blizzard log: " << log_fn << std::endl;
  std::ifstream is(log_fn.c_str());
  std::string line;
  float diff_sum = 0.0f;
  float heal_sum = 0.0f;
  float damage_sum = 0.0f;
  int64_t ms_start = 0;
  int64_t ms_end = 0;
  
  std::vector<LogEntry> log;
  int64_t lines = 0;
  int64_t time_prev_ms = 0;
  constexpr int64_t hour_ms = static_cast<int64_t>(60*60*1e3);
  while (std::getline(is, line)) {
    LogEntry e;
    if (LineToLogEntryIfAny(line, &e)) {
      // std::cout << "ignored line: " << line << std::endl;
      diff_sum += e.hp_diff;
      if (ms_start == 0) {
        ms_start = e.time;
        time_prev_ms = ms_start;
      }
      if (e.hp_diff < 0) {
        damage_sum -= e.hp_diff;
      } else if (e.source != "DEATH") {
        heal_sum += e.hp_diff;
      }
      ms_end = e.time;
      if (e.time - time_prev_ms > hour_ms) {
        std::cout << "Pretty strange time increment of : " << static_cast<float>(e.time - time_prev_ms)/hour_ms 
            << " hours. Let's say we skip this line:" << std::endl;
        std::cout << line << std::endl;
        time_prev_ms = e.time;
      } else {
        log.push_back(e);
        time_prev_ms = e.time;
      }
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
  return log;
}

std::vector<LogEntry> ReadWclParsedLog(const std::string& log_fn)
{
  std::cout << "Reading WCL parsed log: " << log_fn << std::endl;
  std::ifstream is(log_fn.c_str());
  std::string line;
  std::vector<LogEntry> log;
  int64_t lines = 0;
  int64_t valid = 0;
  while (std::getline(is, line)) {
    LogEntry e;
    if (WclParsedLineToLogEntryIfAny(line, &e)) {
      log.push_back(e);
      valid++;
    }
    lines++;
  }
  is.close();
  std::cout << "Saw " << lines << " lines of log, with " << valid << " log lines" << std::endl;
  std::cout << "Sorting by timestamp..." << std::endl;

  auto sortRuleLambda = [] (LogEntry const& e1, LogEntry const& e2) -> bool
  {
    return e1.time < e2.time;
  };

  std::sort(log.begin(), log.end(), sortRuleLambda);
  std::cout << "Sorted." << std::endl;
  return log;
}


LogsType GetLogs(const std::string& log_fn, int64_t* skipped_player_entries)
{
  std::vector<LogEntry> log;
  if (global::assumptions.wcl_log) {
    log = ReadWclParsedLog(log_fn);
  } else {
    log = ReadBlizzardLog(log_fn);
  }
  std::cout << "Pruning log from: " << log.size() << " entries." << std::endl;
  std::string remove_player = global::assumptions.skip_player;
  auto logs = PrunedLog(log, remove_player, skipped_player_entries);
  int total_size = 0;
  for (auto one_log : logs) {
      total_size += static_cast<int>(one_log.size());
  }
  std::cout << "Pruned to: " << total_size << " entries." << std::endl;
  return logs;
}

LogResult HpsForLogs(const PriestCharacter& c, float oh_limit, float time_left_mul, const LogsType& logs)
{
  LogResult out;

  const float time_step = global::assumptions.time_step;
  const float time_min_s = global::assumptions.min_combat_length;
  Stats stats(c);
  const float max_mana = stats.getMaxMana();
  float mana = max_mana;
  if (logs.empty() || logs[0].empty()) return out;

  int64_t time = logs.front().front().time;

  auto cds = SetsToCooldowns(c.set_bonuses);
  for (const auto& log : logs) {
    if (log.empty()) continue;

    float log_time = (log.back().time - log.front().time)/1e3f;
    if (log_time > time_min_s) {

      // drink before next combat
      float before_log_time_s = (log.front().time - time)/1e3f;
      mana += stats.getManaRegenPerSecondDrinking()*before_log_time_s;
      mana = std::min(mana, max_mana);
      float mana_at_start = mana;
      auto res = SimpleLogHealing(c, log, time_step, oh_limit, time_left_mul, &mana, &cds);
      // auto res = SimpleLogHealing(c, log, time_step, oh_limit, time_left_mul, &mana);
      time = log.back().time;
      if (res.in_combat_sum > time_min_s) {
        out.mana_at_start_sum += mana_at_start;
        out.mana_at_end_sum += mana;
        out.n_combats++;
        out.in_combat_sum += res.in_combat_sum;
        out.heal_sum += res.heal_sum;
        out.deficit_time_sum += res.deficit_time_sum;
        for (const auto& entry : res.spell_casts) {
          out.spell_casts[entry.first] += entry.second;
        }
        for (const auto& entry : res.spell_heal_sums) {
          out.spell_heal_sums[entry.first] += entry.second;
        }
        for (const auto& entry : res.player_heal_sums) {
          out.player_heal_sums[entry.first] += entry.second;
        }
        if (out.spell_id_to_spell.empty()) {
          out.spell_id_to_spell = res.spell_id_to_spell;
        }
        // std::cout << "HPS: " << res.first/res.second << std::endl;
      }
    }
  }
  const float sum = out.spell_heal_sums["8pT2"];
  if (sum) {
    // std::cout << "8pT2 sum: " << out.spell_heal_sums["8pT2"] << ", total sum: " << out.heal_sum << std::endl;
  }
  // std::cout << "oh_limit: " << oh_limit << ", time_left_mul: " << time_left_mul << " -> total hps: " << heal_sum/time_sum << std::endl;
  return out;
}

std::pair<float, float> FindBestOhLimitAndTimeLeftMul(const PriestCharacter& c, const LogsType& logs)
{
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

  PriestCharacter c_tmp = c;
  if (!c_tmp.set_bonuses.HasBonus("hazza'rah's 1")) {
      Item item;
      item.name = "hazz'rah's charm of healing";
      c_tmp.set_bonuses.AddItem(item);
  }

  auto res = HpsForLogs(c_tmp, best_oh_limit, best_time_left_mul, logs);
  float hps = res.heal_sum/res.in_combat_sum;
  best_hps = hps;
  float best_deficit_thr = global::assumptions.total_deficit_to_pop_trinkets;
  float deficit_thr = best_deficit_thr;
  constexpr int max_change = 10;  // Avoid being stuck in loop forever

  float &thr = global::assumptions.total_deficit_to_pop_trinkets;
  for (int i = 0; i < max_change; ++i) {
      thr *= 1.05f;
      res = HpsForLogs(c_tmp, best_oh_limit, best_time_left_mul, logs);
      hps = res.heal_sum/res.in_combat_sum;

      if (hps > best_hps) {
          best_deficit_thr = thr;
          best_hps = hps;
      }
  }

  thr = deficit_thr;
  for (int i = 0; i < max_change; ++i) {
      thr *= 0.95f;
      res = HpsForLogs(c_tmp, best_oh_limit, best_time_left_mul, logs);
      hps = res.heal_sum/res.in_combat_sum;

      if (hps > best_hps) {
          best_deficit_thr = thr;
          best_hps = hps;
      }
  }

  thr = best_deficit_thr;
  // std::cout << "best hps: " << best_hps << ", oh_limit: " << best_oh_limit << ", time_left_mul: " << best_time_left_mul //
      // << ", total_deficit_to_pop_trinkets: " << thr << std::endl;
  return {best_oh_limit, best_time_left_mul};
}

void ParseBased(const std::string& log_fn)
{
  global::assumptions.log_in = log_fn;
  auto logs = GetLogs(log_fn);

  auto c = BaseLvl60HolyDiscHealing();
  c.sp = 700;
  c.intelligence = 400;
  c.spirit = 400;
  c.mp5 = 70;
  c.talents.meditation = 6;  // 3p T2
  FindBestOhLimitAndTimeLeftMul(c, logs);
}

}  // namespace css
