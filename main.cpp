#include <iostream>
#include <cmath>
#include <vector>

namespace css {

struct Wand {
  float dps = 0.0f;
  School type = School::Shadow;
  float speed = 1.5f;
};
struct Water {
  float per_tick = 0.0f;
  float num_ticks = 0.0f;
};

PriestCharacter PaistiLvl51() {
  PriestCharacter paisti;
  paisti.level = 51;
  paisti.base_mana = 911;
  paisti.intelligence = 209;
  paisti.spirit = 225;
  paisti.sp = 82;
  paisti.wand.dps = 55;
  paisti.wand.speed = 1.3f;
  paisti.wand.type = School::Nature;
  paisti.talents.wand_specialization = 5;
  paisti.talents.meditation = 1;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  return paisti;
}



float ModifiedShadow(const PriestCharacter& c, const Spell& s)
{   
  const auto& t = c.talents;
  float damage = s.damage;
  if (t.shadowform) {
    damage *= 1.15f;
  }
  if (t.darkness) {
    damage *= (1.0f+(0.02f*t.darkness));
  }
  damage += s.modifier * c.sp;
  damage += s.modifier * c.sp_shadow;
  return damage;
}
Spell Shield(const PriestCharacter& c, int rank) {
  Spell s;
  s.cost = 355; // This is rank8
  s.shield = 6220;
  // std::cout << "Check Shield impl." << std::endl;
  return s;
}

PriestCharacter PaistiLvl53() {
  PriestCharacter paisti;
  paisti.level = 53;
  paisti.base_mana = 911+47;
  paisti.intelligence = 169;
  paisti.spirit = 234;
  paisti.sp = 128;
  paisti.wand.dps = 55;
  paisti.wand.speed = 1.3f;
  paisti.wand.type = School::Nature;
  paisti.talents.wand_specialization = 5;
  paisti.talents.meditation = 3;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  return paisti;
}


struct Mob {
  float health = 0.0f;
};

class Stats {
 public:
  Stats(const PriestCharacter& c) : c_(c) {}
  float getMaxMana() const
  {
    return 15.0f*c_.intelligence+c_.base_mana;
  }

  float getManaRegenTickOutOfFsr() const
  {
    return 12.5+c_.spirit*0.25f + mp5Tick();
  }

  float getManaRegenTickUnderFsr() const
  {
    return c_.talents.meditation*0.05*(12.5+c_.spirit*0.25f) + mp5Tick();
  }
  float getManaRegenTickSpiritTapUnderFsr() const
  {
    return c_.talents.meditation*0.05*(12.5+c_.spirit*0.25f*2.0f)  // double spirit == double meditation
        + 12.5*0.5f + c_.spirit*0.25f // half of double spirit normal regen == normal regen
        + mp5Tick();
  }
  float getManaRegenTickSpiritTapOutOfFsr() const
  {
    return 12.5 + c_.spirit*0.25f*2.0f
        + mp5Tick();
  }
  float spellCritMul() const
  {
    return 1.0f + 0.01*(c_.intelligence/59.5f);
  }
 private:
  float mp5Tick() const { return c_.mp5/5.0f*2.0f; }
  const PriestCharacter& c_;
};

struct FightResult {
  float mana_drop = 0.0f;
  float time_to_out_of_fsr = 0.0f;
  float duration = 0.0f;
  float damage_spells = 0.0f;
  float damage_wand = 0.0f;
  float time_wand = 0.0f;
  float spell_total_cost = 0.0f;
  float dps = 0.0f;
  float mobs_per_min = 0.0f;
  float regen_for_next_time = 0.0f;
  float drink_to_full_time = 0.0f;
  float mobs_per_drink = 0.0f;
  float shields = 0.0f;
  float blasts = 0.0f;
  float flay_ticks = 0.0f;
  float touches = 0.0f;
  float blast_down_rank = 0.0f;
  float flay_down_rank = 0.0f;
  float diff = 0.0f;
};

FightResult fightResultSum(const FightResult &a, const FightResult &b) {
  FightResult sum;
  sum.mana_drop = a.mana_drop + b.mana_drop;
  sum.time_to_out_of_fsr = a.time_to_out_of_fsr + b.time_to_out_of_fsr;
  sum.duration = a.duration + b.duration;
  sum.damage_spells = a.damage_spells + b.damage_spells;
  sum.damage_wand = a.damage_wand + b.damage_wand;
  sum.time_wand = a.time_wand + b.time_wand;
  sum.spell_total_cost = a.spell_total_cost + b.spell_total_cost;
  sum.dps = a.dps + b.dps;
  sum.mobs_per_min = a.mobs_per_min + b.mobs_per_min;
  sum.regen_for_next_time = a.regen_for_next_time + b.regen_for_next_time;
  sum.drink_to_full_time = a.drink_to_full_time + b.drink_to_full_time;
  sum.mobs_per_drink = a.mobs_per_drink + b.mobs_per_drink;
  sum.shields = a.shields + b.shields;
  sum.blasts = a.blasts + b.blasts;
  sum.flay_ticks = a.flay_ticks + b.flay_ticks;
  sum.touches = a.touches + b.touches;
  sum.blast_down_rank = a.blast_down_rank + b.blast_down_rank;
  sum.flay_down_rank = a.flay_down_rank + b.flay_down_rank;
  sum.diff = a.diff + b.diff;
  return sum;
}

void fightResultMul(float mul, FightResult* a)
{
  a->mana_drop *= mul;
  a->time_to_out_of_fsr *= mul;
  a->duration *= mul;
  a->damage_spells *= mul;
  a->damage_wand *= mul;
  a->time_wand *= mul;
  a->spell_total_cost *= mul;
  a->dps *= mul;
  a->mobs_per_min *= mul;
  a->regen_for_next_time *= mul;
  a->drink_to_full_time *= mul;
  a->mobs_per_drink *= mul;
  a->shields *= mul;
  a->blasts *= mul;
  a->flay_ticks *= mul;
  a->touches *= mul;
  a->blast_down_rank *= mul;
  a->flay_down_rank *= mul;
  a->diff *= mul;
}

void coutFightResult(const FightResult& a)
{
  std::cout << "mana_drop :           " << a.mana_drop << std::endl;
  std::cout << "time_to_out_of_fsr :  " << a.time_to_out_of_fsr << std::endl;
  std::cout << "duration :            " << a.duration << std::endl;
  std::cout << "damage_spells :       " << a.damage_spells << std::endl;
  std::cout << "damage_wand :         " << a.damage_wand << std::endl;
  std::cout << "time_wand :           " << a.time_wand << std::endl;
  std::cout << "spell_total_cost :    " << a.spell_total_cost << std::endl;
  std::cout << "dps :                 " << a.dps << std::endl;
  std::cout << "mobs_per_min :        " << a.mobs_per_min << std::endl;
  std::cout << "regen_for_next_time : " << a.regen_for_next_time << std::endl;
  std::cout << "mobs_per_drink :      " << a.mobs_per_drink << std::endl;
  std::cout << "shields :             " << a.shields << std::endl;
  std::cout << "blasts :              " << a.blasts << std::endl;
  std::cout << "flay_ticks :          " << a.flay_ticks << std::endl;
  std::cout << "touches :             " << a.touches << std::endl;
  std::cout << "blast_down_rank :     " << a.blast_down_rank << std::endl;
  std::cout << "flay_down_rank :      " << a.flay_down_rank << std::endl;
  std::cout << "diff:                 " << a.diff << std::endl;
}
}  // namespace css

