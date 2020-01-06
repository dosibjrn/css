#include "pve.h"

#include <cstring>
#include <iostream>

#include "item_picking_main.h"

namespace css
{

int PveHealingItemPicking(int argc, char** argv, bool full_buffs)
{
  auto c = BaseLvl60DiscHolyPvpHealing();
  float freq = 1.0f;
  // Horrible hack, TODO: actual command line argument parsing
  bool seen = false;
  for (int i = 0; i < argc; ++i) {
    if (seen) {
      argv[i - 2] = argv[i];
    }
    if (strcmp(argv[i], "-f") == 0) {
      if (i + 1 < argc) {
        freq = atof(argv[i + 1]);
        i++;
        seen = true;
      }
    }
  }
  if (seen) {
    argc -= 2;
  }

  for (int i = 0; i < argc; ++i) {
    std::cout << argv[i] << " ";
  }
  std::cout << std::endl;

  if (full_buffs) {
    AddFullBuffs(freq, &c);
  }
  auto value_choice = ItemPicker::ValueChoice::pve_healing;
  return ItemPickingMain(argc, argv, value_choice, c);
}

}  // namespace css
