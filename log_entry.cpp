#include "log_entry.h"

#define _CRT_SECURE_NO_WARNINGS
#include <time.h>

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <locale>

#include "csv.h"

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

namespace css
{

namespace {

enum Type {
  HEAL = 0,
  DMG = 1,
  SPELL_DMG = 2,
  UNIT_DIED = 3,
  BAD = 4
};


int64_t GetTime(int64_t month, int64_t day, int64_t hour, int64_t minute, int64_t s, int64_t ms)
{
  // First seconds since 1st jan
  struct tm  tm;
  time_t rawtime;
  time(&rawtime);
  tm = *localtime ( &rawtime );
  tm.tm_year = 0;
  tm.tm_mon = static_cast<int>(month - 1);
  tm.tm_mday = static_cast<int>(day);
  tm.tm_hour = static_cast<int>(hour);
  tm.tm_min = static_cast<int>(minute);
  tm.tm_sec = static_cast<int>(s);
  mktime(&tm);
  int64_t stamp = ((((tm.tm_yday*24) + hour)*60 + minute)*60 + s)*1000 + ms;
  return stamp;
}

void IncrementCell(int increment, const std::string& line, size_t* start, size_t* end)
{
  for (int i = 0; i < increment; ++i) {
    *start = *end;
    *end = line.find(',', *start + 1);
  }
}

}  // namespace

bool LineToLogEntryIfAny(const std::string& line, LogEntry* e) {
  // Example entry A
  // 2/26 21:48:51.188  SPELL_HEAL,Player-4476-004E8DD3,"Paisti-Gehennas",0x511,0x0,Player-4476-0241EF6A,"Imajoke-Gehennas",0x10514,0x0,10917,"Flash Heal",0x2,Player-4476-0241EF6A,0000000000000000,100,100,0,0,0,-1,0,0,0,-7623.41,-1096.02,0,0.9683,68,1342,1342,276,0,nil
  // Example entry B
  // 2/26 21:49:12.929  SWING_DAMAGE,Creature-0-4458-469-5988-12017-000056C441,"Broodlord Lashlayer",0xa48,0x0,Player-4476-013D02E7,"Rawrmew-Gehennas",0x10514,0x0, Creature-0-4458-469-5988-12017-000056C441,0000000000000000,84,100,0 ,0,0,-1,0,0,0,-7612.08,-1096.62,0,5.4551,63,1726,3226,-1,1,0,0,0,nil,nil,1
  // 2/26 21:48:36.477  SPELL_PERIODIC_DAMAGE,Creature-0-4458-469-5988-12468-000456C441,"Death Talon Hatcher",0xa48,0x0,Player-4476-00FC8B3C,"Ghan-Gehennas",0x514,0x0, 22442,"Growing Flames",0x4,Player-4476-00FC8B3C, 0000000000000000,58,100,0,0,0,-1,0,0,0,-7621.67,-1098.88,0,6.2650,68,200,200,-1,4,0,0,0,nil,nil,nil

  // For periodic spell damage, damage seems to 2nd cell after the heading, which is the float in [0, 2pi] range
  // That is 29th cell

  // Start by checking type: if not interesting, we can just skip line


  size_t start = 0;
  size_t end = line.find(',', start);
  if (end == std::string::npos) {
    return false;
  }

  std::string cell = line.substr(start, end + 6);
  
  Type type = Type::BAD;

  size_t cell_size = cell.size();

  if (cell_size > 10 && cell.substr(cell_size - 10, 10) == "HEAL,Playe") {
    type = Type::HEAL;
  } else if (cell_size > 12 && cell.substr(cell_size - 12, 12) == "DAMAGE,Creat") {
    type = Type::DMG; 
    if (cell.find("SPELL") != std::string::npos) {
      type = Type::SPELL_DMG;
    }
  } else if ( cell_size > 15 && (cell.substr(cell_size - 15, 15) == "UNIT_DIED,00000")
             && (line.find("Player-") != std::string::npos) ) {
    type = Type::UNIT_DIED;
  } else {
    // type = Type::BAD;
    return false;
  }

  // 2/26 21:48:51.188
  start = 0;
  end = line.find('/', start);
  int64_t month = atoi(cell.substr(start, end - start).c_str());

  start = end + 1;
  end = cell.find(' ', start);
  int64_t day = atoi(cell.substr(start, end - start).c_str()) ;

  start = end;
  end = cell.find(':', start);
  int64_t hour = atoi(cell.substr(start, end - start).c_str());

  start = end + 1;
  end = cell.find(':', start);
  int64_t minute = atoi(cell.substr(start, end - start).c_str());

  start = end + 1;
  end = cell.find('.', start);
  int64_t s = atoi(cell.substr(start, end - start).c_str());

  start = end + 1;
  end = cell.find(' ', start);
  int64_t ms = atoi(cell.substr(start, end - start).c_str());

  // date entry -> time since 2000-01-01
  e->time = GetTime(month, day, hour, minute, s, ms);

  end = 0;
  // Get source player name: 3th cell in csv
  IncrementCell(3, line, &start, &end);
  e->source = line.substr(start + 2, end - start - 3);  // skip both commas, both quotation marks

  // Get target player name: 7th cell in csv -> 4 more
  IncrementCell(4, line, &start, &end);
  e->player = line.substr(start + 2, end - start - 3);

  
  // 2/26 21:48:51.188  SPELL_HEAL,Player-4476-004E8DD3,"Paisti-Gehennas",0x511,0x0,Player-4476-0241EF6A,"Imajoke-Gehennas",0x10514,0x0,10917,"Flash Heal",0x2,Player-4476-0241EF6A,0000000000000000,100,100,0,0,0,-1,0,0,0,-7623.41,-1096.02,0,0.9683,68,1342,1342,276,0,nil
  if (type == Type::HEAL) {
    // Number of commas before the heal entry: 29 -> 30th cell
    IncrementCell(23, line, &start, &end);
    int healing = atoi(line.substr(start + 1, start - end - 2).c_str());

    IncrementCell(1, line, &start, &end);
    // int overhealing = atoi(line.substr(start + 1, start - end - 2).c_str());
    int total_healing = healing;  // we handle overhealing separately // - overhealing;
    if (total_healing <= 0) {
      return false;
    }
    e->hp_diff = static_cast<float>(total_healing);
  } else if (type == Type::DMG) {
    // 2/26 21:49:12.929  SWING_DAMAGE,Creature-0-4458-469-5988-12017-000056C441,"Broodlord Lashlayer",0xa48,0x0,Player-4476-013D02E7,"Rawrmew-Gehennas",0x10514,0x0,Creature-0-4458-469-5988-12017-000056C441,0000000000000000,84,100,0,0,0,-1,0,0,0,-7612.08,-1096.62,0,5.4551,63,1726,3226,-1,1,0,0,0,nil,nil,1
    // Note: this indexing starts from 0
    // 25 onwards: 1726,3226,-1,1,0,0,0,
    // 25  amount  Mitigated damage amount
    IncrementCell(19, line, &start, &end);
    int mitigated = atoi(line.substr(start + 1, start - end - 2).c_str());

    // 26  rawAmount  Raw damage amount (pre-armor, etc.)
    // IncrementCell(1, line, &start, &end);
    // int raw_amount = atoi(line.substr(start + 1, start - end - 2).c_str());

    // 27  overkill  Overkill amount
    // 28  school  Spell school

    // 29  resisted  Resisted amount
    // IncrementCell(3, line, &start, &end);
    // int resisted = atoi(line.substr(start + 1, start - end - 2).c_str());

    // 30  blocked  Blocked amount
    // IncrementCell(1, line, &start, &end);
    // int blocked = atoi(line.substr(start + 1, start - end - 2).c_str());

    // 31  absorbed  Absorbed amount
    // IncrementCell(1, line, &start, &end);
    // int absorbed = atoi(line.substr(start + 1, start - end - 2).c_str());

    // e->hp_diff = -1.0f * (raw_amount - mitigated - resisted - blocked - absorbed);
    // int total_damage = mitigated;
    int total_damage = mitigated;
    if (total_damage <= 0) {
      return false;
    }
    e->hp_diff = -1.0f * total_damage;
  } else if (type == Type::SPELL_DMG) {
    // 7->29
    IncrementCell(22, line, &start, &end);
    int total_damage = atoi(line.substr(start + 1, start - end - 2).c_str());
    if (total_damage <= 0) {
      return false;
    }
    e->hp_diff = -1.0f * total_damage;
  } else if (type == Type::UNIT_DIED) {
    // From healing point of view, dead is as good as healed to full, funnily enough

    // std::cout << "(unit died) line: " << line << std::endl;
    e->source = "DEATH";
    e->hp_diff = 1e5f; // + 100 000 should do it in classic context
  } else {
    return false;
  }
  // std::cout << "line: " << line << std::endl;
  // std::cout << "time: " << e->time << ", player: " << e->player << ", hp_diff: " << e->hp_diff << std::endl;
  return true;  // Should not really get here
}

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


bool WclParsedLineToLogEntryIfAny(const std::string &line, LogEntry* e)
{
  constexpr bool debug = false;
  auto entries = SplitCsvLine(line);
  if (debug) {
    std::cout << "entries: " << entries.size();
    for (auto entry : entries) {
      std::cout << " " << entry;
    }
    std::cout << std::endl;
  }
  if (entries.size() < 4) {
    return false;
  } else {
    // time without month and day, source, target, hp diff
    std::string cell = entries[0];
    size_t start = 0;
    size_t end = cell.find(':', start);
    int64_t hour = atoi(cell.substr(start, end - start).c_str());

    start = end + 1;
    end = cell.find(':', start);
    int64_t minute = atoi(cell.substr(start, end - start).c_str());

    start = end + 1;
    double s_frac = atof(cell.substr(start).c_str());
    int64_t s = static_cast<int>(s_frac);
    int64_t ms = static_cast<int64_t>((s_frac - s) * 1000.0);

    int64_t month = 0;
    int64_t day = 0;
    e->time = GetTime(month, day, hour, minute, s, ms);
    e->source = trim(entries[1]);
    e->player = trim(entries[2]);
    e->hp_diff = static_cast<float>(atof(entries[3].c_str()));
    if (debug) {
      std::cout << "ts: " << e->time << ", s: " << e->source << ", p: " << e->player << ", hpd: " << e->hp_diff << std::endl;
    }
    return true;
  }
}

// Stuff to (maybe) add:
// UNIT_DIED,[5 cells],Player -> DÖD
// SPELL_RESURRECT,Plauyer -> rezd
// ENCOUNTER_END == combat ended
//    Or when player damage drops / is out for 10 seconds?
// ENCOUNTER_START == combat start
// or when a player takes damage? good enough?

}  // namespace css
