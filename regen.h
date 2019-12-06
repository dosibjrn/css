#pragma once

namespace css
{

struct Regen {
  Regen() {}
  Regen(int ticks_in, int casts_in) : ticks(ticks_in), casts(casts_in) {}
  int ticks = 0;
  int casts = 1;
};

}  // namespace css
