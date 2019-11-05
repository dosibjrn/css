#include "leveling.h"
#include "pvp.h"

int main(int argc, char** argv)
{
  if (argc == 1) {
    return css::PvpStats(argc, argv);
  } else {
    return LevelingStats(argc, argv);
  }
  // return css::PvpStatsVsBase(argc, argv);
}
