#pragma once

#include <string>

namespace css
{

struct Item {
  std::string name;
  std::string slot;
  float strength = 0.0f;
  float agility = 0.0f;
  float intelligence = 0.0f;
  float stamina = 0.0f;
  float spirit = 0.0f;
  float sp = 0.0f;
  float sp_shadow = 0.0f;
  float sp_healing = 0.0f;
  float mp5 = 0.0f;
};


}  // namespace css
