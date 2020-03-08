#pragma once

#include <cstdint>
#include <string>

namespace css
{

struct LogEntry {
  float hp_diff;
  int64_t time;  // in milliseconds since 2000-01-01
  std::string player;
};

bool LineToLogEntryIfAny(const std::string &line, LogEntry* e);

}  // namespace css
