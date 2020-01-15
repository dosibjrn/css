#include <vector>
#include <string>

namespace css
{

std::vector<std::string> SplitCsvLine(const std::string& line);
std::string RemoveComments(const std::string& line);

}  // namespace css
