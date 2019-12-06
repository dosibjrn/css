#include <iostream>
#include <fstream>

#include "priest_character.h"
#include "item_picker.h"

namespace css
{

int PveHealingItemPicking(int argc, char** argv)
{
  // PriestCharacter c = BaseLvl60UdShadow();
  // PriestCharacter c = BaseLvl60UdPvpHealing();
  // PriestCharacter c = BaseLvl60HolyDiscHealing();

  // with pi
  // 2 min quick: 323.284 
  // 2-6 min quick: 231.412
  // 2 min non-quick: 347.104
  // 2 min non-quick new: 353.319
  // 2-6 min non-quick: 264.283
  // 2-6 min non-quick new: 272.15
  // 2-6 min non-quick new: 272.582
  auto c = BaseLvl60DiscHolyPvpHealing();

  // 2 min quick: 319.056
  // 2-6 min quick: 232.87
  // 2 min non-quick: 334.77
  // 2-6 min non-quick: 259.014
  // 2-6 min non-quick new: 269.774
  // auto c = BaseLvl60HolyDiscHealing();  
  if (argc < 3) {
    std::cout << "Please give file name for PvpHealingItemPicking." << std::endl;
    return -1;
  }
  std::string item_table_fn = argv[2];
  ItemPicker ip(c, item_table_fn, ItemPicker::ValueChoice::pve_healing);
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
  } else {
    bool got_something = false;
    std::string fn = "start_with.txt";
    std::cout << "Reading tmp locked from: " << fn << std::endl;
    std::ifstream is(fn.c_str());
    std::string line;
    while(std::getline(is, line)) {
      std::cout << "Locking: " << line << std::endl;
      ip.AddLocked(line);
      got_something = true;
    }
    if (got_something) {
      ItemTable item_table(item_table_fn);
      int static_for_all_slots = 0;
      int iters_without_new_best = 0;
      std::cout << "Optimizing start items..." << std::endl;
      bool disable_bans = true;
      int max_iters = 5;
      for (int iter = 0; iter < max_iters; ++iter) {
        ip.PickBestForSlots(item_table, disable_bans, iter, max_iters, //
                            &static_for_all_slots, &iters_without_new_best);
        std::cout << "*";
        std::cout.flush();
      }
      std::cout << std::endl;
      ip.ClearLocked();
      std::cout << "Read starting items and cleared locks." << std::endl;
      ip.CoutBestItems();
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
  ip.CoutCurrentValuesAlt();
  std::cout << "------------------" << std::endl;
  std::cout << "Diffs to start: " << std::endl;
  ip.CoutDiffsToStart();
 
  return 0;
}

}  // namespace css
