#include "item.h"

#include <iostream>

namespace css
{

void coutItem(const Item& i)
{
  std::cout << "name: " << i.name << std::endl;
  std::cout << "slot: " << i.slot << std::endl;
  std::cout << "strength: " << i.strength << std::endl;
  std::cout << "agility: " << i.agility << std::endl;
  std::cout << "intelligence: " << i.intelligence << std::endl;
  std::cout << "stamina: " << i.stamina << std::endl;
  std::cout << "spirit: " << i.spirit << std::endl;
  std::cout << "sp: " << i.sp << std::endl;
  std::cout << "sp_shadow: " << i.sp_shadow << std::endl;
  std::cout << "sp_healing: " << i.sp_healing << std::endl;
  std::cout << "mp5: " << i.mp5 << std::endl;
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



