#include "csv.h"

namespace css
{

std::string RemoveComments(const std::string& line) {
  char d = '#';
  int e = static_cast<int>(line.find(d));
  return line.substr(0, e);
}

std::vector<std::string> SplitCsvLine(const std::string& line)
{
  char d = ',';
  int s = 0;
  int e = 0;
  std::vector<std::string> splitted;
  while (e >= 0) {
    e = static_cast<int>(line.find(d, s));
    splitted.push_back(line.substr(s, e - s));
    s = e + 1;
    // std::cout << "splitted: |" << splitted.back() << "|, s:" << s << ", e: " << e << std::endl;
  }
  return splitted;
}

}  // namespace css
