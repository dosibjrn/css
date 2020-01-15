#include "assumptions.h"

#include <fstream>
#include <map>

namespace css
{

namespace global
{

Assumptions assumptions;

}  // namespace global

void ReadAssumptions(const std::string& fn)
{
  std::map<std::string, std::string> lines;
  std::ifstream is(fn.c_str());
}

}  // namespace css

