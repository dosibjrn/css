#pragma once

#include "heal_target.h"

class Raid {
  public:
   void CastHealTo(const std::string& name, const Heal& , float overhealing_cutoff);
   void DamageTo(const std::string& name, float amount);

   std::string PickTarget(std::function<float(const HealTarget&)> &score_func);

   float Time() const { return m_time; }

   float 

  private:
   std::map<std::string, int> m_name_to_ix;
   std::vector<HealTarget> m_raid;
   
};

// 2/26 21:49:12.929  SWING_DAMAGE,Creature-0-4458-469-5988-12017-000056C441,"Broodlord Lashlayer",0xa48,0x0,Player-4476-013D02E7,"Rawrmew-Gehennas",0x10514,0x0,Creature-0-4458-469-5988-12017-000056C441,0000000000000000,84,100,0,0,0,-1,0,0,0,-7612.08,-1096.62,0,5.4551,63,1726,3226,-1,1,0,0,0,nil,nil,1
// Based on github JanKoppe/wow-log-parser: parse.js
// 2/26 21:49:12.929 : month, day, h, min, s, ms
// SWING_DAMAGE : event
// Creature-0-4458-469-5988-12017-000056C441 : source guid
// "Broodlord Lashlayer" : source name
// 0xa48 : source flags
// 0x0 : source raidflags
// Player-4476-013D02E7: target guid  // NOTE: not all events have targets.
// "Rawrmew-Gehennas" : target name
// 0x10514 : target flags
// 0x0 : target raidflags

// Regarding any following, we have (from: https://github.com/magey/classic-warrior/wiki/Combat-log-format) :
// 0  eventName  SWING_DAMAGE, SPELL_DAMAGE, etc.
// 1  sourceGUID  Source unique ID
// 2  sourceName  Source name
// 3  sourceFlags  Source unit flags, see https://wow.gamepedia.com/UnitFlag
// 4  sourceRaidFlags  Source raid flags, see https://wow.gamepedia.com/RaidFlag
// 5  destGUID  Target unique ID
// 6  destName  Target name
// 7  destFlags  Target unit flags
// 8  destRaidFlags  Target raid flags
// Above were covered then we have
// 9  infoGUID*  Unique ID of the unit the extra information is provided for
// 10  ownerGUID*  Unique ID of owner (in case of pet, etc.)
// 11  currentHP *  Unit current HP
// 12  maxHP*  Unit max HP
// 13  attackPower*  Unit attack power, values seem off for classic
// 14  spellPower*  Unit spell power, values seem off for classic
// 15  armor*  Unit armor
// 16  powerType*  Power type, see https://wow.gamepedia.com/Enum_Unit.PowerType
// 17  currentPower*  Unit current power
// 18  maxPower*  Unit max power
// 19  powerCost*  Not relevant for normal melee swings
// 20  positionX*  Unit X position
// 21  positionY*  Unit Y position
// 22  uiMapID  Map ID, see https://wow.gamepedia.com/UiMapID
// 23  facing*  Unit facing direction in the [0, 2π] range
// 24  level*  Seems to only make sense when the extra info unit is an NPC
// 25  amount  Mitigated damage amount
// 26  rawAmount  Raw damage amount (pre-armor, etc.)
// 27  overkill  Overkill amount
// 28  school  Spell school
// 29  resisted  Resisted amount
// 30  blocked  Blocked amount
// 31  absorbed  Absorbed amount
// 32  critical  1 if critical hit, nil otherwise
// 33  glancing  1 if glancing blow, nil otherwise
// 34  crushing  1 if crushing blow, nil otherwise

// infoGUID: 84
// ownerGUID: 100
// currentHP: 0
// maxHP: 0
// attackPower: 0 
// spellPower: -1
// armor: 0
// powerType: 0
// currentPower: 0
// maxPower: 0
// powerCost: 0
// positionX: -7612.08
// positiionY: -1096.62
// uiMapID: 0
// facing: 5.4551
// level: 63
// amount (mitigated damage): 1726  // And these we care about: health change == raw-mitigated-resisted-blocked-absorbed or sth
// Raw damage amount: 3226 
// Overkill: -1
// Spell school: 1
// resisted: 0
// blocked: 0
// absorbed: 0
// critical: nil
// glancing: nil
// crushing: 1

// For any healing stuff, from the same json github thing we have the info that instead of:
// amount, overkill, school, resisted - we should have:
// amount, overheal, absorbed, critical

// Let's see a healing entry and start on the amount
// 2/26 21:48:51.188  SPELL_HEAL,Player-4476-004E8DD3,"Paisti-Gehennas",0x511,0x0,Player-4476-0241EF6A,"Imajoke-Gehennas",0x10514,0x0,10917,"Flash Heal",0x2,Player-4476-0241EF6A,0000000000000000,100,100,0,0,0,-1,0,0,0,-7623.41,-1096.02,0,0.9683,68,1342,1342,276,0,nil
// Easy to hop to stuff after level (which i doubt that 69 is):
//,1277,1277,549,0,nil
// Ok, that flash heal was not full overheal, nowai.Let's rechec that from wowlogs. 
// Imajoke is not much of heal target: i healed him once. 1066 real healing 276 overhealing.
// So: we have
// Something healing1: 1342
// Amount of heal: 1342
// Overheal: 276:
// Absorbed: 0
// Critical: 0
// And of these we really care about the Amount of heal and overheal

// Now for these entries we REALLY at least first care about:
// time
// target
// damage done (couple of entries due to all kinds of resists)
// healing done (2 entries: heal and overheal)
// damage and healing can actually be treated the same: + or -






// If second entry starts with Creature, it's damage from creature to player
//   SWING_DAMAGE,Creature
//   SWING_DAMAGE_LANDED,Creature
//     Damage could be the entry 241,340 in the above: 241 went thru, 340 without mitigation?
//   SPELL_DAMAGE,Creature
// Some research from existing parsers: damage order should be: amount, overkill, school, resisted
// Healing should be amount, overheal, absorbed, critical

// Actual heals causing shifts to hp
//   SPELL_HEAL,Player
//   SPELL_PERIODIC_HEAL,Player
// Let us assume that the damage healed is the 850,850,0,0,nil second of those entries...
//   Overheal could be the third (0 here), but i gues we can cope with that via the deficit

// ENCOUNTER_START = boss start
// ENCOUNTER_END = boss end

// COMBATANT_INFO contains max hp. We can start with assuming that everyone is full == deficit 0. With UNIT_DIED == DEAD. No names here, just ids

// For estimating incoming heals -> SPELL_CAST_START -> id -> average healing incoming. (we do not know the amount at that time)

// First, could start without the incoming stuff, just how healt shifts through heals and damage in
