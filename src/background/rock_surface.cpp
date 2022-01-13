/*!
  TinyDarius

  @file rock_surface.hpp
  @brief upper/lower rock surface
*/
#include <LovyanGFX.hpp>

#include "rock_surface.hpp"
#include "../debug.hpp"
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
constexpr char BITMAP_PATH[] = "/res/td/rock.bmp";
constexpr HitBox HIT_BOX{ Pos2(0,0), Rect2(0,0, SCREEN_WIDTH, 16) };
}
const std::array<Pos2, RockSurfaceUpper::Velocity::Max> RockSurfaceUpper::VELOCITY_TABLE =
{
    Pos2(8, 0),
    Pos2(12, 0),
};

RockSurfaceUpper:: RockSurfaceUpper(std::int32_t left, std::int32_t top)
        : GameObj(PRIORITY_ROCKSURFACE, ORDER_ROCKSURFACE, CATEGORY_WALL, "rocksurface")
        , _sprite(nullptr)
        , _apalette(nullptr)
        , _velocity(VELOCITY_TABLE[Velocity::Normal])
{
    _sprite = new goblib::lgfx::GSprite4();
    assert(_sprite);
    _apalette = new AnimatedPalette(16, _sprite);
    assert(_apalette);

    _hbox = HIT_BOX;;
    move(Pos2(left, top));
}

RockSurfaceUpper:: ~RockSurfaceUpper()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_apalette);
    goblib::safeDelete(_sprite);
}

void RockSurfaceUpper::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool RockSurfaceUpper::onInitialize()
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

bool RockSurfaceUpper::onRelease()
{
    return GameObj::onRelease();
}

void RockSurfaceUpper::onExecute(const float delta)
{
    // loop
    auto np = pos() + _velocity;
    if(np.x() > RENDER_WIDTH)
    {
        auto left = static_cast<std::int32_t>(np.x()) % RENDER_WIDTH;
        np = Pos2(Pos2::pos_type(left), y());
    }
    move(np);
    
    if(_apalette->pump())
    {
        _sprite->makePaletteTable256();
    }
    // keep hitbox in screen
    _hbox.move(Pos2::pos_type(0), y());
}

void RockSurfaceUpper::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    std::int32_t left = -(static_cast<std::int32_t>(x()) % RENDER_WIDTH);
    std::int32_t yy = static_cast<std::int32_t>(y());

    assert(left >= -320 && "Illegal left");
    
    _sprite->pushSpriteTo16(target, left,                yy + rarg->yorigin);
    _sprite->pushSpriteTo16(target, left + SCREEN_WIDTH, yy + rarg->yorigin);

    GameObj::render(arg);
}

RockSurfaceLower:: RockSurfaceLower(std::int32_t left, std::int32_t top)
        : RockSurfaceUpper(left, top)
{
    // keep hitbox in screen
    _hbox = HIT_BOX;
    _hbox.move(Pos2(0, top));
}

bool RockSurfaceLower::onInitialize()
{
    goblib::lgfx::GSprite4 tmp;

    if(createFromBitmap(tmp, BITMAP_PATH))
    {
        // Create reversed virtical, because reverce rendering on th fly is slow.
        auto wid = tmp.width();
        auto hgt = tmp.height();
        _sprite->setColorDepth(tmp.getColorDepth());
        _sprite->createSprite(wid, hgt);
        tmp.setPivot(wid * 0.5f - 0.5f, hgt * 0.5f - 0.5f);
        tmp.pushRotateZoom(_sprite, wid/2 - 0.5f, hgt/2 - 0.5f, 0, 1.0f, -1.0f);
        auto* ps = tmp.getPalette();
        auto* ds = _sprite->getPalette();
        auto len = tmp.getPaletteCount();;
        while(len--) { *ds++ = *ps++; }

        _sprite->makePaletteTable256();
        _apalette->from(*_sprite);

        return GameObj::onInitialize();
    }
    kill();
    return false;
}
