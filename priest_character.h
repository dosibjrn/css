#pragma once

#include <vector>

#include "priest_talents.h"
#include "spell.h"
#include "wand.h"

namespace css
{

struct PriestCharacter {
  float base_mana = 0.0f;
  float base_hp = 0.0f;
  int level = 0;
  int strength = 0;
  int agility = 0;
  int stamina = 0;
  int intelligence = 0;
  int spirit = 0;
  int sp = 0;
  int sp_shadow = 0;
  int sp_healing = 0;
  int mp5 = 0;
  Wand wand;
  PriestTalents talents;
  std::vector<Spell> spells_sequence;
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

}  // namespace css
