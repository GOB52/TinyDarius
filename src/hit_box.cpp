/*!
  TinyDarius

  @file hit_box.cpp
  @brief Collision
*/
#include <LovyanGFX.hpp>
#include "hit_box.hpp"
#include "renderer.hpp"
#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;


void HitBox::render(void* arg, std::uint32_t clr)
{
#ifndef NDEBUG
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;

    if(_box)
    {
        target->drawRect(_box.left(), _box.top() + rarg->yorigin,
                         _box.width(), _box.height(), clr);
    }
#endif
}
