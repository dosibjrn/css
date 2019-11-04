#include "spells_priest_shadow.h"

#include "school.h"

namespace css
{

float ModifiedShadow(const PriestCharacter& c, const Spell& s)
{   
  const auto& t = c.talents;
  float damage = s.damage;
  if (t.shadowform) {
    damage *= 1.15f;
  }
  if (t.darkness) {
    damage *= (1.0f+(0.02f*t.darkness));
  }
  damage += s.modifier * c.sp;
  damage += s.modifier * c.sp_shadow;
  return damage;
}


// https://www.reddit.com/r/classicwow/comments/95abc8/list_of_spellcoefficients_1121/
// for modifiers
// classic wowhead for vals
Spell MindBlast(const PriestCharacter& c, int rank)
{
  auto &t = c.talents;
  Spell s;
  s.type = School::Shadow;
  s.modifier = 0.4285f;
  s.cast_time = 1.5f;
  s.cd = 8.0f - (0.5f * t.improved_mind_blast);
  switch(rank) {
    case 1:
      s.damage = 0.5f*(42.0f+46.0f);
      s.cost = 50.0f;
      s.level_req = 10;
      break;
    case 2:
      s.damage = 0.5f*(76.0f+83.0f);
      s.cost = 80.0f;
      s.level_req = 16;
      break;
    case 3:
      s.damage = 0.5f*(117.0f+126.0f);
      s.cost = 110.0f;
      s.level_req = 22;
      break;
    case 4:
      s.damage = 0.5f*(174.0f+184.0f);
      s.cost = 150.0f;
      s.level_req = 28;
      break;
    case 5:
      s.damage = 0.5f*(225.0f+239.0f);
      s.cost = 185.0f;
      s.level_req = 34;
      break;
    case 6:
      s.damage = 0.5f*(288.0f+307.0f);
      s.cost = 225.0f;
      s.level_req = 40;
      break;
    case 7:
      s.damage = 0.5f*(356.0f+377.0f);
      s.cost = 265.0f;
      s.level_req = 46;
      break;
    case 8:
      s.damage = 0.5f*(437.0f+461.0f);
      s.cost = 310;
      s.level_req = 52;
      break;
    case 9:
      s.damage = 0.5f*(508.0f+537.0f);
      s.cost = 350;
      s.level_req = 58;
      break;
    default:
      s.damage = 0.5f*(508.0f+537.0f);
      s.cost = 350;
      s.level_req = 58;
      break;
  }

  s.damage = ModifiedShadow(c, s);
  return s;
}

Spell Swp(const PriestCharacter&c, int rank)
{
  Spell s;
  s.type = School::Shadow;
  s.modifier = 1.0f/6.0f;
  s.cast_time = 1.5f;
  s.dot = true;
  s.can_crit = false;

  switch(rank) {
    case 1:
      s.damage = 30/6.0f;
      s.cost = 25;
      s.level_req = 4;
      break;
    case 2:
      s.damage = 66/6.0f;
      s.cost = 50;
      s.level_req = 10;
      break;
    case 3:
      s.damage = 132/6.0f;
      s.cost = 95;
      s.level_req = 18;
      break;
    case 4:
      s.damage = 234/6.0f;
      s.cost = 155;
      s.level_req = 26;
      break;
    case 5:
      s.damage = 366/6.0f;
      s.cost = 230;
      s.level_req = 34;
      break;
    case 6:
      s.damage = 510/6.0f;
      s.cost = 305;
      s.level_req = 42;
      break;
    case 7:
      s.damage = 672/6.0f;
      s.cost = 385;
      s.level_req = 50;
      break;
    case 8:
      s.damage = 852/6.0f;
      s.cost = 470;
      s.level_req = 58;
    default:
      s.damage = 852/6.0f;
      s.cost = 470;
      s.level_req = 58;
      break;
  }

  s.damage = ModifiedShadow(c, s);
  s.tick_after = 3.0f;
  s.num_ticks = 6 + c.talents.imp_swp;
  return s;
}

Spell MindFlay(const PriestCharacter& c, int rank, int ticks)
{
  Spell s;
  s.type = School::Shadow;
  s.modifier = 0.45f/3.0f;
  s.cast_time = 1.0f*ticks;
  s.can_crit =  false;
  switch (rank) {
    case 1 :
      s.cost = 45.0f;
      s.damage = 75.0f/3.0f;
      s.level_req = 20;
      break;
    case 2 :
      s.cost = 70.0f;
      s.damage = 126.0f/3.0f;
      s.level_req = 28;
      break;
    case 3 :
      s.cost = 100.0f;
      s.damage = 186.0f/3.0f;
      s.level_req = 36;
      break;
    case 4 :
      s.cost = 135.0f;
      s.damage = 261.0f/3.0f;
      s.level_req = 44;
      break;
    case 5 :
      s.cost = 165.0f;
      s.damage = 330.0f/3.0f;
      s.level_req = 52;
      break;
    case 6 :
      s.cost = 205.0f;
      s.damage = 426.0f/3.0f;
      s.level_req = 60;
      break;
    default:
      s.cost = 205.0f;
      s.damage = 426.0f/3.0f;
      break;
  }

  s.damage = ModifiedShadow(c, s);
  s.tick_after = 1.0f;
  s.num_ticks = ticks;
  s.channeled = true;
  // std::cout << "Flay rank: " << rank << ", dama: " << s.damage << ", cost: " << s.cost << std::endl;
  return s;
}

Spell VampiricEmbrace()
{
  Spell s;
  s.cost = 40;
  // std::cout << "Check VampiricEmbrace impl." << std::endl;
  return s;
}

Spell TouchOfWeakness(const PriestCharacter& c, int rank) {
  Spell s;
  s.type = School::Shadow;
  // Rank 5
  s.cost = 145;
  s.damage = 48;
  s.damage = ModifiedShadow(c, s);
  // std::cout << "Check TouchOfWeakness impl." << std::endl;
  return s;
}

}  // namespace css

