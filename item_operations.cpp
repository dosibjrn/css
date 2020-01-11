#include "item_operations.h"

#include <iostream>

#include "priest_character.h"

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
  if (i.spell_hit) std::cout << "spell_hit: " << i.spell_hit << std::endl;

  if (i.arcane_res) std::cout << "arcane_res: " << i.arcane_res << std::endl;
  if (i.nature_res) std::cout << "nature_res: " << i.nature_res << std::endl;
  if (i.fire_res) std::cout << "fire_res: " << i.fire_res << std::endl;
  if (i.frost_res) std::cout << "frost_res: " << i.frost_res << std::endl;
  if (i.shadow_res) std::cout << "shadow_res: " << i.shadow_res << std::endl;

  if (i.armor) std::cout << "armor: " << i.armor << std::endl;
  if (i.defense) std::cout << "defense: " << i.defense << std::endl;
  if (i.dodge) std::cout << "dodge: " << i.dodge << std::endl;
  if (i.parry) std::cout << "parry: " << i.parry << std::endl;
}

void AddItem(const Item &i, PriestCharacter* c)
{
  Item i_tmp = i;
  if (i.slot != "Set bonuses") {
    Item set_bonus_pre = c->set_bonuses.getTotalBonus();
    c->set_bonuses.AddItem(i);
    Item set_bonus_post = c->set_bonuses.getTotalBonus();
    if (set_bonus_pre.name != set_bonus_post.name) {
      AddToItemWithMul(set_bonus_pre, -1.0f, &i_tmp);  
      AddToItemWithMul(set_bonus_post, 1.0f, &i_tmp);  
    }
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
  c->spell_hit += i_tmp.spell_hit;

  c->arcane_res += i_tmp.arcane_res;
  c->nature_res += i_tmp.nature_res;
  c->fire_res += i_tmp.fire_res;
  c->frost_res += i_tmp.frost_res;
  c->shadow_res += i_tmp.shadow_res;

  c->armor += i_tmp.armor;
  c->defense += i_tmp.defense;
  c->dodge += i_tmp.dodge;
  c->parry += i_tmp.parry;
}

void RemoveItem(const Item &i, PriestCharacter* c)
{
  Item i_tmp = i;
  if (i.slot != "Set bonuses") {
    Item set_bonus_pre = c->set_bonuses.getTotalBonus();
    c->set_bonuses.RemoveItem(i);
    Item set_bonus_post = c->set_bonuses.getTotalBonus();
    if (set_bonus_pre.name != set_bonus_post.name) {
      // if there was a change, we want the item to be bigger/better
      AddToItemWithMul(set_bonus_post, -1.0f, &i_tmp);  
      // pre was bigger as we removed something -> add it with 1.0f
      AddToItemWithMul(set_bonus_pre, 1.0f, &i_tmp);
    }
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
  c->spell_hit -= i_tmp.spell_hit;

  c->arcane_res -= i_tmp.arcane_res;
  c->nature_res -= i_tmp.nature_res;
  c->fire_res -= i_tmp.fire_res;
  c->frost_res -= i_tmp.frost_res;
  c->shadow_res -= i_tmp.shadow_res;

  c->armor -= i_tmp.armor;
  c->defense -= i_tmp.defense;
  c->dodge -= i_tmp.dodge;
  c->parry -= i_tmp.parry;
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
  o->spell_hit += mul*i.spell_hit;

  o->arcane_res += mul*i.arcane_res;
  o->nature_res += mul*i.nature_res;
  o->fire_res += mul*i.fire_res;
  o->frost_res += mul*i.frost_res;
  o->shadow_res += mul*i.shadow_res;

  o->armor += mul*i.armor;
  o->defense += mul*i.defense;
  o->dodge += mul*i.dodge;
  o->parry += mul*i.parry;

  o->source += ", " + i.source;
}

}  // namespace css



