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

struct LogResult
{
  float heal_sum = 0.0f;
  float in_combat_sum = 0.0f;
  std::map<std::string, int64_t> spell_casts;
  std::map<std::string, float> spell_heal_sums;
  std::map<std::string, Spell> spell_id_to_spell;
};

void ParseBased(const std::string& log_fn);

LogsType GetLogs(const std::string& log_fn);

LogResult HpsForLogs(const PriestCharacter& c, float oh_limit, float time_left_mul, const LogsType& logs);

std::pair<float, float> FindBestOhLimitAndTimeLeftMul(const PriestCharacter& c, const LogsType& logs);


}  // namespace css
