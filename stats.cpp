#include "stats.h"

#include <iostream>

#include "spells_priest_disc.h"

namespace css 
{

float Stats::getEffectiveHp() const
{
  Spell shield = Shield(c_, 10); 
  float ehp = c_.base_hp + (c_.stamina + 70)*10.0f + 2.0f*shield.shield;
  return ehp;
}


void Stats::CoutStats() const
{
  std::cout << "Effective hp: " << getEffectiveHp() << std::endl;
  std::cout << "Effective mana: " << getEffectiveMana(100, 2.0/3.0) << std::endl;
  std::cout << "Raw max mana: " << getMaxMana() << std::endl;
  std::cout << "Int: " << c_.intelligence << std::endl;
  std::cout << "Stam: " << c_.stamina << std::endl;
  std::cout << "Spirit: " << c_.spirit << std::endl;
  std::cout << "Strength: " << c_.strength << std::endl;
  std::cout << "Agility: " << c_.agility << std::endl;

  std::cout << "Mp5: " << c_.mp5 << std::endl;
  std::cout << "Sp: " << c_.sp << std::endl;
  std::cout << "Sp shadow: " << c_.sp_shadow << std::endl;
  std::cout << "Sp healing: " << c_.sp_healing << std::endl;
  std::cout << "Spell crit: " << c_.spell_crit << std::endl;
  std::cout << "Spell hit: " << c_.spell_hitell_hit << std::endl;
  std::cout << "Arcane res: " << c_.arcane_res << std::endl;
  std::cout << "Nature res: " << c_.nature_res << std::endl;
  std::cout << "Fire res: " << c_.fire_res << std::endl;
  std::cout << "Frost res: " << c_.frost_res << std::endl;
  std::cout << "Shadow res: " << c_.shadow_res << std::endl;
  std::cout << "Armor: " << c_.armor << std::endl;
  std::cout << "Defense: " << c_.defense << std::endl;
  std::cout << "Dodge: " << c_.dodge << std::endl;
  std::cout << "Parry: " << c_.parry << std::endl;
}

}  // namespace css
