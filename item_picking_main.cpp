#include "item_picking_main.h"

#include <iostream>
#include <fstream>

namespace css
{

int ItemPickingMain(int argc, char** argv, ItemPicker::ValueChoice value_choice, const PriestCharacter& c)
{
  if (argc < 3) {
    std::cout << "Please give file name for PvpHealingItemPicking." << std::endl;
    return -1;
  }
  std::string item_table_fn = argv[2];
  ItemPicker ip(c, item_table_fn, value_choice);
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

  {
    bool got_something = false;
    std::string fn = "start_with.txt";
    std::cout << "Reading tmp locked from: " << fn << std::endl;
    std::ifstream is(fn.c_str());
    std::string line;
    while(std::getline(is, line)) {
      std::cout << "Locking: " << line << std::endl;
      ip.AddLocked(line);
      ip.AddWhitelisted(line);
      got_something = true;
    }
    if (got_something) {
      ItemTable item_table(item_table_fn);
      int static_for_all_slots = 0;
      int iters_without_new_best = 0;
      std::cout << "Optimizing start items..." << std::endl;
      bool disable_bans = false;
      int max_iters = 5;
      for (int iter = 0; iter < max_iters; ++iter) {
        if (iter > max_iters/2) {
          disable_bans = true;
        }
        ip.PickBestForSlots(item_table, disable_bans, iter, max_iters, //
                            &static_for_all_slots, &iters_without_new_best);
        std::cout << "*";
        std::cout.flush();
      }
      std::cout << std::endl;
      ip.ClearLocked();
      std::cout << "Read starting items and cleared locks." << std::endl;
      ip.CoutBestItems();
      std::cout << " --- And counts for start items ---" << std::endl;
      ip.CoutBestCounts();
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

  if (argc > 5) {
    std::cout << "Setting tag name to: " << argv[5] << std::endl;
    ip.setTagName(argv[5]);
  }
  ip.Calculate();
  ip.FinalCouts();
  return 0;

}

}  // namespace css

