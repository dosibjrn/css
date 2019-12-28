#include <cstdlib>
#include <iostream>

#include "leveling.h"
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
      return css::PveHealingItemPicking(argc, argv);
    default:
      std::cout << "Usage: " << argv[0] << "[mode] [arg for mode]" << std::endl;
      std::cout << "Modes: " << std::endl;
      std::cout << "       0 := leveling stats, second arg for delay between mobs" << std::endl;
      std::cout << "       1 := pvp stats, trying to find linear weights" << std::endl;
      std::cout << "       2 := pvp item picking, second arg for item table file name" << std::endl;
      std::cout << "       3 := pvp healing item picking, second arg for item table file name" << std::endl;
      std::cout << "       4 := healing item picking, second arg for item table file name. 2, 4 and 6 minute fights." << std::endl;
      std::cout << "            third arg for banned, fourth arg for locked" << std::endl;
      break;
  }
  return 0;
}

// ---------- TODO -----------
// * more ranks for spells: heal and greater heal?
//   - or better: adjustability to ranks as well as part of optimizer.
//   - maybe a tad more general optimizer with limits for this?
// * refactoring: e.g. item_table -> m_item_table
//   - item_picker is a huuuuge class: could separate a ton of functionality to another class. long funcs as well.
// * unit tests: could still have a ton of bugs
// * refactoring of spell counts and ranks: hps.cpp & .hpp functionality under class?
// * all upgrade couting needs refactoring and seems a bit fishy every now and then: ring values differ notably
