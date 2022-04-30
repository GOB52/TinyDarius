/*!
  TinyDarius

  @file silver_hawk.cpp
  @brief My ship
*/
#include <LovyanGFX.hpp>

#include "silver_hawk.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../renderer.hpp"
#include "../typedef.hpp"
#include "../utility.hpp"
#include "../effect/explosion.hpp"

#include <gob_utility.hpp>
#include <gob_suffix.hpp>
using goblib::suffix::operator"" _u8;
#include <gob_easing.hpp>
#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GSprite;
using goblib::lgfx::GSprite4;
#include <gob_m5s_faces.hpp>
using goblib::m5s::FaceGB;

#include <cstdio>
#include <cassert>

using goblib::lgfx::CellRect;
using Seq = goblib::graph::Sequence;

namespace
{
constexpr char BITMAP_PATH[] = "/res/td/sh.bmp";

constexpr CellRect BODY_RECT(8, 0, 24, 16);
constexpr CellRect BULLET_RECT(32, 0, 16, 8);
constexpr std::int32_t BULLET_VELOCITY_X = 8;

constexpr Pos2 INITIAL_POS(8, 80);
constexpr std::int32_t REWIND_TIMES = 3_fsec;

constexpr HitBox HBOX_BODY =     { Pos2(1,3), Rect2(0,0, 20, 10) };
constexpr HitBox HBOX_BULLET =  { Pos2(2,0), Rect2(0,0, 16-2, 8) };

constexpr std::int16_t VELOCITY = 4;
constexpr float CS45 = 0.70710678118f;  // sin(deg2rad(45)), cos(deg2rad(45))
constexpr std::array<Pos2, 16> MOVE_TABLE =
{
    Pos2(),
    Pos2(0, -VELOCITY), // up
    Pos2(0,  VELOCITY), // down
    Pos2(),
    Pos2(-VELOCITY,  0), // left
    Pos2(-VELOCITY * CS45, -VELOCITY * CS45), // left up
    Pos2(-VELOCITY * CS45,  VELOCITY * CS45), // left down
    Pos2(),
    Pos2(VELOCITY, 0), // right
    Pos2(VELOCITY * CS45, -VELOCITY * CS45), // right up
    Pos2(VELOCITY * CS45,  VELOCITY * CS45), // right down
    Pos2(),
    Pos2(),
    Pos2(),
    Pos2(),
    Pos2(),
};    
//
}

SilverHawk::SilverHawk()
        : GameObj(PRIORITY_SILVERHAWK, ORDER_SILVERHAWK, CATEGORY_PLAYER,"silverhawk")
        , _sprite(nullptr)
        , _function(&SilverHawk::onPlay)
        , _burner
{
    -8, 0,
    {
        CellRect(0,  0, 8, 16),
        CellRect(0, 16, 8, 16),
    },
    {
        Seq(Seq::Draw, 0, 3_u8),
        Seq(Seq::Draw, 1, 3_u8),
        Seq(Seq::Goto, 0_u8),
    }
}
        , _enable(true)
        , _bullets(BULLET_MAX)
{
    _sprite = new goblib::lgfx::GCellSprite4();
    assert(_sprite);
    _hbox = HBOX_BODY;
}

SilverHawk:: ~SilverHawk()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_sprite);
}

void SilverHawk::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool SilverHawk::onInitialize()
{
    if(createFromBitmap(*_sprite, BITMAP_PATH))
    {
        _sprite->makePaletteTable256();
        return GameObj::onInitialize();
    }
    kill();
    return false;
}

bool SilverHawk::onRelease()
{
    return GameObj::onRelease();
}

void SilverHawk::onExecute(const float delta)
{
    _burner.pump();
    (this->*_function)(delta);
    GameObj::onExecute(delta);
}

