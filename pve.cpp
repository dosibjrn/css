#include "pve.h"
#include "item_picking_main.h"

namespace css
{

int PveHealingItemPicking(int argc, char** argv, bool full_buffs)
{
  auto c = BaseLvl60DiscHolyPvpHealing();
  if (full_buffs) {
    AddFullBuffs(&c);
  }
  auto value_choice = ItemPicker::ValueChoice::pve_healing;
  return ItemPickingMain(argc, argv, value_choice, c);
}

}  // namespace css
