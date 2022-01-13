/*!
  TinyDarius

  @file math_table.cpp
  @brief Table based mathmatics.
*/
#include "math_table.hpp"
#include <gob_math.hpp>

namespace
{
constexpr fx16 tsin(int x) { return fx16(std::sin(goblib::math::deg2rad(x*360.0f/TANGLE_DIV))); }
constexpr auto sin_table = goblib::template_helper::table::generator<TANGLE_DIV>(tsin);

constexpr fx16 tcos(int x) { return fx16(std::cos(goblib::math::deg2rad(x*360.0f/TANGLE_DIV))); }
constexpr auto cos_table = goblib::template_helper::table::generator<TANGLE_DIV>(tcos);

constexpr Tangle tatan2(std::size_t v) { return goblib::math::round(goblib::math::rad2deg(std::atan2(v/NUM_OF_HOR, v%NUM_OF_HOR)) / (360/TANGLE_DIV)); }
constexpr auto atan2_table = goblib::template_helper::table::generator<NUM_OF_HOR * NUM_OF_VER>(tatan2);
//
}

fx16 table_sin(const Tangle a) { return sin_table[a % TANGLE_DIV]; }

fx16 table_cos(const Tangle a) { return cos_table[a % TANGLE_DIV]; }

Tangle table_atan2(const std::int16_t y, const std::int16_t x)
{
    if(y == x && y == 0) { return Tangle(0); }

    auto xx = (x + goblib::math::sign(x) * SCREEN_DIV_WIDTH - 1) / SCREEN_DIV_WIDTH;
    auto yy = (y + goblib::math::sign(y) * SCREEN_DIV_HEIGHT - 1) / SCREEN_DIV_HEIGHT;;

    auto v = atan2_table[ std::abs(yy) * NUM_OF_HOR + std::abs(xx) ];

    if(xx < 0) { v = (TANGLE_DIV/2) - v; }
    if(yy < 0) { v = TANGLE_DIV - v; }

    return v;
}

