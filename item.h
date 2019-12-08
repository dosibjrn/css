#pragma once

#include <string>

#include "priest_character.h"

namespace css
{

struct Item {
  std::string name = "";
  std::string slot = "";
  float strength = 0.0f;
  float agility = 0.0f;
  float intelligence = 0.0f;
  float stamina = 0.0f;
  float spirit = 0.0f;
  float sp = 0.0f;
  float sp_shadow = 0.0f;
  float sp_healing = 0.0f;
  float mp5 = 0.0f;
  float spell_crit = 0.0f;
  bool unique = false;
};

// TODO: capitalization
void coutItem(const Item& i);
void addItem(const Item &i, PriestCharacter* c);
void removeItem(const Item &i, PriestCharacter* c);
void AddToItemWithMul(const Item& i, float mul, Item* o);

}  // namespace css

