#include "item.h"

#include <iostream>

namespace css
{

void coutItem(const Item& i)
{
  std::cout << "name: " << i.name << std::endl;
  std::cout << "slot: " << i.slot << std::endl;
  if (i.strength) std::cout << "strength: " << i.strength << std::endl;
  if (i.agility) std::cout << "agility: " << i.agility << std::endl;
  if (i.intelligence) std::cout << "intelligence: " << i.intelligence << std::endl;
  if (i.stamina) std::cout << "stamina: " << i.stamina << std::endl;
  if (i.spirit) std::cout << "spirit: " << i.spirit << std::endl;
  if (i.sp) std::cout << "sp: " << i.sp << std::endl;
  if (i.sp_shadow) std::cout << "sp_shadow: " << i.sp_shadow << std::endl;
  if (i.sp_healing) std::cout << "sp_healing: " << i.sp_healing << std::endl;
  if (i.mp5) std::cout << "mp5: " << i.mp5 << std::endl;
  if (i.spell_crit) std::cout << "spell_crit: " << i.spell_crit << std::endl;
}

void addItem(const Item &i, PriestCharacter* c)
{
  c->strength += i.strength;
  c->agility += i.agility;
  c->intelligence += i.intelligence;
  c->stamina += i.stamina;
  c->spirit += i.spirit;
  c->sp += i.sp;
  c->sp_shadow += i.sp_shadow;
  c->sp_healing += i.sp_healing;
  c->mp5 += i.mp5;
  c->spell_crit += i.spell_crit;
}

void removeItem(const Item &i, PriestCharacter* c)
{
  c->strength -= i.strength;
  c->agility -= i.agility;
  c->intelligence -= i.intelligence;
  c->stamina -= i.stamina;
  c->spirit -= i.spirit;
  c->sp -= i.sp;
  c->sp_shadow -= i.sp_shadow;
  c->sp_healing -= i.sp_healing;
  c->mp5 -= i.mp5;
  c->spell_crit -= i.spell_crit;
}

void AddToItemWithMul(const Item& i, float mul, Item* o)
{
  o->strength += mul*i.strength;
  o->agility += mul*i.agility;
  o->intelligence += mul*i.intelligence;
  o->stamina += mul*i.stamina;
  o->spirit += mul*i.spirit;
  o->sp += mul*i.sp;
  o->sp_shadow += mul*i.sp_shadow;
  o->sp_healing += mul*i.sp_healing;
  o->mp5 += mul*i.mp5;
  o->spell_crit += mul*i.spell_crit;
}

}  // namespace css



