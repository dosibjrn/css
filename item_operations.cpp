#include "item_operations.h"

#include <iostream>

namespace css
{

void CoutItem(const Item& i)
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

void AddItem(const Item &i, PriestCharacter* c)
{
  Item set_bonus_pre = c->set_bonuses.getTotalBonus();
  c->set_bonuses.AddItem(i);
  Item set_bonus_post = c->set_bonuses.getTotalBonus();
  Item i_tmp = i;
  if (set_bonus_pre.name != set_bonus_post.name) {
    AddToItemWithMul(set_bonus_pre, -1.0f, &i_tmp);  
    AddToItemWithMul(set_bonus_post, 1.0f, &i_tmp);  
  }
  c->strength += i_tmp.strength;
  c->agility += i_tmp.agility;
  c->intelligence += i_tmp.intelligence;
  c->stamina += i_tmp.stamina;
  c->spirit += i_tmp.spirit;
  c->sp += i_tmp.sp;
  c->sp_shadow += i_tmp.sp_shadow;
  c->sp_healing += i_tmp.sp_healing;
  c->mp5 += i_tmp.mp5;
  c->spell_crit += i_tmp.spell_crit;
}

void RemoveItem(const Item &i, PriestCharacter* c)
{
  Item set_bonus_pre = c->set_bonuses.getTotalBonus();
  c->set_bonuses.RemoveItem(i);
  Item set_bonus_post = c->set_bonuses.getTotalBonus();
  Item i_tmp = i;
  if (set_bonus_pre.name != set_bonus_post.name) {
    // if there was a change, we want the item to be bigger/better
    AddToItemWithMul(set_bonus_post, -1.0f, &i_tmp);  
    // pre was bigger as we removed something -> add it with 1.0f
    AddToItemWithMul(set_bonus_pre, 1.0f, &i_tmp);
  }
  c->strength -= i_tmp.strength;
  c->agility -= i_tmp.agility;
  c->intelligence -= i_tmp.intelligence;
  c->stamina -= i_tmp.stamina;
  c->spirit -= i_tmp.spirit;
  c->sp -= i_tmp.sp;
  c->sp_shadow -= i_tmp.sp_shadow;
  c->sp_healing -= i_tmp.sp_healing;
  c->mp5 -= i_tmp.mp5;
  c->spell_crit -= i_tmp.spell_crit;
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



