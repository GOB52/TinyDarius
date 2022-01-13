/*!
  TinyDarius

  @file math_table.hpp
  @brief Table based mathmatics.
*/
#pragma once
#ifndef TD_MATH_TABLE_HPP
#define TD_MATH_TABLE_HPP

#include "typedef.hpp"
#include "constants.hpp"
#include <gob_template_helper.hpp>
#include <gob_fixed_point_number.hpp>
#include <cstdint>
#include <array>


/*! @brief sin/cos division for table based angle
  0              : deg(0)
  TANGLE_DIV*1/4 : deg(90)
  TANGLE_DIV*1/2 : deg(180)
  TANGLE_DIV*3/4 : deg(270)
*/
constexpr std::size_t TANGLE_DIV = 32;

/*! @brief Table based angle definition */
using Tangle = std::int8_t;

/*! @brief tabel based atan2 block width */
constexpr std::int16_t SCREEN_DIV_WIDTH = 16;
/*! @brief tabel based atan2 block height */
constexpr std::int16_t SCREEN_DIV_HEIGHT = 16;

constexpr std::int16_t NUM_OF_HOR = SCREEN_WIDTH / SCREEN_DIV_WIDTH;
constexpr std::int16_t NUM_OF_VER = SCREEN_HEIGHT / SCREEN_DIV_HEIGHT;

/*! @brief table based sin
  @param a Tangle
  @return fx16
*/
fx16 table_sin(const Tangle a);

/*! @brief table based cos
  @param a Tangle
  @return fx16
*/
fx16 table_cos(const Tangle a);

/*! atan2 rerurn table based angle.
  @param y y cordinate
  @param x x cordinate
  @return [0 ~ TANGLE_DIV)
*/
Tangle table_atan2(const std::int16_t y, const std::int16_t x);

#endif
