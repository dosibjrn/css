#pragma once

#include "item.h"

namespace css
{

struct PriestCharacter;

void CoutItem(const Item& i);
void AddItem(const Item &i, PriestCharacter* c);
void RemoveItem(const Item &i, PriestCharacter* c);
void AddToItemWithMul(const Item& i, float mul, Item* o);

}  // namespace css
