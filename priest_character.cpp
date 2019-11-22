#include "priest_character.h"

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
  paisti.wand.dps = 62.6;
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
  paisti.wand.dps = 62.6;
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
  paisti.wand.dps = 62.6;
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
  paisti.wand.dps = 62.6;
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
  paisti.wand.dps = 62.6;
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
  c.talents.meditation = 3;
  c.talents.spiritual_guidance = 5;
  c.talents.spiritual_healing = 5;
  c.talents.imp_renew = 3;
  c.talents.improved_healing = 3;
  c.talents.divine_fury = 5;
  return c;
}

}  // namespace css

