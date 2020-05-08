#include "cooldown.h"

namespace css
{

void ApplyCooldownEffects(const Cooldowns& cooldowns, Spell* s)
{
  // ZG trinket
  bool hazz_active = cooldowns.find("hazza'rah's") != cooldowns.end() && cooldowns.at("hazza'rah's").active;
  if (hazz_active) {
    s->cost *= 0.85f;
    if (s->name == "Greater Heal") {
      s->cast_time *= 0.6f;
    }
  }
}

Cooldowns SetsToCooldowns(const SetBonuses& sb)
{
  Cooldowns out;
  if (sb.HasBonus("hazza'rah's 1")) {
    Cooldown cd;
    cd.name = "hazza'rah's";
    out["hazza'rah's"] = cd;
  }
  return out;
}

}  // namespace css
