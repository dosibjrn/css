#include "combat.h"

#include <iostream>

#include "fight_result.h"
#include "spell_sequence_priest.h"
#include "stats.h"
#include "water.h"

namespace css
{

namespace globals
{

namespace
{
  float g_timeToPull = 1.0f;
}  // namespace

float getTimeToPull()
{
  return g_timeToPull;
}

void setTimeToPull(float f)
{
  g_timeToPull = f;
}

}  // namespace globals

FightResult FightMob(const PriestCharacter& c, const std::vector<Spell>& spells, const Mob& mob)
{
  bool verbose = false;
  FightResult fr;
  float mob_health = mob.health;
  int weave = 0;
  float time = 0.0f;
  int spell_ix = 0;

  float mana_diff = 0.0f;
  float last_rege_tick = 0.0f;
  Stats stats(c);

  // Supporting 1 dot, basically swp now. 
  int dot_tick_ix = 0;
  Spell dot;
  float dot_start_time = 0.0f;
  float no_casting = 0.0f;
  float cooldown_start = -10.0f;
  float weave_drops = 0.0f;
  while (mob_health > 0) {
    // Check weave drop
    if (time > weave_drops) {
      weave = 0;
    }
    bool spell_cast = false;
    if (spell_ix < spells.size()) {
      Spell s = spells[spell_ix];
      if (!s.cd || s.cd + cooldown_start < time) {
        if (s.cd) {
          cooldown_start = time;
        }
        spell_cast = true;
        no_casting = 0.0f;
        mana_diff -= s.cost;
        fr.spell_total_cost += s.cost;
        spell_ix++;
        time += s.cast_time;
        float damage = s.damage;
        if (s.can_crit) {
          damage *= stats.spellCritMul();
        }
        if (damage > 0 && s.type == School::Shadow) {
          // add weave to damage if not dot - dot's handled later
          if (!s.dot) {
            damage = damage + damage*(weave*0.03f);
          }
          // increment weave
          weave = std::min(5, weave + 1);
          weave_drops = time+15.0f;
        }
        if (s.channeled) {
          damage *= s.num_ticks;
        }
        if (!s.dot) {
          mob_health -= damage;
          if (verbose) {
            if (s.channeled) {
              std::cout << "channel, ";
            }
            std::cout << "mob_health: " << mob_health << ", time: " << time << std::endl;
          }
          fr.damage_spells += damage;
        } else {
          dot = s;
          dot_start_time = time - s.cast_time;
          if (verbose) std::cout << "dot start: " << dot_start_time << ", time: " << time << std::endl;
        }
      } else if (verbose) {
        std::cout << "skipped spell on cd, s.cd: " << s.cd << ", cooldown_start: " << cooldown_start
            << ", s.cd + cooldown_start: " << s.cd + cooldown_start << " < time: " << time << std::endl;
      }
    }
    if (!spell_cast) { // wand
      float damage = c.wand.dps*c.wand.speed*(1.0f + c.talents.wand_specialization*0.05f);
      if (c.wand.type == School::Shadow) {
        damage = damage + damage*(weave*0.03f);
      }
      time += c.wand.speed;
      mob_health -= damage;

      if(verbose) std::cout << "wand, mob_health: " << mob_health << ", time: " << time << std::endl;
      fr.damage_wand += damage;
      fr.time_wand += c.wand.speed;
      no_casting += c.wand.speed;
    }
    // Check dots for damage
    if (dot_tick_ix < dot.num_ticks && time >= dot_start_time + (dot_tick_ix + 1)*3.0f) {
      dot_tick_ix++;
      float damage = dot.damage + dot.damage*(weave*0.03f);
      mob_health -= damage;

      if(verbose) std::cout << "dot, mob_health: " << mob_health << ", time: " << time << std::endl;
      fr.damage_spells += damage;
    }
    // Check mana rege
    if (time > last_rege_tick + 2.0f) {
      last_rege_tick = time;
      if (no_casting > 5.0f) {
        mana_diff += stats.getManaRegenTickOutOfFsr();
      } else {
        mana_diff += stats.getManaRegenTickUnderFsr();
      }
    }
  }
  fr.duration = time;
  fr.mana_drop = -1.0f*mana_diff;
  fr.time_to_out_of_fsr = std::max(0.0f, 5.0f - no_casting);
  fr.dps = (fr.damage_spells + fr.damage_wand)/fr.duration;
  // std::cout << "Mob with health: " << mob.health << " dead in " << time << " s, mana_drop: " << fr.mana_drop << ", ttooFSR: " << fr.time_to_out_of_fsr << std::endl;

  return fr;
}

// Lvl 45 water for now
Water BestWater(const PriestCharacter &c)
{
  // 44 -> 39 -> 3 -> 30 -> 35. 45 -> 40 -> 4 -> 40 -> 45. 5 -> ...
  int water_level = ((c.level - 5)/10)*10 + 5;
  Water w;
  float water_max = 0.0f;
  float dura = 0.0f;
  switch (water_level) {
    case 5:
      water_max = 436.8f;
      dura = 21;
    case 15 :
      water_max = 835.2f;
      dura = 24;
      break;
    case 25 :
      water_max = 1344.6f;
      dura = 27.0f;
      break;
    case 35 :
      water_max = 1992.0f;
      dura = 30.0f;
      break;
    case 45 :
      water_max = 2934.0f;
      dura = 30.0f;
      break;
    case 55 :
      water_max = 4200.0f;
      dura = 30.0f;
      break;
    default :  // lvl 1
      water_max = 151.2f;
      dura = 18.0f;
      break;
  }
  w.num_ticks = dura/3.0f;
  w.per_tick = water_max/w.num_ticks;
  return w;
}


// TODO: should we consider no spirit tap? not really?
float DrinkToFull(const Stats& stats, const Water& water, const FightResult& fr, float current_mana)
{
  float max_mana = stats.getMaxMana();
  int normal_ticks = fr.time_to_out_of_fsr/2.0f;
  float time = 0.0f;
  int spirit_tap_max_ticks = 7;
  float last_water_tick = 0.0f;
  float last_regen_tick = 0.0f;
  float min_time_drinking = water.num_ticks*3.0f/2.0f;

  int i = 0;
  while (current_mana < max_mana || time < min_time_drinking) {
    time += 1.0f;
    if (time >= last_water_tick + 3.0f) {
      current_mana += water.per_tick;
      last_water_tick = time;
    }
    if (time >= last_regen_tick + 2.0f) {
      last_regen_tick = time;
      if (i < spirit_tap_max_ticks) {
        if (i < normal_ticks) {
          current_mana += stats.getManaRegenTickSpiritTapUnderFsr();
        } else {
          current_mana += stats.getManaRegenTickSpiritTapOutOfFsr();
        }
      } else {
        current_mana += stats.getManaRegenTickOutOfFsr();  // Note: will get out of FSR before spirit tap ends...
      }
      i++;
    }
  }
  return time + globals::getTimeToPull();
}

float RegenForNext(const Stats& stats, const FightResult& fr, int ticks_for_next, float *current_mana)
{
  int normal_ticks = fr.time_to_out_of_fsr/2.0f;
  float time = 0.0f;
  int i = 0;
  int spirit_tap_max_ticks = 7;
  for (int tick = 0; tick < ticks_for_next; ++tick) {
    // if remaining spirit tap ticks take us to target, add them and return
    if (i < spirit_tap_max_ticks) {
      if (i < normal_ticks) {
        *current_mana += stats.getManaRegenTickSpiritTapUnderFsr();
      } else {
        *current_mana += stats.getManaRegenTickSpiritTapOutOfFsr();
      }
    } else {
      *current_mana += stats.getManaRegenTickOutOfFsr();
    }
    time += 2.0f;
    i++;
  }
  if (ticks_for_next < spirit_tap_max_ticks) {
    float extra_from_spirit_tap = (spirit_tap_max_ticks - ticks_for_next)*(stats.getManaRegenTickSpiritTapUnderFsr()
                                                                           - stats.getManaRegenTickUnderFsr());
    *current_mana += extra_from_spirit_tap;
  }
  *current_mana = std::min(stats.getMaxMana(), *current_mana);
  return time;
}


FightResult FightMobs(const PriestCharacter& c, const std::vector<Spell>& spells, const Mob& mob, int ticks_for_next)
{
  FightResult fr;
  Stats stats(c);
  auto water = BestWater(c);  // best for level
  float regen_after_kill;
  float current_mana = stats.getMaxMana();
  float time_elapsed = 0.0f;
  float regen_for_next_sum = 0.0f;
  int n_max = 90;
  int n_killed = 0;
  int regen_for_next_count = 0;
  while (n_killed < n_max) {
    fr = FightMob(c, spells, mob);
    fr.mana_drop += 50;  // Inner fire etc
    n_killed++;
    current_mana -= fr.mana_drop;
    time_elapsed += fr.duration;
    if (current_mana < fr.mana_drop || n_killed == n_max) {
      fr.drink_to_full_time = DrinkToFull(stats, water, fr, current_mana);
      time_elapsed += fr.drink_to_full_time;
      break;
    } else {
      float regen_for_next_time = RegenForNext(stats, fr, ticks_for_next, &current_mana);
      time_elapsed += regen_for_next_time;
      regen_for_next_sum += regen_for_next_time;
      regen_for_next_count++;
    }
  }
  if (regen_for_next_count) {
    fr.regen_for_next_time = regen_for_next_sum/regen_for_next_count;
  }
  fr.mobs_per_min = n_killed/(time_elapsed/60.0f);
  fr.mobs_per_drink = n_killed;
  return fr;
}
FightResult BestMobsPerMin(const PriestCharacter&c, const Mob& mob) {
  FightResult bfr;
  std::vector<int> ticks_for_next_opts;
  for (int i = globals::getTimeToPull()/2; i < 10; ++i) {
    ticks_for_next_opts.push_back(i);
  }

  // for (int ticks_for_next = 8; ticks_for_next < 30; ++ticks_for_next) {
  for (int ticks_for_next : ticks_for_next_opts) {
    for (int shields = 0; shields < 1; ++shields) {
      std::vector<int> flaytick_options{0, 3, 5, 7, 9, 11, 13};
      if (shields == 1) {
        flaytick_options = {0, 3, 6, 9, 12, 15};
      }
      for (auto &ticks : flaytick_options) {
        // flayticks 3 or 5
        for (int blasts = 0; blasts <= 3; ++blasts) {
          for (int touches = 0; touches < 1; ++touches) {
            // float blast_incr = 1.0f/blasts;
            // if (blasts == 0) {
              // blast_incr = 10.0f;
            // }
            // for (float blast_down_rank = 0; blast_down_rank < 7.0f; blast_down_rank += blast_incr) {
              // for (float flay_down_rank = 0; flay_down_rank < 4.1f; flay_down_rank += 1.0f/(ticks)) {
              // for (int flay_down_rank = 0; flay_down_rank < 5; ++flay_down_rank) {
              // float flay_incr = 1.0f/ticks;
              // if (ticks == 0) {
                // flay_incr = 10.0f;
              // }
              // for (float flay_down_rank = 0; flay_down_rank < 4.1f; flay_down_rank += flay_incr) {
            for (float blast_down_rank = 0; blast_down_rank < 0.01f; blast_down_rank += 0.5f) {
              for (float flay_down_rank = 0; flay_down_rank < 0.01f; flay_down_rank += 1.0f/3.0f) {
                auto spells = SpellsSequencePaistiLvl52(c, ticks, blasts, shields, touches, flay_down_rank, blast_down_rank);
                FightResult fr = FightMobs(c, spells, mob, ticks_for_next);
                if (fr.mobs_per_min > bfr.mobs_per_min) {
                  bfr = fr;
                  bfr.flay_ticks = ticks;
                  bfr.blasts = blasts;
                  bfr.shields = shields;
                  bfr.touches = touches;
                  bfr.flay_down_rank = flay_down_rank;
                  bfr.blast_down_rank = blast_down_rank;
                }
              }
            }
          }
        }
      }
    }
  }
  std::cout << ".";
  std::cout.flush();
  coutFightResult(bfr);
  return bfr;
}

// int, spirit, sp, wand dps, mp5
std::vector<FightResult> RelativeValues(const PriestCharacter& c, float multiplier, const Mob& mob)
{
  // std::cout << "Base: " << std::endl;
  auto base_fr = BestMobsPerMin(c, mob);
  float base_mobs_per_min = base_fr.mobs_per_min;

  // int
  auto c_int = c;
  c_int.intelligence *= (2.0*multiplier);
  // std::cout << "Int: " << c.intelligence << " vs. " << c_int.intelligence << std::endl;
  auto int_fr = BestMobsPerMin(c_int, mob);
  float int_mobs_per_min = int_fr.mobs_per_min;
  float int_diff = (int_mobs_per_min - base_mobs_per_min)
      /(c_int.intelligence - c.intelligence)*60.0f;
  int_fr.diff = int_diff;

  // spirit
  auto c_spirit = c;
  c_spirit.spirit *= multiplier;
  // std::cout << "Spirit: " << c.spirit << " vs. " << c_spirit.spirit << std::endl;
  auto spirit_fr = BestMobsPerMin(c_spirit, mob);
  float spirit_mobs_per_min = spirit_fr.mobs_per_min;
  float spirit_diff = (spirit_mobs_per_min - base_mobs_per_min)
      /(c_spirit.spirit - c.spirit)*60.0f;
  spirit_fr.diff = spirit_diff;

  // sp
  auto c_sp = c;
  c_sp.sp *= multiplier;
  c_sp.sp = std::max(c.sp+20, c_sp.sp);

  // std::cout << "Sp: " << c.sp << " vs. " << c_sp.sp << std::endl;
  auto sp_fr = BestMobsPerMin(c_sp, mob);
  float sp_mobs_per_min = sp_fr.mobs_per_min;
  float sp_diff = (sp_mobs_per_min - base_mobs_per_min)
      /(c_sp.sp - c.sp)*60.0f;
  sp_fr.diff = sp_diff;

  // wand
  auto c_wand = c;
  c_wand.wand.dps *= multiplier;
  // std::cout << "Wand dps: " << c.wand.dps << " vs. " << c_wand.wand.dps << std::endl;
  auto wand_fr = BestMobsPerMin(c_wand, mob);
  float wand_mobs_per_min = wand_fr.mobs_per_min;
  float wand_diff = (wand_mobs_per_min - base_mobs_per_min)
      /(c_wand.wand.dps - c.wand.dps)*60.0f;
  wand_fr.diff = wand_diff;

  // mp5
  auto c_mp5 = c;
  c_mp5.mp5 = std::max<int>(c.mp5 + 10, c.mp5*multiplier);  //
  // std::cout << "Mp5: " << c.mp5 << " vs. " << c_mp5.mp5 << std::endl;
  auto mp5_fr = BestMobsPerMin(c_mp5, mob);
  float mp5_mobs_per_min = mp5_fr.mobs_per_min;
  float mp5_diff = (mp5_mobs_per_min - base_mobs_per_min)
      /(c_mp5.mp5 - c.mp5)*60.0f;
  mp5_fr.diff = mp5_diff;

  // float ref_diff = spirit_diff;
  // float rel_int = int_diff/ref_diff;
  // float rel_spirit = spirit_diff/ref_diff;
  // float rel_sp = sp_diff/ref_diff;
  // float rel_wand = wand_diff/ref_diff;
  // float rel_mp5 = mp5_diff/ref_diff;
  // std::cout << "Relative values based on mobs_per_min: " << std::endl;
  // std::cout << "In:           : " << rel_int << std::endl;
  // std::cout << "Spirit        : " << rel_spirit << std::endl;
  // std::cout << "Spell power   : " << rel_sp << std::endl;
  // std::cout << "Wand dps      : " << rel_wand << std::endl;
  // std::cout << "Mp5           : " << rel_mp5 << std::endl;
  // return {rel_int, rel_spirit, rel_sp, rel_wand, rel_mp5};
  return {base_fr, int_fr, spirit_fr, sp_fr, wand_fr, mp5_fr};
}

}  // namespace css