int main(int argc, char** argv)
{
  if (argc > 1) {
    css::globals::time_to_pull = std::atof(argv[1]);
    std::cout << "Setting time to pull: " << css::globals::time_to_pull << std::endl;
  }
  std::vector<css::FightResult> results(6);
  css::Mob mob;
  std::vector<float> mob_healths{3000, 3200, 3400, 3600};
  std::vector<float> multis{1.1f, 1.2f, 1.3f};
  int count = mob_healths.size()*multis.size();
  for (auto mob_health : mob_healths) {
    // for (auto medi : meditations) {
      for (auto multi : multis) {
        mob.health = mob_health;
        auto paisti = css::PaistiLvl53();
        // paisti.sp += 200;
        // paisti.spirit += 50;
        // paisti.talents.meditation = medi;
        // paisti.wand.dps = 40;
        auto result = css::RelativeValues(paisti, multi, mob);
        int i = 0;
        for (auto res : result) {
          fightResultMul(1.0f/count, &result[i]);
          results[i] = fightResultSum(result[i], results[i]);
          i++;
        }
      }
    // }
  }
  std::cout << std::endl;
  std::cout << "Average fight results:" << std::endl;
  std::cout << "***************" << std::endl;
  std::cout << "Int" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[1]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Spirit" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[2]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Sp" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[3]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Wand dps" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[4]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Mp5" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[5]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Base line" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[0]);
  std::cout << std::endl;

  std::cout << "Average relative values based on mobs_per_min:" << std::endl;
  std::cout << "Base Int Spi Sp WandDps Mp5" << std::endl;
  float ref = results[2].diff;
  for (auto res : results) {
    std::cout << res.diff/ref << " ";
  }
  std::cout << std::endl;

  css::Stats s(css::PaistiLvl51());
  std::cout << "Baseline mana stats: " << std::endl;
  std::cout << "Max mana: " << s.getMaxMana() << std::endl;
  std::cout << "In fsr tick: " << s.getManaRegenTickUnderFsr() << std::endl;
  std::cout << "No fsr tick: " << s.getManaRegenTickOutOfFsr() << std::endl;
  std::cout << "In fsr spirit tap tick: " << s.getManaRegenTickSpiritTapUnderFsr() << std::endl;
  std::cout << "No fsr spirit tap tick: " << s.getManaRegenTickSpiritTapOutOfFsr() << std::endl;



  return 0;
}


// TODO: Mental agility to instant spells
