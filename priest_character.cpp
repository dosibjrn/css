#include "priest_character.h"

#include "assumptions.h"

namespace css
{

PriestCharacter PaistiLvl51()
{
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

PriestCharacter PaistiLvl53()
{
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

PriestCharacter PaistiLvl60PvPShadow()
{
  PriestCharacter paisti;
  paisti.level = 60;
  paisti.stamina = 208;
  paisti.intelligence = 263;
  paisti.base_mana = 1436.0f;
  paisti.base_hp = 1387.0f;
  paisti.spirit = 204;
  paisti.sp = 215;
  paisti.wand.dps = 62.6f;
  paisti.wand.speed = 1.9f;
  paisti.wand.type = School::Arcane;
  paisti.talents.meditation = 3;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  paisti.talents.improved_mind_blast = 5;
  return paisti;
}

PriestCharacter PaistiLvl60PvPShadowIter2()
{
  PriestCharacter paisti;
  paisti.level = 60;
  paisti.stamina = 206;
  paisti.intelligence = 361;
  paisti.base_mana = 1436.0f;
  paisti.base_hp = 1387.0f;
  paisti.spirit = 211;
  paisti.sp = 149;
  paisti.mp5 = 3;
  paisti.wand.dps = 62.6f;
  paisti.wand.speed = 1.9f;
  paisti.wand.type = School::Arcane;
  paisti.talents.meditation = 3;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  paisti.talents.improved_mind_blast = 5;
  return paisti;
}

PriestCharacter PaistiLvl60PvPShadowIter3()
{
  PriestCharacter paisti;
  paisti.level = 60;
  paisti.stamina = 221;
  paisti.intelligence = 311;
  paisti.base_mana = 1436.0f;
  paisti.base_hp = 1387.0f;
  paisti.spirit = 202;
  paisti.sp = 144;
  paisti.wand.dps = 62.6f;
  paisti.wand.speed = 1.9f;
  paisti.wand.type = School::Arcane;
  paisti.talents.meditation = 3;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  paisti.talents.improved_mind_blast = 5;
  return paisti;
}

PriestCharacter PaistiLvl60PvPShadowIter4()
{
  PriestCharacter paisti;
  paisti.level = 60;
  paisti.stamina = 223;
  paisti.intelligence = 341;
  paisti.base_mana = 1436.0f;
  paisti.base_hp = 1387.0f;
  paisti.spirit = 211;
  paisti.sp = 149;
  paisti.wand.dps = 62.6f;
  paisti.wand.speed = 1.9f;
  paisti.wand.type = School::Arcane;
  paisti.talents.meditation = 3;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  paisti.talents.improved_mind_blast = 5;
  return paisti;
}

PriestCharacter PaistiLvl60PvPShadowRaid1()
{
  PriestCharacter paisti;
  paisti.level = 60;
  paisti.stamina = 246;
  paisti.intelligence = 412;
  paisti.base_mana = 1436.0f;
  paisti.base_hp = 1387.0f;
  paisti.spirit = 268;
  paisti.sp = 166;
  paisti.wand.dps = 62.6f;
  paisti.wand.speed = 1.9f;
  paisti.wand.type = School::Arcane;
  paisti.talents.meditation = 3;
  paisti.talents.imp_swp = 2;
  paisti.talents.shadow_weaving = 5;
  paisti.talents.shadowform = 1;
  paisti.talents.darkness = 5;
  paisti.talents.improved_mind_blast = 5;
  return paisti;
}


PriestCharacter BaseLvl60UdShadow()
{
  PriestCharacter c;
  c.level = 60;
  c.strength = 34;
  c.agility = 38;
  c.stamina = 51;
  c.intelligence = 118;
  c.spirit = 130;
  c.base_mana = 1436.0f;
  c.base_hp = 1387.0f;
  c.talents.meditation = 3;
  c.talents.imp_swp = 2;
  c.talents.shadow_weaving = 5;
  c.talents.shadowform = 1;
  c.talents.darkness = 5;
  c.talents.improved_mind_blast = 5;
  return c;
}

PriestCharacter BaseLvl60UdShadowEnchanted()
{
  PriestCharacter c = BaseLvl60UdShadow();
  c.strength += 4;
  c.agility += 4;
  c.stamina += 11;
  c.intelligence += 27;
  c.spirit += 4;
  c.sp += 30;
  return c;
}

PriestCharacter BaseLvl60HolyDiscHealing()
{
  PriestCharacter c;
  c.level = 60;
  c.strength = 34;
  c.agility = 38;
  c.stamina = 51;
  c.intelligence = 118;
  c.spirit = 130;
  c.base_mana = 1436.0f;
  c.base_hp = 1387.0f;

  c.defense = 300;
  c.armor = 1395;

  c.talents.imp_pws = 3;
  c.talents.meditation = 3;
  c.talents.spiritual_guidance = 5;
  c.talents.spiritual_healing = 5;
  c.talents.imp_renew = 3;
  c.talents.improved_healing = 3;
  c.talents.divine_fury = 5;
  c.talents.holy_specialization = 3;
  return c;
}

PriestCharacter BaseLvl60DiscHolyPvpHealing()
{
  PriestCharacter c;
  c.level = 60;
  c.strength = 34;
  c.agility = 38;
  c.stamina = 51;
  c.intelligence = 118;
  c.spirit = 130;
  c.base_mana = 1436.0f;
  c.base_hp = 1387.0f;

  c.defense = 300;
  c.armor = 1814;

  c.talents.imp_pws = 3;
  c.talents.meditation = 3;
  c.talents.spiritual_guidance = 0;
  c.talents.spiritual_healing = 0;
  c.talents.imp_renew = 3;
  c.talents.improved_healing = 3;
  c.talents.divine_fury = 5;

  c.talents.mental_agility = 5;
  c.talents.mental_strength = 5;
  c.talents.power_infusion = 1;
  c.talents.holy_specialization = 3;
  return c;
}

void ApplyBuffs(PriestCharacter *c) {
  float frac = global::assumptions.buff_fraction;
  c->mp5 += 60.0f*frac;  // major mana potion
  c->mp5 += 50.0f*frac;  // runes
  // c->mp5 += 12.0f*frac;  // brilliant mana oil
  // c->mp5 += 12.0f*frac;  // mageblood potion
  c->mp5 += 10.0f*frac;  // nightfin soup
  c->mp5 += 10.0f*frac;  // wcb
  c->spell_crit += 10.0f*frac;  // ony
  c->spell_crit += 3.0f*frac;  // tribute

  c->spirit += 50; // spirit of zanza
  c->stamina += 50;  // spirit of zanza

  // Songflower
  c->strength += 15.0f*frac;
  c->agility += 15.0f*frac;
  c->intelligence += 15.0f*frac;
  c->spirit += 15.0f*frac;
  c->stamina += 15.0f*frac;
  c->spell_crit += 5.0f*frac;

  // ZG Heart / Spirit of Zandalar
  global::assumptions.all_stats_mul = 1.0f + 0.15f*frac;

  global::assumptions.flask = false;
  if (frac >= 1.0f) {
    global::assumptions.flask = true;
  }
  if (global::assumptions.flask) {
    // Flask of distilled wisdom: -crit to affect only mana.
    c->bonus_mana += 2000.0f;
  }

  if (global::assumptions.fort) {
    c->stamina += 70;
  }
  if (global::assumptions.motw) {
    c->armor += 285*1.35f;
    c->strength += 12*1.35f;
    c->agility += 12*1.35f;
    c->spirit += 12*1.35f;
    c->stamina += 12*1.35f;
    c->intelligence += 12*1.35f;

    c->arcane_res += 20*1.35f;
    c->nature_res += 20*1.35f;
    c->fire_res += 20*1.35f;
    c->frost_res += 20*1.35f;
    c->shadow_res += 20*1.35f;
  }
  if (global::assumptions.spirit) {
    c->spirit += 40;
  }
  if (global::assumptions.ai) {
    c->intelligence += 31;
  }
}

void AddEnchants(PriestCharacter* c) {
  // weapon
  c->sp_healing += 55;

  // bracers
  c->sp_healing += 24;

  // head
  c->sp_healing += 24;
  c->mp5 += 4;
  c->stamina += 10;

  // legs
  c->sp_healing += 24;
  c->mp5 += 4;
  c->stamina += 10;

  // chest, +4 all
  c->intelligence += 4;
  c->spirit += 4;
  c->strength += 4;
  c->agility += 4;
  c->stamina += 4;
}

}  // namespace css

