#include <iostream>
#include <fstream>

#include "priest_character.h"
#include "item_picker.h"

namespace css
{

int PveHealingItemPicking(int argc, char** argv)
{
  PriestCharacter c = BaseLvl60UdShadow();
  // PriestCharacter c = BaseLvl60UdPvpHealing();
  if (argc < 3) {
    std::cout << "Please give file name for PvpHealingItemPicking." << std::endl;
    return -1;
  }
  std::string fn = argv[2];
  ItemPicker ip(c, fn, ItemPicker::ValueChoice::pve_healing);
  if (argc > 3) {
    std::string fn = argv[3];
    std::cout << "Reading banned from: " << fn << std::endl;
    std::ifstream is(fn.c_str());
    std::string line;
    while(std::getline(is, line)) {
      std::cout << "Banning: " << line << std::endl;
      ip.AddBanned(line);
    }
  }
  if (argc > 4) {
    std::string fn = argv[4];
    std::cout << "Reading locked from: " << fn << std::endl;
    std::ifstream is(fn.c_str());
    std::string line;
    while(std::getline(is, line)) {
      std::cout << "Locking: " << line << std::endl;
      ip.AddLocked(line);
    }
  }
  ip.Calculate();
    
  ip.CoutBestItems();
  std::cout << "------------------" << std::endl;
  std::cout << "Best value: " << ip.getBestValue() << std::endl;
  std::cout << "------------------" << std::endl;
  std::cout << "Char stats: " << std::endl;
  ip.CoutCharacterStats();
  std::cout << "------------------" << std::endl;
  std::cout << "Best counts:" << std::endl;
  ip.CoutBestCounts();
  std::cout << "------------------" << std::endl;
 
  return 0;
}

}  // namespace css
