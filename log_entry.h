#pragma once

#include <cstdint>
#include <string>

namespace css
{

struct LogEntry {
  float hp_diff;
  int64_t time;  // in milliseconds since 00:00 1st january
  std::string player;
};

bool LineToLogEntryIfAny(const std::string &line, LogEntry* e);

}  // namespace css
