#pragma once

#include <string>

namespace css
{

struct Item {
  std::string name = "";
  std::string slot = "";

  // old school stats
  float strength = 0.0f;
  float agility = 0.0f;
  float intelligence = 0.0f;
  float stamina = 0.0f;
  float spirit = 0.0f;

  // spell stats
  float sp = 0.0f;
  float sp_shadow = 0.0f;
  float sp_healing = 0.0f;
  float mp5 = 0.0f;
  float spell_crit = 0.0f;
  float spell_hit = 0.0f;

  // resistances
  float arcane_res = 0.0f;
  float nature_res = 0.0f;
  float fire_res = 0.0f;
  float frost_res = 0.0f;
  float shadow_res = 0.0f;

  // def
  float armor = 0.0f;
  float defense = 0.0f;
  float dodge = 0.0f;
  float parry = 0.0f;

  bool unique = false;
};

}  // namespace css

