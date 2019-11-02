#pragma once

namespace css
{
// FightResult FightMob(const PriestCharacter& c, const std::vector<Spell>& spells, const Mob& mob)
// Water BestWater(const PriestCharacter &c)
// float DrinkToFull(const Stats& stats, const Water& water, const FightResult& fr, float current_mana)
// float RegenForNext(const Stats& stats, const FightResult& fr, int ticks_for_next, float *current_mana)
// FightResult FightMobs(const PriestCharacter& c, const std::vector<Spell>& spells, const Mob& mob, int ticks_for_next)
// FightResult BestMobsPerMin(const PriestCharacter&c, const Mob& mob) {

std::vector<FightResult> RelativeValues(const PriestCharacter& c, float multiplier, const Mob& mob);

}  // namespace css
