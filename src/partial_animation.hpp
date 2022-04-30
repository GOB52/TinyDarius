/*!
  TinyDarius

  @file partial_animation.hpp
  @brief Partial animation
*/
#pragma once
#ifndef TD_PARTIAL_ANIMATION_HPP
#define TD_PARTIAL_ANIMATION_HPP

#include <gob_macro.hpp>
#include <gob_animation.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <vector>

struct PartialAnimation
{
    GOBLIB_INLINE std::uint8_t cell() const { return sequencer.cell(); }
    GOBLIB_INLINE const goblib::lgfx::CellRect& rect() const { return rects[sequencer.cell()]; }
    GOBLIB_INLINE std::int32_t offsetX() const { return sequencer.offsetX(); }
    GOBLIB_INLINE std::int32_t offsetY() const { return sequencer.offsetY(); }

    GOBLIB_INLINE bool isFinish() const { return sequencer.isFinish(); }
    GOBLIB_INLINE void pump() { sequencer.pump(); }
    GOBLIB_INLINE void reset() { sequencer.reset(); }   
    GOBLIB_INLINE void pause(bool b) { sequencer.pause(b); }
    GOBLIB_INLINE std::uint8_t index() const { return sequencer.index(); }
    
    //
    std::int32_t rx,ry; // Relative position by parent.
    std::vector<goblib::lgfx::CellRect> rects; // Cell rectangles.
    goblib::graph::AnimationSequencer sequencer;
};

#endif
