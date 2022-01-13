/*!
  TinyDarius

  @file explosion.cpp
  @brief Explosion
*/
#include <LovyanGFX.hpp>

#include "explosion.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../renderer.hpp"
#include "../utility.hpp"
#include "../constants.hpp"

#include <lgfx/gob_lgfx_animated_sprite.hpp>
#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GSprite;
using goblib::lgfx::GCellSprite4;
#include <gob_utility.hpp>
#include <gob_suffix.hpp>
using goblib::suffix::operator"" _u8;

#include <esp_system.h>
#include <random>
#include <cassert>

using goblib::lgfx::CellRect;
using Seq = goblib::graph::Sequence;

namespace
{
constexpr static char BITMAP_PATH[] = "/res/td/bomb.bmp";
}

const PartialAnimation Explosion::_animations[Explosion::Type::Max] =
{
    // type 0
    {
        0,0,
        {
            CellRect(  0,  0, 32, 32),
            CellRect( 32,  0, 32, 32),
            CellRect( 64,  0, 32, 32),
            CellRect( 96,  0, 32, 32),
            CellRect(128,  0, 32, 32),
        },
        {
            Seq(Seq::Draw, 0, 4_u8),
            Seq(Seq::Draw, 1, 4_u8),
            Seq(Seq::Draw, 2, 6_u8),
            Seq(Seq::Draw, 3, 6_u8),
            Seq(Seq::Draw, 4, 4_u8),
        }
    },
    // type 1
    {
        0,0,
        {
            CellRect(  0, 32, 32, 32),
            CellRect( 32, 32, 32, 32),
            CellRect( 64, 32, 32, 32),
        },
        {
            Seq(Seq::Draw, 0, 4_u8),
            Seq(Seq::Draw, 1, 6_u8),
            Seq(Seq::Draw, 2, 4_u8),
        }
    },
    // type 2
    {
        0,0,
        {
            CellRect( 96, 32, 32, 32),
            CellRect(128, 32, 32, 32),
            CellRect(160, 32, 32, 32),
            CellRect(192, 32, 32, 32),
            CellRect(224, 32, 32, 32),
        },
        {
            Seq(Seq::Draw, 0, 4_u8),
            Seq(Seq::Draw, 1, 4_u8),
            Seq(Seq::Draw, 2, 6_u8),
            Seq(Seq::Draw, 3, 6_u8),
            Seq(Seq::Draw, 4, 4_u8),
        }
    }
};


goblib::lgfx::GCellSprite4* Explosion::_sprite;


void Explosion::setup()
{
    ScopedReleaseBus();    

    if(_sprite) { return; }
    
    _sprite = new goblib::lgfx::GCellSprite4();
    assert(_sprite);
    if(!createFromBitmap(*_sprite, BITMAP_PATH)) { assert(0); }
    _sprite->makePaletteTable256();
}

void Explosion::finalize()
{
    goblib::safeDelete(_sprite);

}

Explosion::Explosion(const Pos2::pos_type cx, const Pos2::pos_type cy, std::uint32_t order, Type t, GameObj* follow, std::int32_t apos, bool repeat)
        : GameObj(PRIORITY_EXPLOSION, order, CATEGORY_NONE, "explosion")
        , _cpos(cx,cy)
        , _animation()
        , _follow(follow)
        , _repeat(repeat)
{
    std::mt19937 re(esp_random()); // esp_random() is hardware RNG.
    if(t == Type::Random)
    {
        std::uniform_int_distribution<> dist(0, goblib::to_underlying(Type::Max)-1);
        t = static_cast<Type>(dist(re));
    }
    assert(t >=0 );
    assert(t < Type::Max);
    t = static_cast<Type>(goblib::clamp(goblib::to_underlying(t), goblib::to_underlying(Type::Type_0), goblib::to_underlying(Type::Type_2)));
    _animation = _animations[t];

    auto left = cx + (_follow ? _follow->x() : Pos2::pos_type(0)) - WIDTH/2;
    auto top  = cy + (_follow ? _follow->y() : Pos2::pos_type(0)) - HEIGHT/2;
    move(left, top);
    
    if(apos < 0)
    {
        std::uniform_int_distribution<> dist(0U, _animation.sequencer.stepSize() -1);
        apos = dist(re);
    }
    while(apos-- > 0) { _animation.pump(); }

}

Explosion:: ~Explosion()
{
    //    TD_PRINT_FUNCTION();
}

void Explosion::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool Explosion::onInitialize()
{
    return GameObj::onInitialize();
}

bool Explosion::onRelease()
{
    return GameObj::onRelease();
}

void Explosion::onReceive(const goblib::TaskMessage& msg)
{
    if(msg.msg == MSG_HIDE_EFFECT)
    {
        release();
    }
}

void Explosion::onExecute(const float delta)
{
    if(_follow)
    {
        move(_cpos.x() + _follow->x() - WIDTH/2, _cpos.y() + _follow->y() - HEIGHT/2);
    }

    _animation.pump();

    if(_animation.isFinish())
    {
        if(_repeat)
        {
            _animation.reset();
        }
        else
        {
            release();
        }
    }
}

void Explosion::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    _sprite->pushCellTo16(target, _animation.rect(), static_cast<std::int32_t>(x()), static_cast<std::int32_t>(y()) + rarg->yorigin, 0);
}
