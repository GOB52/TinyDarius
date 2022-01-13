/*!
  TinyDarius

  @file branch_rock.cpp
  @brief Rocks for branching that appear when cleared.
*/
#include <LovyanGFX.hpp>

#include "../debug.hpp"
#include "branch_rock.hpp"
#include "../app.hpp"
#include "../renderer.hpp"
#include "../utility.hpp"
#include "../constants.hpp"

#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GSprite;
using goblib::lgfx::GSprite4;
#include <lgfx/gob_lgfx_animated_palette.hpp>
using goblib::lgfx::AnimatedPalette;
#include <gob_utility.hpp>

#include <cassert>

namespace
{
constexpr char BITMAP_PATH[] = "/res/td/branch.bmp";
constexpr Pos2 INITIAL_POS(SCREEN_WIDTH, SCREEN_HEIGHT/2 - 8);
constexpr Pos2 VELOCITY(-12, 0);
constexpr HitBox HBOX{Pos2(0,0), Rect2(0,0, SCREEN_WIDTH * 2, 16)};
}

BranchRock::BranchRock()
        : GameObj(PRIORITY_BRANCHROCK, ORDER_BRANCHROCK, CATEGORY_WALL, "branchrock")
        , _sprite(nullptr)
        , _apalette(nullptr)
{
    _sprite = new GSprite4();
    assert(_sprite);
    _apalette = new AnimatedPalette(16, _sprite);
    assert(_apalette);

    _hbox = HBOX;
    move(INITIAL_POS);
}

BranchRock::~BranchRock()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_apalette);
    goblib::safeDelete(_sprite);
}

void BranchRock::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool BranchRock::onInitialize()
{
    if(createFromBitmap(*_sprite, BITMAP_PATH))
    {
        _sprite->makePaletteTable256();
        _apalette->from(*_sprite);
        return GameObj::onInitialize();
    }
    kill();
    return false;
}

bool BranchRock::onRelease()
{
    return GameObj::onRelease();
}

void BranchRock::onExecute(const float delta)
{
    offset(VELOCITY);
    if(x() < -_sprite->width())
    {
        offset(Pos2(_sprite->width(), 0));
        // keep hitbox in screen
        _hbox.move(Pos2::pos_type(0), y());
    }

    if(_apalette->pump())
    {
        _sprite->makePaletteTable256();
    }
}

void BranchRock::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    std::int16_t xx = static_cast<std::int16_t>(x());
    std::int16_t yy = static_cast<std::int16_t>(y());

    _sprite->pushSpriteTo16(target, xx,                        yy + rarg->yorigin, 0);
    _sprite->pushSpriteTo16(target, xx + _sprite->width() * 1, yy + rarg->yorigin, 0);
    _sprite->pushSpriteTo16(target, xx + _sprite->width() * 2, yy + rarg->yorigin, 0);

    GameObj::render(arg);
}
