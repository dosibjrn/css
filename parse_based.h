#pragma once

#include <string>
#include <vector> // WOW a vector

#include "log_entry.h"
#include "priest_character.h"

namespace css
{

typedef std::vector<std::vector<LogEntry>> LogsType;

void ParseBased(const std::string& log_fn);
float HpsForLogs(const PriestCharacter& c, float oh_limit, float time_left_mul, const LogsType& logs);
std::pair<float, float> FindBestOhLimitAndTimeLeftMul(const PriestCharacter& c, const LogsType& logs);

}  // namespace css
