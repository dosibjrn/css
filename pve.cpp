#include "pve.h"
#include "item_picking_main.h"

namespace css
{

int PveHealingItemPicking(int argc, char** argv)
{
  auto c = BaseLvl60DiscHolyPvpHealing();
  auto value_choice = ItemPicker::ValueChoice::pve_healing;
  return ItemPickingMain(argc, argv, value_choice, c);
}

}  // namespace css
