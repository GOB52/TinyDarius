/*!
  TinyDarius

  @file renderer.cpp
  @brief utility for rendering
*/
#include <LovyanGFX.hpp>
#include "renderer.hpp"
#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;

ScopedClip::ScopedClip(GSprite& target, const Rect2& clip, const std::int32_t yorigin)
        : _sprite(target), _rect()
{
    std::int32_t x,y,w,h;
    _sprite.getClipRect(&x, &y, &w, &h);
    auto r = _rect = decltype(_rect)(x, y, w, h);
    r.offset(0, -yorigin);

    _clip = r & clip;
    _clip.offset(0, yorigin);
    if(_clip.valid())
    {
        _sprite.setClipRect(_clip.left(), _clip.top(), _clip.width(), _clip.height());
    }
}

ScopedClip::~ScopedClip()
{
    // restore clip rect.
    _sprite.setClipRect(_rect.left(), _rect.top(), _rect.width(), _rect.height());
}
