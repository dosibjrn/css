#include <iostream>
#include <cmath>
#include <vector>

#include "combat.h"
#include "priest_character.h"
#include "stats.h"

int LevelingStats(int argc, char** argv)
{
  if (argc > 2) {
    css::globals::setTimeToPull(std::atof(argv[2]));
    std::cout << "Setting time to pull: " << css::globals::getTimeToPull() << std::endl;
  }
  std::vector<css::FightResult> results(6);
  css::Mob mob;
  std::vector<float> mob_healths{3000, 3200, 3400, 3600};
  std::vector<float> multis{1.1f, 1.2f, 1.3f};
  int count = mob_healths.size()*multis.size();
  for (auto mob_health : mob_healths) {
    // for (auto medi : meditations) {
      for (auto multi : multis) {
        mob.health = mob_health;
        auto paisti = css::PaistiLvl53();
        // paisti.sp += 200;
        // paisti.spirit += 50;
        // paisti.talents.meditation = medi;
        // paisti.wand.dps = 40;
        auto result = css::RelativeValues(paisti, multi, mob);
        int i = 0;
        for (auto res : result) {
          fightResultMul(1.0f/count, &result[i]);
          results[i] = fightResultSum(result[i], results[i]);
          i++;
        }
      }
    // }
  }
  std::cout << std::endl;
  std::cout << "Average fight results:" << std::endl;
  std::cout << "***************" << std::endl;
  std::cout << "Int" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[1]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Spirit" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[2]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Sp" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[3]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Wand dps" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[4]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Mp5" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[5]);
  std::cout << std::endl;

  std::cout << "***************" << std::endl;
  std::cout << "Base line" << std::endl;
  std::cout << "***************" << std::endl;
  coutFightResult(results[0]);
  std::cout << std::endl;

  std::cout << "Average relative values based on mobs_per_min:" << std::endl;
  std::cout << "Base Int Spi Sp WandDps Mp5" << std::endl;
  float ref = results[2].diff;
  for (auto res : results) {
    std::cout << res.diff/ref << " ";
  }
  std::cout << std::endl;

  css::Stats s(css::PaistiLvl51());
  std::cout << "Baseline mana stats: " << std::endl;
  std::cout << "Max mana: " << s.getMaxMana() << std::endl;
  std::cout << "In fsr tick: " << s.getManaRegenTickUnderFsr() << std::endl;
  std::cout << "No fsr tick: " << s.getManaRegenTickOutOfFsr() << std::endl;
  std::cout << "In fsr spirit tap tick: " << s.getManaRegenTickSpiritTapUnderFsr() << std::endl;
  std::cout << "No fsr spirit tap tick: " << s.getManaRegenTickSpiritTapOutOfFsr() << std::endl;

  return 0;
}


// TODO: Mental agility to instant spells
