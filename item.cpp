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
}

}  // namespace css



