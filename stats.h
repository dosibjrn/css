#pragma once

#include "assumptions.h"
#include "priest_character.h"

namespace css
{

class Stats {
 public:
  Stats(const PriestCharacter& c) : c_(c) {}
  void CoutStats() const;
  float getMaxMana() const
  {
    return (15.0f*c_.intelligence*global::assumptions.all_stats_mul + c_.base_mana)*(1.0f + c_.talents.mental_strength*0.02f);
  }

  float getManaRegenTickOutOfFsr() const
  {
    return 12.5f + c_.spirit*global::assumptions.all_stats_mul*0.25f + mp5Tick();
  }

  float getManaRegenTickUnderFsr() const
  {
    int meditation = c_.talents.meditation;
    if (c_.set_bonuses.getTotalBonus().name.find("transcendence 3") != std::string::npos) {
      meditation += 3;
    } else if (c_.set_bonuses.getPartial() && c_.set_bonuses.getTotalBonus().name.find("transcendence 2") != std::string::npos) {
      meditation += 2;
    } else if (c_.set_bonuses.getPartial() && c_.set_bonuses.getTotalBonus().name.find("transcendence 1") != std::string::npos) {
      meditation += 1;
    }
    return meditation*0.05f*(12.5f + c_.spirit*0.25f) + mp5Tick();
  }
  float getManaRegenTickSpiritTapUnderFsr() const
  {
    int meditation = c_.talents.meditation;
    if (c_.set_bonuses.getTotalBonus().name.find("transcendence 3") != std::string::npos) {
      meditation += 3;
    } else if (c_.set_bonuses.getPartial() && c_.set_bonuses.getTotalBonus().name.find("transcendence 2") != std::string::npos) {
      meditation += 2;
    } else if (c_.set_bonuses.getPartial() && c_.set_bonuses.getTotalBonus().name.find("transcendence 1") != std::string::npos) {
      meditation += 1;
    }
    const float spirit = c_.spirit*global::assumptions.all_stats_mul;
    return meditation*0.05f*(12.5f+spirit*0.25f*2.0f)  // double spirit == double meditation
        + 12.5f*0.5f + spirit*0.25f // half of double spirit normal regen == normal regen
        + mp5Tick();
  }
  float getManaRegenTickSpiritTapOutOfFsr() const
  {
    return 12.5f + c_.spirit*global::assumptions.all_stats_mul*0.25f*2.0f
        + mp5Tick();
  }
  float spellCritMul() const
  {
    return 1.0f + 0.01f*(c_.intelligence*global::assumptions.all_stats_mul/59.5f);
  }

  float getEffectiveMana(float duration, float fsr_frac) const 
  {
    int no_fsr_ticks = static_cast<int>((duration * (1.0f - fsr_frac))/2.0f);
    int fsr_ticks = static_cast<int>(duration/2.0f - no_fsr_ticks);
    float effective_mana = getMaxMana() + no_fsr_ticks*getManaRegenTickOutOfFsr()
        + fsr_ticks*getManaRegenTickUnderFsr();
    return effective_mana;
  }

  float getEffectiveHp() const;
  float getEffectiveHp(float attacker_level, float attacker_attack, float phys, 
                       float arcane, float nature, float fire, float frost, float shadow, float holy) const;
  float getEffectiveHpPvp() const;

  float getManaRegenPerSecondDrinking(bool under_fsr = false, bool spirit_tap = false);

 private:
  float mp5Tick() const { return c_.mp5/5.0f*2.0f; }
  const PriestCharacter& c_;
};

}  // namespace css

