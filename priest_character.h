#pragma once

namespace css
{

struct PriestCharacter {
  float base_mana = 0.0f;
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

}  // namespace css
