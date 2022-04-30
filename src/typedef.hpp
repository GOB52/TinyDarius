/*!
  TinyDarius

  @file typedef.hpp
  @brief type definitions
*/
#ifndef TD_TYPEDEF_HPP
#define TD_TYPEDEF_HPP

#include <cstdint>
#include <gob_shape2d.hpp>
#include <gob_rect2d.hpp>
#include <gob_ellipse2d.hpp>
#include <gob_fixed_point_number.hpp>
#include <gob_vector2d.hpp>

using fx16 = goblib::FixedPointNumber<std::int16_t, 6>; // 1:sign, 10(0~511):int, 6[0~2^-6):fraction */
using VecFx16 = goblib::shape2d::Vector2<fx16>;

#if 0
// using int
using Pos2 = goblib::shape2d::Point<std::int16_t>;
using Rect2 = goblib::shape2d::Rectangle<std::int16_t>;
#else
// using fixed point number
using Pos2 = goblib::shape2d::Point<fx16>;
using Rect2 = goblib::shape2d::Rectangle<std::int16_t>;
#endif
using Pos2i = goblib::shape2d::Point<std::int16_t>;

constexpr std::int32_t APP_FPS = 30;

// seconds to frame times.
constexpr std::int32_t  operator "" _fsec(unsigned long long x) { return static_cast<std::int32_t>(x * APP_FPS); }
// minutes to ftame times.
constexpr std::int32_t  operator "" _fmin(unsigned long long x) { return static_cast<std::int32_t>(x * 60_fsec); }

#endif
