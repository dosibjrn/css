#pragma once

#include <vector>

#include "priest_talents.h"
#include "set_bonuses.h"
#include "spell.h"
#include "wand.h"

namespace css
{

struct PriestCharacter {
  float base_mana = 0.0f;
  float base_hp = 0.0f;
  int level = 0;

  // old school stats
  float strength = 0.0f;
  float agility = 0.0f;
  float stamina = 0.0f;
  float intelligence = 0.0f;
  float spirit = 0.0f;

  // spell stats
  float sp = 0.0f;
  float sp_shadow = 0.0f;
  float sp_healing = 0.0f;
  float mp5 = 0.0f;
  float spell_crit = 0.0f;
  float spell_hit = 0.0f;

  // resistances
  // average_resistance = (target_resistance / (caster_level * 5)) * 0.75
  float arcane_res = 0.0f;
  float nature_res = 0.0f;
  float fire_res = 0.0f;
  float frost_res = 0.0f;
  float shadow_res = 0.0f;

  // def
  // armor: dr% = armor / (armor + 400 + 85 * (attacker_level + 4.5* (attacker_level - 59)))
  float armor = 0.0f;

  // every point of defense gives 0.04 dodge, 0.04 parry, 0.04 reduced chance to be hit, 0.04 reduced chance to be crit
  float defense = 0.0f;

  // total_dodge = dodge + (agi/agi_to_dodge_ratio) + talents + race + item + ((def - enemy_att) * 0.04)
  float dodge = 0.0f;

  // hunters, palas, rogues, warriors can parry. shamans with skill. can't parry from behind.
  float parry = 0.0f;

  Wand wand;
  PriestTalents talents;
  std::vector<Spell> spells_sequence;
  SetBonuses set_bonuses;
};

PriestCharacter PaistiLvl51();
PriestCharacter PaistiLvl53();
PriestCharacter PaistiLvl60PvPShadow();
PriestCharacter PaistiLvl60PvPShadowIter2();
PriestCharacter PaistiLvl60PvPShadowIter3();
PriestCharacter PaistiLvl60PvPShadowIter4();
PriestCharacter PaistiLvl60PvPShadowRaid1();
PriestCharacter BaseLvl60UdShadow();
PriestCharacter BaseLvl60UdShadowEnchanted();
PriestCharacter BaseLvl60HolyDiscHealing();
PriestCharacter BaseLvl60DiscHolyPvpHealing();

void ApplyBuffs(PriestCharacter *c);

void AddEnchants(PriestCharacter* c);

}  // namespace css
