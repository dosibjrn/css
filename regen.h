#pragma once

namespace css
{

struct Regen {
  Regen() {}
  Regen(int casts_in, int ticks_in, int ticks_oom_in) : casts(casts_in), ticks(ticks_in), ticks_oom(ticks_oom_in) {}
  int casts = 1;
  int ticks = 0;
  int ticks_oom = 0;
};

}  // namespace css
