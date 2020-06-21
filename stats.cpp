#include "stats.h"

#include <algorithm>
#include <iostream>

#include "spells_priest_disc.h"
#include "water.h"

namespace css 
{


float Stats::getManaRegenPerSecondDrinking(bool under_fsr, bool spirit_tap)
{
  Water w = BestWater(c_);
  float mana_per_s = w.per_tick/3.0f;
  if (spirit_tap) {
    if (under_fsr) {
      mana_per_s += 0.5f*getManaRegenTickSpiritTapUnderFsr();
    } else {
      mana_per_s += 0.5f*getManaRegenTickSpiritTapOutOfFsr();
    }
  } else {
    if (under_fsr) {
      mana_per_s += 0.5f*getManaRegenTickUnderFsr();
    } else {
      mana_per_s += 0.5f*getManaRegenTickOutOfFsr();
    }
  }
  return mana_per_s;
}

float Stats::getEffectiveHp() const
{
  Spell shield = Shield(c_, 10); 
  float ehp = c_.base_hp + (c_.stamina + 70)*global::assumptions.all_stats_mul*10.0f + 2.0f*shield.shield;
  return ehp;
}

float Stats::getEffectiveHp(float attacker_level, float attacker_attack, float phys, 
                            float arcane, float nature, float fire, float frost, float shadow, float holy) const
{
  float school_sum = phys + arcane + nature + fire + frost + shadow + holy;
  phys /= school_sum;
  arcane /= school_sum;
  fire /= school_sum;
  frost /= school_sum;
  shadow /= school_sum;
  holy /= school_sum;
  nature /= school_sum;

  const float agi_to_dodge_ratio = 20;  // TODO check
  const float agi = c_.agility*global::assumptions.all_stats_mul;
  float phys_reduction = (c_.armor + agi*2) / (c_.armor + agi*2 + 400.0f + 85.0f * (attacker_level + 4.5f * (attacker_level - 59.0f)));
  float phys_through = std::max(0.0f, std::min(1.0f - phys_reduction, 1.0f));
  phys_through *= std::max(0.0f, std::min(1.0f, (100.0f - (c_.dodge + (agi/agi_to_dodge_ratio) + (c_.defense - attacker_attack) * 0.04f))/100.0f));
  if (c_.talents.shadowform) {
    phys_through *= 0.85f;
  }

  float arcane_reduction = std::max(0.0f, std::min(1.0f, (c_.arcane_res / (attacker_level * 5)) * 0.75f));
  float nature_reduction = std::max(0.0f, std::min(1.0f, (c_.nature_res / (attacker_level * 5)) * 0.75f));
  float fire_reduction = std::max(0.0f, std::min(1.0f, (c_.fire_res / (attacker_level * 5)) * 0.75f));
  float frost_reduction = std::max(0.0f, std::min(1.0f, (c_.frost_res / (attacker_level * 5)) * 0.75f));
  float shadow_reduction = std::max(0.0f, std::min(1.0f, (c_.shadow_res / (attacker_level * 5)) * 0.75f));

  Spell shield = Shield(c_, 10); 
  float ehp = c_.base_hp + (c_.stamina + 70)*global::assumptions.all_stats_mul*10.0f + 2.0f*shield.shield + c_.bonus_hp;
  float ehp_was = ehp;
  ehp /= (phys*phys_through + arcane*(1.0f - arcane_reduction) + nature*(1.0f - nature_reduction) + fire*(1.0f - fire_reduction)
          + frost*(1.0f - frost_reduction) + shadow*(1.0f - shadow_reduction) + holy);
  // if (c_.armor > 50 || c_.frost_res > 50 || c_.dodge > 50) {
  // if (c_.armor > 3050) {
  if (0) {
    std::cout << "ehp: " << ehp_was << " -> " << ehp << std::endl;
    std::cout << "armor: " << c_.armor << ", dodge: " << c_.dodge << ", phys_reduction: " << phys_reduction << ", phys_through: " << phys_through << std::endl;
    std::cout << "arcane: " << c_.arcane_res << " -> red: " << arcane_reduction << std::endl;
    std::cout << "nature: " << c_.nature_res << " -> red: " << nature_reduction << std::endl;
    std::cout << "fire: " << c_.fire_res << " -> red: " << fire_reduction << std::endl;
    std::cout << "frost: " << c_.frost_res << " -> red: " << frost_reduction << std::endl;
    std::cout << "shadow: " << c_.shadow_res << " -> red: " << shadow_reduction << std::endl;
  }
  return ehp;
}

float Stats::getEffectiveHpPvp() const
{
  float attacker_level = 60;
  float attacker_attack = 300;
  float phys = 0.45f;
  float arcane = 0.05f;
  float nature = 0.05f;
  float fire = 0.1f;
  float frost = 0.2f;
  float shadow = 0.13f;
  float holy = 0.05f;
  return getEffectiveHp(attacker_level, attacker_attack, phys, 
                        arcane, nature, fire, frost, shadow, holy);
}


void Stats::CoutStats() const
{
  std::cout << "Effective hp: " << getEffectiveHp() << std::endl;
  std::cout << "Effective mana: " << getEffectiveMana(100.0f, 2.0f/3.0f) << std::endl;
  std::cout << "Raw max mana: " << getMaxMana() << std::endl;
  const float mul = global::assumptions.all_stats_mul;
  std::cout << "Int: " << c_.intelligence*mul << std::endl;
  std::cout << "Stam: " << c_.stamina*mul << std::endl;
  std::cout << "Spirit: " << c_.spirit*mul << std::endl;
  std::cout << "Strength: " << c_.strength*mul << std::endl;
  std::cout << "Agility: " << c_.agility*mul << std::endl;
  std::cout << "All stats multiplier (taken into account above): " << mul << std::endl;

  std::cout << "Bonus mana: " << c_.bonus_mana << std::endl;
  std::cout << "Bonus hp: " << c_.bonus_hp << std::endl;

  std::cout << "Mp5: " << c_.mp5 << std::endl;
  std::cout << "Sp: " << c_.sp << std::endl;
  std::cout << "Sp shadow: " << c_.sp_shadow << std::endl;
  std::cout << "Sp healing: " << c_.sp_healing << std::endl;
  std::cout << "Spell crit: " << c_.spell_crit << std::endl;
  std::cout << "Spell hit: " << c_.spell_hit << std::endl;
  std::cout << "Arcane res: " << c_.arcane_res << std::endl;
  std::cout << "Nature res: " << c_.nature_res << std::endl;
  std::cout << "Fire res: " << c_.fire_res << std::endl;
  std::cout << "Frost res: " << c_.frost_res << std::endl;
  std::cout << "Shadow res: " << c_.shadow_res << std::endl;
  std::cout << "Armor: " << c_.armor << std::endl;
  std::cout << "Defense: " << c_.defense << std::endl;
  std::cout << "Dodge: " << c_.dodge << std::endl;
  std::cout << "Parry: " << c_.parry << std::endl;
}

}  // namespace css
