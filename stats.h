#pragma once

namespace css
{



class Stats {
 public:
  Stats(const PriestCharacter& c) : c_(c) {}
  float getMaxMana() const
  {
    return 15.0f*c_.intelligence+c_.base_mana;
  }

  float getManaRegenTickOutOfFsr() const
  {
    return 12.5+c_.spirit*0.25f + mp5Tick();
  }

  float getManaRegenTickUnderFsr() const
  {
    return c_.talents.meditation*0.05*(12.5+c_.spirit*0.25f) + mp5Tick();
  }
  float getManaRegenTickSpiritTapUnderFsr() const
  {
    return c_.talents.meditation*0.05*(12.5+c_.spirit*0.25f*2.0f)  // double spirit == double meditation
        + 12.5*0.5f + c_.spirit*0.25f // half of double spirit normal regen == normal regen
        + mp5Tick();
  }
  float getManaRegenTickSpiritTapOutOfFsr() const
  {
    return 12.5 + c_.spirit*0.25f*2.0f
        + mp5Tick();
  }
  float spellCritMul() const
  {
    return 1.0f + 0.01*(c_.intelligence/59.5f);
  }
 private:
  float mp5Tick() const { return c_.mp5/5.0f*2.0f; }
  const PriestCharacter& c_;
};

}  // namespace css

