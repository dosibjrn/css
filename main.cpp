#include <cstdlib>
#include <iostream>

#include "leveling.h"
#include "parse_based.h"
#include "pvp.h"
#include "pve.h"

int main(int argc, char** argv)
{
  std::cout << "Your call: ";
  for (int i = 0; i < argc; ++i) {
    std::cout << argv[i] << " ";
  }
  std::cout << std::endl;
  int choice = 100;
  if (argc > 1) {
    choice = atoi(argv[1]);
  }
  switch(choice) {
    case 0:
      return LevelingStats(argc, argv);
      break;
    case 1:
      return css::PvpStats(argc, argv);
      break;
    case 2:
      return css::PvpItemPicking(argc, argv);
      break;
    case 3:
      return css::PvpHealingItemPicking(argc, argv);
      break;
    case 4:
      return css::PveHealingItemPicking(argc, argv, false);
    case 5:
      return css::PveHealingItemPicking(argc, argv, true);
    case 6:
      if (argc < 3) {
        std::cout << "please give a wow log file name as the third argument" << std::endl;
        return 1;
      }
      css::ParseBased(argv[2]);
      break;
 
    default:
      std::cout << "Usage: " << argv[0] << "[mode] [arg for mode]" << std::endl;
      std::cout << "Modes: " << std::endl;
      std::cout << "       0 := leveling stats, second arg for delay between mobs" << std::endl;
      std::cout << "       1 := pvp stats, trying to find linear weights" << std::endl;
      std::cout << "       2 := pvp item picking, second arg for item table file name" << std::endl;
      std::cout << "       3 := pvp healing item picking, second arg for item table file name" << std::endl;
      std::cout << "       4 := healing item picking, second arg for item table file name. 2, 3, 4, 5 and 6 minute fights." << std::endl;
      std::cout << "            third arg for banned items / sources, fourth arg for locked items" << std::endl;
      std::cout << "            ./css 4 -r [0-1] sets maximum fraction of time spent in full regen. Default is 1.0." << std::endl;
      std::cout << "            -a sets assumptions file, which is a csv. example file in confs/example_conf.csv" << std::endl;
      std::cout << "       5 := same as 4, but with potions, runes, mana oil, mageblood and flask" << std::endl;
      std::cout << "            ./css 5 -f [0-1] sets consumable use to 0% - 100%. Default is 1.0." << std::endl;
      std::cout << "       6 := Parse based healing optimization. Work in progress." << std::endl;
      std::cout << "All items listed in start_with.txt will be used for initial optimization for options 2-5." << std::endl; 
      std::cout << "Fifth optional arg is tag name for saving pawn tag for modes 2-5." << std::endl;
      break;
  }
  return 0;
}

// ---------- TODO -----------
// * spell ranks into optimization?
// * refactoring: e.g. item_table -> m_item_table
//   - item_picker is a huuuuge class: could separate a ton of functionality to another class. long funcs as well.
// * unit tests: could still have a ton of bugs
// * refactoring of spell counts and ranks: hps.cpp & .hpp functionality under class?
// * all upgrade couting needs refactoring and seems a bit fishy every now and then: ring values differ notably
// * would probably be informative to get numbers like heal per mana, frac out of fsr in pve info?
