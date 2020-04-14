#pragma once

#include <cstdint>
#include <string>

namespace css
{

struct LogEntry {
  float hp_diff;
  int64_t time;  // in milliseconds since 00:00 1st january
  std::string player;  // target
  std::string source;  // caster in case of heals
};

bool LineToLogEntryIfAny(const std::string &line, LogEntry* e);

}  // namespace css
