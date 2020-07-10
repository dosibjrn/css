#pragma once

#include <string>
#include <map>
#include <vector> // WOW a vector

#include "log_entry.h"
#include "priest_character.h"
#include "spell.h"

namespace css
{

typedef std::vector<std::vector<LogEntry>> LogsType;

// holds entries which control spell choices in log based healing (the tad more complex iteration, WIP)
struct LogBasedParameters
{
  float heal_deficit_mul;
  float heal_dtps_mul;

  float hot_deficit_mul;
  float hot_dtps_mul;

  float shield_deficit_mul;
  float shield_dtps_mul;

  float heal_mul;
  float hot_mul;
  float shield_mul;

  float time_left_mul;
  float oh_limit;

  // So the idea here is that deficit_mul*deficit + dtps_mul*dtps get us value per category.
  //   spell rank is picked by this value and oh_limit (aiming overhealing of half the oh_limit)
  // This value is multiplied by the category multiplier. Highest value of this product = the pick for cast.

  // The only "mana saving" approach here is again the slow spells and this is controlled by the time_left_mul

  // Should probably add incoming heals here, ugh.
};

struct LogResult
{
  float heal_sum = 0.0f;
  float in_combat_sum = 0.0f;
  std::map<std::string, int64_t> spell_casts;
  std::map<std::string, float> spell_heal_sums;
  std::map<std::string, Spell> spell_id_to_spell;
  std::map<std::string, float> player_heal_sums;
  int n_combats = 0;
  float mana_at_start_sum = 0.0f;
  float mana_at_end_sum = 0.0f;
  float deficit_time_sum = 0.0f;  // discrete integral of deficit * time: sum of deficit at each time step * time step 
};

void ParseBased(const std::string& log_fn);

LogsType GetLogs(const std::string& log_fn, int64_t* skipped_player_entries = nullptr);

LogResult HpsForLogs(const PriestCharacter& c, float oh_limit, float time_left_mul, const LogsType& logs);

std::pair<float, float> FindBestOhLimitAndTimeLeftMul(const PriestCharacter& c, const LogsType& logs);


}  // namespace css
