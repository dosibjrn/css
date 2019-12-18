#include <cstdlib>
#include <iostream>

#include "leveling.h"
#include "pvp.h"
#include "pve.h"

int main(int argc, char** argv)
{
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


// TODO: add armor and resistances to database 
// TODO: more ranks for spells: heal and greater heal?
