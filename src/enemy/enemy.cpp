/*!
  TinyDarius

  @file enemy.cpp
  @brief Enemies and effect
*/
#include "../lgfx.hpp"
#include "enemy.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../game.hpp"
#include "../renderer.hpp"
#include "../utility.hpp"
#include "../constants.hpp"
#include "../behavior.hpp"

#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GCellSprite4;
#include <gob_suffix.hpp>
using goblib::suffix::operator"" _u8;
#include <gob_m5s_random.hpp>
#include <gob_utility.hpp>

#include <random>
#include <esp_system.h>
#include <cassert>

using goblib::lgfx::CellRect;
using Seq = goblib::graph::Sequence;

namespace
{
constexpr char BITMAP_PATH[] = "/res/td/enemy.bmp";

constexpr HitBox HBOX_MINE = { Pos2(2,2), Rect2(0, 0, 16-2*2, 16-2*2) };
constexpr HitBox HBOX_YAZUKA = { Pos2(2,2), Rect2(0, 0, 16-2*2, 16-2*2) };
}

goblib::lgfx::GCellSprite4* Enemy::_sprite;

const PartialAnimation Enemy::_animation_table[] =
{
    {   // Mine
        0,0,
        {
            CellRect( 24, 32, 16, 16),
            CellRect( 40, 32, 16, 16),
            CellRect( 56, 32, 16, 16),
            CellRect( 72, 32, 16, 16),
        },
        {
            Seq(Seq::Draw, 0, 4_u8),
            Seq(Seq::Draw, 1, 4_u8),
            Seq(Seq::Draw, 2, 4_u8),
            Seq(Seq::Draw, 3, 8_u8),
            Seq(Seq::Draw, 2, 4_u8),
            Seq(Seq::Draw, 1, 4_u8),
            Seq(Seq::Goto, 0_u8),
        },
    },
    {   // Yazuka(CW)
        0,0,
        {
            CellRect(  0, 16,  16, 16),
            CellRect( 16, 16,  16, 16),
            CellRect( 32, 16,  16, 16),
            CellRect( 48, 16,  16, 16),
            CellRect( 64, 16,  16, 16),
            CellRect( 80, 16,  16, 16),
        },
        {
            Seq(Seq::Draw, 0, 4_u8),
            Seq(Seq::Draw, 1, 4_u8),
            Seq(Seq::Draw, 2, 4_u8),
            Seq(Seq::Draw, 3, 4_u8),
            Seq(Seq::Draw, 4, 4_u8),
            Seq(Seq::Draw, 5, 4_u8),
            Seq(Seq::Goto, 0_u8),
        }
    },
    {   // Yazuka(CCW)
        0,0,
        {
            CellRect(  0, 16,  16, 16),
            CellRect( 16, 16,  16, 16),
            CellRect( 32, 16,  16, 16),
            CellRect( 48, 16,  16, 16),
            CellRect( 64, 16,  16, 16),
            CellRect( 80, 16,  16, 16),
        },
        {
            Seq(Seq::Draw, 0, 4_u8),
            Seq(Seq::Draw, 5, 4_u8),
            Seq(Seq::Draw, 4, 4_u8),
            Seq(Seq::Draw, 3, 4_u8),
            Seq(Seq::Draw, 2, 4_u8),
            Seq(Seq::Draw, 1, 4_u8),
            Seq(Seq::Goto, 0_u8),
        }
    },
    {   // Crash
        0,0,
        {
            CellRect( 32,  0,  16, 16),
            CellRect( 48,  0,  16, 16),
            CellRect( 64,  0,  16, 16),
            CellRect( 80,  0,  16, 16),
        },
        {
            Seq(Seq::Draw, 0, 2_u8),
            Seq(Seq::Draw, 1, 2_u8),
            Seq(Seq::Draw, 2, 3_u8),
            Seq(Seq::Draw, 3, 4_u8),
        }
    },
    {   // Reflection
        0,0,
        {
            CellRect(  0,  0,  16, 16),
            CellRect( 16,  0,  16, 16),
        },
        {
            Seq(Seq::Draw, 0, 1_u8),
            Seq(Seq::Draw, 1, 4_u8),
        }
    }
};

void Enemy::setup()
{
    ScopedReleaseBus();
    if(_sprite) { return; }

    _sprite = new goblib::lgfx::GCellSprite4();
    assert(_sprite);
    if(!createFromBitmap(*_sprite, BITMAP_PATH)) { assert(0); }
    _sprite->makePaletteTable256();
}

