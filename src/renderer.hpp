/*!
  TinyDarius

  @file renderer.hpp
  @brief utility for rendering
*/
#pragma once
#ifndef TD_RENDERER_HPP
#define TD_RENDERER_HPP

#include <gob_renderer.hpp>
#include <lgfx/gob_lgfx.hpp>
#include "typedef.hpp"

// Argument for render
struct RenderArg
{
    std::int32_t yorigin;
    goblib::lgfx::GSprite* sprite;
};

// Clip sprite
struct ScopedClip
{
  public:
    ScopedClip(goblib::lgfx::GSprite& target, const Rect2& clip, const std::int32_t yorigin);
    ~ScopedClip();

    Rect2 clip() const { return _clip; }

  private:
    goblib::lgfx::GSprite& _sprite;
    Rect2 _rect, _clip;
};

#endif
