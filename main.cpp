#include <cstdlib>
#include <iostream>

#include "leveling.h"
#include "pvp.h"

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
    default:
      std::cout << "Usage: " << argv[0] << "[mode] [arg for mode]" << std::endl;
      std::cout << "Modes: " << std::endl;
      std::cout << "       0 := leveling stats, second arg for delay between mobs" << std::endl;
      std::cout << "       1 := pvp stats, trying to find linear weights" << std::endl;
      std::cout << "       2 := pvp item picking, second arg for item table file name" << std::endl;
      break;
  }
  return 0;
}