void Enemy::finalize()
{
    goblib::safeDelete(_sprite);
}

Enemy::Enemy(const Pos2::pos_type cx, const Pos2::pos_type cy, const PartialAnimation& animation,
             const std::uint32_t priority, const std::uint32_t order, const std::uint32_t category, const std::uint32_t score, const char* tag)
        : GameObj(priority, order, category, tag)
        , _animation(animation)
        , _behavior(nullptr)
        , _score(score)
        , _alive(true)
{
    move(cx, cy);
    _score.insertObserver(TinyDarius::instance().game());
}

Enemy::~Enemy()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_behavior);
}

void Enemy::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool Enemy::onInitialize()
{
    return GameObj::onInitialize();
}

bool Enemy::onRelease()
{
    return GameObj::onRelease();
}

void Enemy::onReceive (const goblib::TaskMessage& msg)
{
    if(msg.msg == MSG_VANISH_ENEMY)
    {
        defeat();
    }
}

void Enemy::onExecute(const float delta)
{
    if(_behavior) { _behavior->pump(delta); }
    _animation.pump();
    if(_animation.isFinish()) { release(); }
    GameObj::onExecute(delta);
}

void Enemy::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;

    _sprite->pushCellTo16(target, _animation.rect(), static_cast<std::int32_t>(x()), static_cast<std::int32_t>(y()) + rarg->yorigin, 0);
    GameObj::render(arg);
}    

void Enemy::onHit(GameObj* o, const Rect2& r)
{
    if(o->category() == CATEGORY_BULLET)
    {
        _alive = false;
        _score.notify();
        defeat();
    }
}

void Enemy::defeat()
{
    disableHit();
    hide();

    auto crash = new ::Crash(*this);
    assert(crash);
    TinyDarius::instance().reserveInsertNode(crash, this);
}


//
Crash::Crash(const Pos2::pos_type cx, const Pos2::pos_type cy)
        : Enemy(cx,cy, _animation_table[Type::Crash], PRIORITY_EXPLOSION, ORDER_EXPLOSION_ENEMY, CATEGORY_NONE, 0, "crash")
        , _attach(nullptr)
{
    move(cx - WIDTH/2, cy - HEIGHT/2);
}

Crash::Crash(GameObj& attach)
        : Enemy(attach.x(), attach.y(), _animation_table[Type::Crash], PRIORITY_EXPLOSION, ORDER_EXPLOSION_ENEMY, CATEGORY_NONE, 0, "crash")
        , _attach(&attach)
{
}

Crash::~Crash()
{
    //    TD_PRINT_FUNCTION();
}

void Crash::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

void Crash::onExecute(const float delta)
{
    Enemy::onExecute(delta);
    if(_attach)
    {
        move(_attach->x(), _attach->y());
        if(isRelease() || isKill())
        {
            _attach->release();
        }
    }
}

//
Reflection::Reflection(const Pos2::pos_type cx, const Pos2::pos_type cy)
        : Enemy(cx,cy, _animation_table[Type::Reflection], PRIORITY_EXPLOSION, ORDER_EXPLOSION, CATEGORY_NONE, 0, "reflection")
{
    move(cx - WIDTH/2, cy - HEIGHT/2);
}

Reflection::~Reflection()
{
    //    TD_PRINT_FUNCTION();
}

void Reflection::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

//
Mine::Mine(MineGenerator& generator, const Pos2& cpos, const std::int32_t split)
        : Enemy(cpos, _animation_table[Type::Mine], PRIORITY_ENEMY, ORDER_ENEMY, CATEGORY_ENEMY, PTS_MINE, "mine")
        , _generator(generator)
        , _split(split)
{
    _hbox = HBOX_MINE;

    auto range = 5 - goblib::clamp(split, 0, 4); // 1~5

    std::mt19937 gen(esp_random());
    std::uniform_real_distribution<> aa(range * 0.2f, range * 1.2f);
    std::uniform_real_distribution<> angle(0.25f, 1.0f);
    std::uniform_int_distribution<> rot(0,31);

    auto a = fx16(aa(gen));
    auto b = fx16(aa(gen));
    auto av = fx16(angle(gen));
    if(gen()&1) { av = -av; }
    auto sa = rot(gen);
    auto erot = rot(gen);

    _behavior = new MineBehavior(*this, a, b, sa, av, VecFx16(fx16(-1.0f/4.0f), fx16()), erot);
    assert(_behavior);
}

