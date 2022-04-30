/*!
  TinyDarius

  @file score.hpp
  @brief Score subject
*/

#pragma once
#ifndef TD_SCORE_HPP
#define TD_SCORE_HPP

#include <gob_notification.hpp>

class Score : public goblib::Subject<Score>
{
  public:
    explicit Score(std::uint32_t pts) : _pts(pts) {}
    GOBLIB_INLINE std::uint32_t score() const { return _pts; }
  private:
    const std::uint32_t _pts;
};

#endif
