#include "pve.h"
#include "item_picking_main.h"

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
  // 2-6 min non-quick new: 272.9
  auto c = BaseLvl60DiscHolyPvpHealing();

  // 2 min quick: 319.056
  // 2-6 min quick: 232.87
  // 2 min non-quick: 334.77
  // 2-6 min non-quick: 259.014
  // 2-6 min non-quick new: 269.774
  // auto c = BaseLvl60HolyDiscHealing();  
  auto value_choice = ItemPicker::ValueChoice::pve_healing;
  return ItemPickingMain(argc, argv, value_choice, c);
}

}  // namespace css