Mine::~Mine()
{
    //    TD_PRINT_FUNCTION();
}

void Mine::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    // Don't delete, return to object pool.
    _generator._pool.destruct(this);
    GameObj::onUnchain();
}

void Mine::onHit(GameObj* o, const Rect2& r)
{
    Enemy::onHit(o, r);
    if(_split <= 0) { return; }
    split();
    split();
}

void Mine::onExecute(const float delta)
{
    Enemy::onExecute(delta);
    if(isExecute() && _split > 0 && counter() >= SPLIT_TIME)
    {
        split();
        split();
        _split = 0;
    }
}

void Mine::split()
{
    if(_split > 0)
    {
        std::mt19937 gen(esp_random());
        std::uniform_int_distribution<> dist(-8, 8);
        _generator.spawn(Pos2(x() + dist(gen), y() + dist(gen)), _split - 1);
    }
}

void MineGenerator::spawn()
{
    spawn(Pos2(SCREEN_WIDTH, SCREEN_HEIGHT/2), 4);
}

void MineGenerator::spawn(const Pos2& cpos, const std::int32_t split)
{
    auto p = _pool.construct(*this, cpos, split);
    if(p){ TinyDarius::instance().reserveInsertNode(p, &_belongto); }
}


Yazuka::Yazuka(YazukaGenerator& generator, const Pos2& cpos, bool cw,
               fx16 a, fx16 b, Tangle start, fx16 av, VecFx16 ov, Tangle rotate, std::int32_t wait)
        : Enemy(cpos, _animation_table[cw ? Type::YazukaCW : Type::YazukaCCW], PRIORITY_ENEMY, ORDER_ENEMY, CATEGORY_ENEMY, PTS_YAZUKA, (cw ? "yazukaCW" : "yazukaCCW") )
        , _generator(generator)
        , _wait(wait)
{
    _hbox = HBOX_YAZUKA;
    _behavior = new OrbitalBehavior(*this, a, b, start, av, ov, rotate);
    assert(_behavior);
    show(wait <= 0);
}

Yazuka::~Yazuka()
{
    //    TD_PRINT_FUNCTION();
}

void Yazuka::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    // Don't delete, return to object pool.
    _generator._pool.destruct(this);
    GameObj::onUnchain();
}

void Yazuka::onExecute(const float delta)
{
    show(_alive && _wait <= 0);

    if(_wait <= 0)
    {
        Enemy::onExecute(delta);
        if(x() < -_animation.rect().width()  ) { release(); return; }
    }
    --_wait;
}

//
#ifdef DEBUG
const std::uint32_t YazukaGenerator::MEMBER_MAX = 6;
#endif

void YazukaGenerator::spawn()
{
    std::mt19937 gen(esp_random());
    std::uniform_int_distribution<> du(0,1);
    bool upper = du(gen);
    
    std::uniform_int_distribution<> upperY(FIELD_RECT.top(), FIELD_RECT.top() + 16);
    std::uniform_int_distribution<> lowerY(FIELD_RECT.bottom() -16, FIELD_RECT.bottom());
    std::uniform_int_distribution<>& ypos = upper ? upperY : lowerY;

    auto members = goblib::clamp(3 + _generated++/2, 3U, MEMBER_MAX);
    Pos2 spos( FIELD_RECT.right(), ypos(gen));

    std::uniform_real_distribution<> aa(2.0f, 6.0f);
    std::uniform_real_distribution<> bb(0.5f, 3.0f);
    std::uniform_int_distribution<> rot_upper(9,15);
    std::uniform_int_distribution<> rot_lower(1,7);
    std::uniform_int_distribution<>& rot = upper ? rot_upper : rot_lower;
    
    auto a = fx16(aa(gen));
    auto b = fx16(bb(gen));
    auto av = upper ? fx16(0.5f) : -fx16(0.5f);
    auto er = rot(gen);
    
    for(decltype(members) i=0; i < members; ++i)
    {
        auto p = _pool.construct(*this, spos, upper, a, b, 0, av, VecFx16(fx16(-1),fx16(0)), er, i * 4);
        if(p) {TinyDarius::instance().reserveInsertNode(p, &_belongto); }
    }
}
