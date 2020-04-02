#include "parse_based.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "log_entry.h"

namespace css
{

void ParseBased(const std::string& log_fn)
{
  std::ifstream is(log_fn.c_str());
  std::string line;
  float diff_sum = 0.0f;
  float heal_sum = 0.0f;
  float damage_sum = 0.0f;
  int64_t ms_start = 0;
  int64_t ms_end = 0;
  
  std::vector<LogEntry> log;
  while (std::getline(is, line)) {
    LogEntry e;
    if (LineToLogEntryIfAny(line, &e)) {
      // std::cout << "ignored line: " << line << std::endl;
      diff_sum += e.hp_diff;
      if (ms_start == 0) {
        ms_start = e.time;
      }
      if (e.hp_diff < 0) {
        damage_sum -= e.hp_diff;
      } else {
        heal_sum += e.hp_diff;
      }
      ms_end = e.time;
      log.push_back(e);
    }
  }
  float time_s = (ms_end - ms_start)*1e-3f;
  is.close();
  std::cout << "diff_sum: " << diff_sum << std::endl;
  std::cout << "/40: " << diff_sum/40.0f << std::endl;
  std::cout << "heal_sum: " << heal_sum << " -> hps: " << heal_sum/time_s << std::endl;
  std::cout << "damage_sum: " << damage_sum << " -> dtps: " << damage_sum/time_s << std::endl;
  std::cout << "time_s: " << time_s << std::endl;
}

}  // namespace css
