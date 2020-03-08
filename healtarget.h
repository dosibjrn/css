#pragma once

namespace css
{

struct Heal {
  float amount;
  float lands;
};

struct HealTarget {
  std::string name;
  std::string id;

  bool alive = true;

  float deficit = 0.0f;

  std::vector<Heal> incoming;
  float incoming_sum = 0.0f;;

  //! damage taken per second
  float dtps = 0.0f;
  float damage_taken = 0.0f;
};

}  // namespace css
