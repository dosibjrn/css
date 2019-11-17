#pragma once

namespace css
{

Spell FlashHeal(const PriestCharacter& c, int rank);
Spell Heal(const PriestCharacter& c, int rank);
Spell GreaterHeal(const PriestCharacter& c, int rank);
Spell PrayerOfHealing(const PriestCharacter& c, int rank, int targets);
Spell Renew(const PriestCharacter& c, int rank);

}  // namespace css
