#include "item_picker.h"

#include <string>

#include "dps.h"
#include "stats.h"

namespace css
{

namespace
{
float value(const PriestCharacter &c)
{
  // Similar to what was done prev: dps*dps*ehp*emana
  float dps = ShadowDps(c);
  Stats s(c);
  float duration = 100.0f;  // s
  float fsr_frac = 2.0f/3.0f;
  float emana = s.getEffectiveMana(duration, fsr_frac);
  float ehp = s.getEffectiveHp();
  return dps*dps*emana*ehp;
}
}

Item ItemPicker::PickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot)
{
  // create char without item
  // for each item
  // set char to state without item
  // add effect of new item
  // calculate objective function value
}

}  // namespace css
