#include "parse_based.h"

#include <fstream>
#include <iostream>

#include "log_entry.h"

namespace css
{

void ParseBased(const std::string& log_fn)
{
  std::ifstream is(log_fn.c_str());
  std::string line;
  while (std::getline(is, line)) {
    LogEntry e;
    if (LineToLogEntryIfAny(line, &e)) {
      // std::cout << "ignored line: " << line << std::endl;
    }
  }
  is.close();
}

}  // namespace css
