#include "dps.h"

#include <iostream>

#include "combat.h"
#include "spells_priest_shadow.h"

namespace css
{

float ShadowDps(const PriestCharacter& c)
{
  float health_sum = 0.0f;
  float dura_sum = 0.0f;
  FightResult resultSum;
  int count = 0;
  // for (float health = 5000; health < 5001; health += 100) {
  for (float health = 3000; health <= 10000; health += 100) {
    Mob dummy_rogue;
    dummy_rogue.health = health;
    std::vector<Spell> spells;
    int maxrank = 1000;
    spells.push_back(MindBlast(c, maxrank)); 
    spells.push_back(Swp(c, maxrank)); 

    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindFlay(c, maxrank, 1));
    spells.push_back(MindBlast(c, maxrank)); 

    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindBlast(c, maxrank)); 

    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindBlast(c, maxrank)); 

    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindBlast(c, maxrank)); 

    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindBlast(c, maxrank)); 

    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindFlay(c, maxrank, 3));
    spells.push_back(MindBlast(c, maxrank)); 

    auto result = FightMob(c, spells, dummy_rogue);
    health_sum += dummy_rogue.health;
    dura_sum += result.duration;
    resultSum = fightResultSum(resultSum, result);
    count++;
  }
  fightResultMul(1.0f/count, &resultSum);
  coutFightResult(resultSum);

  return health_sum/dura_sum;
}

}  // namespace css
