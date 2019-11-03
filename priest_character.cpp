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

}  // namespace css

