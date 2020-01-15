#include "pve.h"

#include <cstring>
#include <iostream>

#include "item_picking_main.h"
#include "assumptions.h"

namespace css
{

std::pair<bool, float> ParseFloat(const std::string& key, int* argc, char **argv)
{
  float val = 0.0f;
  bool seen = false;
  for (int i = 0; i < *argc; ++i) {
    if (seen) {
      argv[i - 2] = argv[i];
    }
    if (strcmp(argv[i], key.c_str()) == 0) {
      if (i + 1 < *argc) {
        val = atof(argv[i + 1]);
        i++;
        seen = true;
      }
    }
  }
  if (seen) {
    *argc -= 2;
  }

  if (seen) {
    std::cout << "Found: " << key << ", val: " << val << std::endl;
  }

  for (int i = 0; i < *argc; ++i) {
    std::cout << argv[i] << " ";
  }
  std::cout << std::endl;
  return {seen, val};
}

std::pair<bool, std::string> ParseString(const std::string& key, int* argc, char **argv)
{
  std::string val = "";
  bool seen = false;
  for (int i = 0; i < *argc; ++i) {
    if (seen) {
      argv[i - 2] = argv[i];
    }
    if (strcmp(argv[i], key.c_str()) == 0) {
      if (i + 1 < *argc) {
        val = argv[i + 1];
        i++;
        seen = true;
      }
    }
  }
  if (seen) {
    *argc -= 2;
  }

  if (seen) {
    std::cout << "Found: " << key << ", val: " << val << std::endl;
  }

  for (int i = 0; i < *argc; ++i) {
    std::cout << argv[i] << " ";
  }
  std::cout << std::endl;
  return {seen, val};
}

int PveHealingItemPicking(int argc, char** argv, bool full_buffs)
{
  auto res_string = ParseString("-a", &argc, argv);
  if (res_string.first) {
    ReadAssumptions(res_string.second);  
  }
  if (full_buffs) {
    global::assumptions.buff_fraction = 1.0f;
  }
  auto c = BaseLvl60DiscHolyPvpHealing();
  if (global::assumptions.enchantments) {
    AddEnchants(&c);
  }

  // pve healing specific args
  auto res = ParseFloat("-b", &argc, argv);
  if (res.first) {
    global::assumptions.buff_fraction = res.second;
  }
  res = ParseFloat("-r", &argc, argv);
  global::assumptions.full_regen_limit = 1.0f;
  if (res.first) {
    global::assumptions.full_regen_limit = res.second;
  }

  // apply buffs if any
  ApplyBuffs(&c);
  auto value_choice = ItemPicker::ValueChoice::pve_healing;
  return ItemPickingMain(argc, argv, value_choice, c);
}

}  // namespace css