void SilverHawk::onPlay(const float delta)
{
    if(!_enable) { return; }

    FaceGB& input = TinyDarius::instance().input();
    auto cross = input.getNow() & FaceGB::MASK_CROSS; // 0x00~0x0F
    assert(cross < 16 && "Invalid input");
    if(input.wasRepeated(FaceGB::Button::A))
    {
        shoot();
    }

    auto p = pos();
    p += MOVE_TABLE[cross];
    std::int16_t xx = goblib::clamp(static_cast<Rect2::pos_type>(p.x()),
                                    FIELD_RECT.left(), static_cast<Rect2::pos_type>(FIELD_RECT.right() - BODY_RECT.width()));
    std::int16_t yy = goblib::clamp(static_cast<Rect2::pos_type>(p.y()),
                                    FIELD_RECT.top(), static_cast<Rect2::pos_type>(FIELD_RECT.bottom() - BODY_RECT.height()));
    move(Pos2(xx,yy));
}

void SilverHawk::onRewind(const float delta)
{
    auto t = goblib::easing::linear(static_cast<float>(counter()) / REWIND_TIMES);
    auto diff = _to - _from;
    diff *= t;
    move(_from + diff);

    if(t >= 1.0f)
    {
        _function = &SilverHawk::onPlay;
    }
}

void SilverHawk::onDead(const float delta)
{
    if(counter() == 10)
    {
        hide();
    }
}

void SilverHawk::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    std::int16_t xx = static_cast<std::int16_t>(x());
    std::int16_t yy = static_cast<std::int16_t>(y());

    _sprite->pushCellTo16(target, BODY_RECT,      xx,              yy + rarg->yorigin, 0);
    _sprite->pushCellTo16(target, _burner.rect(), xx + _burner.rx, yy + _burner.ry + rarg->yorigin, 0);

    GameObj::render(arg);
}

void SilverHawk::shoot()
{
    Bullet* b = _bullets.construct(*this);
    if(b)
    {
        b->move(x() + BODY_RECT.width() - 8, y() + 4);
        TinyDarius::instance().reserveInsertNode(b, this);
    }
}

void SilverHawk::restart()
{
    _function = &SilverHawk::onPlay;
    enable(false);
    move(INITIAL_POS);
}

void SilverHawk::rewind()
{
    _function = &SilverHawk::onRewind;
    enable(false);
    _from = pos();
    _to = INITIAL_POS;
    resetCounter();
}

void SilverHawk::dead()
{
    _function = &SilverHawk::onDead;
    resetCounter();
    enable(false);

    auto exp = new Explosion(x()+12, y()+8, ORDER_EXPLOSION);
    assert(exp);
    TinyDarius::instance().reserveInsertNode(exp, this);
}

//

SilverHawk::Bullet::Bullet(SilverHawk& sh)
        : GameObj(PRIORITY_BULLET, ORDER_BULLET, CATEGORY_BULLET, "bullet"), _sh(sh)
{
    _hbox = HBOX_BULLET;
}

SilverHawk::Bullet::~Bullet()
{
    //    TD_PRINT_FUNCTION();
}

void SilverHawk::Bullet::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    // Don't delete, return to object pool.
    _sh._bullets.destruct(this);
    GameObj::onUnchain();
}

bool SilverHawk::Bullet::onInitialize()
{
    return GameObj::onInitialize();
}

bool SilverHawk::Bullet::onRelease()
{
    return GameObj::onRelease();
}

void SilverHawk::Bullet::onExecute(const float delta)
{
    offset(Pos2(BULLET_VELOCITY_X, 0));

    std::int16_t xx = static_cast<std::int16_t>(x());
    std::int16_t yy = static_cast<std::int16_t>(y());
    if(xx > SCREEN_WIDTH || xx < -BULLET_RECT.width() || yy > SCREEN_HEIGHT || yy < -BULLET_RECT.height())
    {
        release();
    }
    GameObj::onExecute(delta);
}

void SilverHawk::Bullet::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;

    _sh._sprite->pushCellTo16(target, BULLET_RECT, static_cast<std::int16_t>(x()), static_cast<std::int16_t>(y()) + rarg->yorigin, 0);
    GameObj::render(arg);
}
