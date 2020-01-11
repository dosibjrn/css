#include <map>

#include "assumptions.h"
#include "spell.h"
#include "priest_character.h"
#include "spell_priest_modify.h"

namespace css
{


// Coeffs for holy spells
// Renew: 20% per tick
// Flash Heal 42.85%
// Greater Heal 85.71%

// Flash heal: 
// rank, lvl req, mana cost, heal_min, heal_max:
// 1, 20, 125, 193, 237 
// 2, 26, 155, 269, 325
// 3, 32, 185, 339, 406
// 4, 38, 215, 414, 492
// 5, 44, 265, 534, 633
// 6, 50, 315, 662, 783
// 7, 56, 380, 828, 975
Spell FlashHeal(const PriestCharacter& c, int rank)
{
  std::map<int, std::vector<int>> vals;
  // rank   lvl, mana, min, max
  vals[1] = {20, 125, 193, 237};
  vals[2] = {26, 155, 269, 325};
  vals[3] = {32, 185, 339, 406};
  vals[4] = {38, 215, 414, 492};
  vals[5] = {44, 265, 534, 633};
  vals[6] = {50, 315, 662, 783};
  vals[7] = {56, 380, 828, 975};
  if (rank < 1 || rank > 7) { rank = 7; }
  Spell s;
  s.rank = rank;
  s.name = "Flash Heal";
  s.level_req = vals[rank][0];
  s.cost = vals[rank][1];
  s.healing = 0.5f*(vals[rank][2] + vals[rank][3]);
  s.modifier = 0.4285f;
  s.can_crit = true;
  s.type = School::Holy;

  ModifySpell(c, &s);
  return s;
}

// http://wow.allakhazam.com/wiki/spell_coefficient_(wow)
// Casting a spell that is below level 20 incurs a significant penalty to the coefficient of the spell. The formula for this is:
     // (20 - [Spell Level]) * .0375 = [Penalty]


// Renew:
// duration: 15 (100%, 20% per tick = 5 ticks = 1 per 3 s)
// rank, lvl req, mana cost, heal:
// 1, 8, 30, 45
// 2, 14, 65, 100
// 3, 20, 105, 175
// 4, 26, 140, 245
// 5, 32, 170, 315
// 6, 38, 205, 400
// 7, 44, 250, 510
// 8, 50, 305, 650
// 9, 56, 365, 810
// 10, 60, 410, 970
Spell Renew(const PriestCharacter& c, int rank)
{
  std::map<int, std::vector<int>> vals;
  // rank   lvl, mana, healing
  vals[1] = {8, 30, 45};
  vals[2] = {14, 65, 100};
  vals[3] = {20, 105, 175};
  vals[4] = {26, 140, 245};
  vals[5] = {32, 170, 315};
  vals[6] = {38, 205, 400};
  vals[7] = {44, 250, 510};
  vals[8] = {50, 305, 650};
  vals[9] = {56, 365, 810};
  vals[10] = {60, 410, 970};
  int max_rank = 10;
  if (rank < 1 || rank > max_rank) { rank = max_rank; }
  Spell s;
  s.rank = rank;
  s.name = "Renew";
  s.instant = true;
  s.level_req = vals[rank][0];
  s.cost = vals[rank][1];
  s.healing = vals[rank][2];
  s.over_time = true;

  // ticks
  s.healing = s.healing / 5;
  s.num_ticks = 5;
  s.tick_after = 3.0f;
  s.modifier = 0.20f;
  s.healing *= 1.0f + c.talents.imp_renew*0.05;
  s.can_crit = false;
  s.type = School::Holy;
  ModifySpell(c, &s);

  return s;
}


Spell GreaterHeal(const PriestCharacter& c, int rank) {
  std::map<int, std::vector<int>> vals;
  // rank, level, mana, healmin, healmax
  vals[1] = {40, 370, 924, 1039};
  vals[2] = {46, 455, 1178, 1318};
  vals[3] = {52, 545, 1470, 1642};
  vals[4] = {58, 655, 1813, 2021};
  vals[5] = {60, 710, 1966, 2194};
  int max_rank = 5;
  if (rank < 1 || rank > max_rank) { rank = max_rank; }
  Spell s;
  s.rank = rank;
  s.name = "Greater Heal";
  s.cast_time = 3.0f;
  s.level_req = vals[rank][0];
  s.cost = vals[rank][1];
  s.healing = 0.5f*(vals[rank][2] + vals[rank][3]);
  s.modifier = s.cast_time/3.5f;
  s.can_crit = true;
  ModifySpell(c, &s);

  bool transc8_exists = global::assumptions.transc8_exists;
  if (transc8_exists) {
    if (c.set_bonuses.getTotalBonus().name.find("transcendence 8") != std::string::npos) {
      Spell extra_renew = Renew(c, 5);
      s.healing += extra_renew.healing*extra_renew.num_ticks;
    } else if (c.set_bonuses.getPartial()) {
      int highest = 0;
      for (int i = 1; i < 8; ++i) {
        if (c.set_bonuses.getTotalBonus().name.find("transcendence " + std::to_string(i)) != std::string::npos) {
          highest = i;
        }
      }
      Spell extra_renew = Renew(c, 5);
      s.healing += extra_renew.healing*extra_renew.num_ticks * highest/8.0f;
    }
  }

  return s;
}

Spell Heal(const PriestCharacter& c, int rank) {
  std::map<int, std::vector<int>> vals;
  // rank, level, mana, healmin, healmax
  vals[1] = {16, 155, 307, 353};
  vals[2] = {22, 205, 445, 507};
  vals[3] = {28, 255, 586, 662};
  vals[4] = {34, 450, 1041, 1174};
  int max_rank = 4;
  if (rank < 1 || rank > max_rank) { rank = max_rank; }
  Spell s;
  s.rank = rank;
  s.name = "Heal";
  s.cast_time = 3.0f;
  s.level_req = vals[rank][0];
  s.cost = vals[rank][1];
  s.healing = 0.5f*(vals[rank][2] + vals[rank][3]);
  s.modifier = s.cast_time/3.5f;
  s.can_crit = true;
  s.type = School::Holy;
  ModifySpell(c, &s);
  return s;
}

Spell PrayerOfHealing(const PriestCharacter& c, int rank, int targets)
{
  std::map<int, std::vector<int>> vals;
  // rank, level, mana, healmin, healmax
  vals[1] = {30, 410, 312, 333};
  vals[2] = {40, 560, 458, 487};
  vals[3] = {50, 770, 675, 713};
  vals[4] = {60, 1030, 939, 991};
  vals[5] = {60, 1070, 1041, 1099};
  int max_rank = 5;
  if (rank < 1 || rank > max_rank) { rank = max_rank; }
  Spell s;
  s.rank = rank;
  s.name = "Prayer of Healing";
  s.cast_time = 3.0f;
  s.level_req = vals[rank][0];
  s.cost = vals[rank][1];
  s.healing = 0.5f*(vals[rank][2] + vals[rank][3])*targets;
  s.modifier = (s.cast_time/3.5f)/3.0f;
  s.can_crit = true;
  s.type = School::Holy;
  ModifySpell(c, &s);
  return s;
}

}  // namespace css
