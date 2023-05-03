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
    LGFX_Sprite* sprite;
};

// Clip sprite
struct ScopedClip
{
  public:
    ScopedClip(LGFX_Sprite& target, const Rect2& clip, const std::int32_t yorigin);
    ~ScopedClip();

    Rect2 clip() const { return _clip; }

  private:
    LGFX_Sprite& _sprite;
    Rect2 _rect, _clip;
};

#endif
