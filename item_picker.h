#pragma once

#include "item.h"
#include "priest_character.h"

namespace css
{

class ItemPicker {
 public:
  ItemPicker();

  // Maybe just internally loop through an objective function
  void setIntWeight(float f) { int_weight_ = f; }
  void setSpiritWeight(float f) { spirit_weight_ = f; }
  void setStaminaWeight(float f) { stamina_weight_ = f; }
  void setMp5Weight(float f) { mp5_weight_ = f; }
  void setSpWeight(float f) { sp_weight_ = f; }
  void setSpShadowWeight(float f) { sp_shadow_weight_ = f; }

  Item PickBest(const PriestCharacter& c, const Item& current_item, std::vector<Item>& items_for_slot);

 private:
  float int_weight_;
  float spirit_weight_;
  float stamina_weight_;
  float mp5_weight_;
  float sp_weight_;
  float sp_shadow_weight_;
};

}  // namespace css

// All in all could be: one slot at a time, pick best for slot. When no slot changed, we're good.
