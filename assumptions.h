#pragma once

namespace css
{

struct Assumptions
{
  float full_regen_limit = 1.0f;
};

namespace global
{

extern Assumptions assumptions;

}  // namespace global

}  // namespace css
