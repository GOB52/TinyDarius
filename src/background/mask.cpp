/*!
  TinyDarius

  @file mask.cpp
  @brief Masking
*/
#include "../lgfx.hpp"
#include "mask.hpp"
#include "../debug.hpp"
#include "../renderer.hpp"

#include <lgfx/gob_lgfx.hpp>

Mask::~Mask()
{
    //    TD_PRINT_FUNCTION();
}

void Mask::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

void Mask::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;
    target->fillRect(_rect.left(), _rect.top() + rarg->yorigin, _rect.width(), _rect.height(), CLR_BLACK);
    //    target->fillRect(_rect.left(), _rect.top() + rarg->yorigin, _rect.width(), _rect.height(), CLR_RED);
}
