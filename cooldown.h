#pragma once

#include <map>
#include <string>

#include "spell.h"
#include "set_bonuses.h"

namespace css
{

// So at start of combat we check which cooldowns we have due to gear

struct Cooldown
{
  std::string name = "";  // this is enough to describe the effect -> implementation elsewhere
  bool active = false;
  int64_t effect_end_ms = 0;
  int64_t off_cooldown_ms = 0;
};

typedef std::map<std::string, Cooldown> Cooldowns;

// Construct cooldowns from set bonuses
Cooldowns SetsToCooldowns(const SetBonuses& sb);

void ApplyCooldownEffects(const Cooldowns& cooldowns, Spell* s);

}  // namespace css

