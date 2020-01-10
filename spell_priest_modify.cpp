#include "spell_priest_modify.h"

#include "assumptions.h"

namespace css
{

void ModifySpell(const PriestCharacter& c, Spell* s)
{
  s->cost *= 1.0f - (c.talents.mental_agility * 0.02f);
  float bonus_sp = c.talents.spiritual_guidance*0.05*c.spirit;
  if (s->shield > 0.0f) {
    s->shield += (c.sp + c.sp_healing + bonus_sp)*s->modifier;
  }
  if (s->healing > 0.0f) {
    s->healing += (c.sp + c.sp_healing + bonus_sp)*s->modifier;
  }
  if (s->damage > 0.0f) {
    s->damage += (c.sp + bonus_sp) * s->modifier;
    if (s->type == School::Shadow) {
      s->damage += c.sp_shadow*s->modifier;
    }
  }

  s->healing *= 1.0f + 0.02*c.talents.spiritual_healing;
  if (s->name == "Lesser Heal" || s->name == "Heal" || s->name == "Greater Heal") {
    s->cost *= 1.0f - 0.05f*c.talents.improved_healing;
  }
  if (s->name == "Heal" || s->name == "Greater Heal" || s->name == "Smite" || s->name == "Holy Fire") {
    s->cast_time -= 0.1*c.talents.divine_fury;
  }
  if (s->instant) {
    s->cost *= 1.0f - 0.02f*c.talents.mental_agility;
  }
  if (s->damage > 0.0f && s->type == School::Shadow) {
    if (c.talents.shadowform) {
      s->damage *= 1.15f;
    }
    if (c.talents.darkness) {
      s->damage *= 1.0f + 0.02f*c.talents.darkness;
    }
  }

  // Crit
  if (s->can_crit) {
    float more_crit = 0.0f;
    if (s->type == School::Holy) {
      more_crit = c.talents.holy_specialization;
    }
    s->damage *= 1.0 + (0.01*(c.spell_crit + more_crit + c.intelligence/59.4));
    float not_overhealing = global::assumptions.healing_from_crit_fraction;
    s->healing *= 1.0 + not_overhealing*(0.01*(c.spell_crit + more_crit + c.intelligence/59.4));
  }
}

}  // namespace css
