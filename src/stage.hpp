/*!
  TinyDarius

  @file stage.hpp
  @brief Stage information
*/
#pragma once
#ifndef TD_STAGE_HPP
#define TD_STAGE_HPP

#include <cstdint>
#include <utility>
#include "constants.hpp"
#include <gob_utility.hpp>

struct Stage
{
    using Next = std::pair<std::int8_t, std::int8_t>; // first is upper stage, second is lower stage.

    constexpr static std::uint8_t MAX = 28;
    constexpr static std::uint8_t HISTORY_MAX = 8; // 7+terminator (e.g. A-C-F-J-O-T-Z-terminator)

    static const Stage terminator;
    static const Stage table[MAX];
    
    const std::int8_t current;
    const std::int8_t depth;
    const std::int8_t upper;
    const std::int8_t lower;
    const std::int32_t time;
    const BGM bgm;

    int toChar() const { return current < 0 ? ' ' : (current < 26) ? ('A' + current) : _extra[ current - 26 ]; }
    Next next() const { return Next(upper, lower); }
    BGM bossBgm() const { return static_cast<BGM>(goblib::to_underlying(BGM::BOSS_1) + depth); }
    bool isTail() const { return upper < 0 || lower < 0; }
    
  private:
    static int _extra[]; // Stage charactor for V'/Z'
};

#endif
