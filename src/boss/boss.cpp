/*!
  TinyDarius

  @file boss.hpp
  @brief Boss base
*/
#include <LovyanGFX.hpp>

#include "boss.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../game.hpp"
#include "../renderer.hpp"
#include "../utility.hpp"
#include "../constants.hpp"
#include "../behavior.hpp"
#include "../effect/explosion.hpp"
#include "../enemy/enemy.hpp"

#include "king_fossil.hpp"
//#include "electric_fan.hpp"

#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;
#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GCellSprite4;
#include <lgfx/gob_lgfx_animated_palette.hpp>
#include <gob_utility.hpp>
#include <gob_easing.hpp>

#include <cassert>

namespace
{
constexpr Pos2 INITIAL_POS(SCREEN_WIDTH, SCREEN_HEIGHT);
constexpr Pos2 LEAVE_TO(80, 240);
constexpr std::int32_t LEAVE_TIMES = 6_fsec;
};

Boss* Boss::create(std::int8_t stage)
{
    Boss* p = nullptr;
    switch(stage)
    {
    case 0: p = new KingFossil();   break; // A
        //    case 1: p = new ElectricFan(ElectricFan::Type::B);  break; // B
        //    case 2: p = new ElectricFan(ElectricFan::Type::C);  break; // C
        /*
          case 3: p = new DualShears(DualShears::Type::D);    break; // D
          case 4: p = new DualShears(DualShears::Type::E);    break; // E
          case 5: p = new DualShears(DualShears::Type::F);    break; // F
        */
    default:    p = new KingFossil(); break;
    }
    return p;
}

Boss::Boss(std::uint32_t pts)
        : GameObj(PRIORITY_BOSS, ORDER_BOSS_BODY, CATEGORY_BOSS, "boss")
        , _sprite(nullptr)
        , _bodyRect()
        , _behavior(nullptr)
        , _hp(0)
        , _partials()
        , _apalette(nullptr)
        , _palettes{}
        , _function(&Boss::onNop)
        , _from(), _to()
        , _to_times(0)
        , _status(Status::Nop)
        , _score(pts)
{
    _sprite = new GCellSprite4();
    assert(_sprite);
    
    _apalette = new goblib::lgfx::AnimatedPalette(16, _sprite);
    assert(_apalette);

    _score.insertObserver(TinyDarius::instance().game());
    
    move(INITIAL_POS);
}

Boss::~Boss()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_apalette);
    goblib::safeDelete(_behavior);
    goblib::safeDelete(_sprite);
}

void Boss::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool Boss::onInitialize()
{
    // Store original palettes.
    getPaletteColors(_palettes.data(), _palettes.size(), *_sprite);
    _sprite->makePaletteTable256();

    return GameObj::onInitialize();
}

bool Boss::onRelease()
{
    return GameObj::onRelease();
}

void Boss::onExecute(const float delta)
{
    (this->*_function)(delta);
    if(_apalette->pump())
    {
        _sprite->makePaletteTable256();
    }
    GameObj::onExecute(delta);
}

void Boss::onHit(GameObj* o, const Rect2& hit)
{
    if(o->category() == CATEGORY_BULLET)
    {
        --_hp;
    }
}

void Boss::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    _sprite->pushCellTo16(target, _bodyRect, static_cast<std::int32_t>(x()), static_cast<std::int32_t>(y()) + rarg->yorigin, 0);

    GameObj::render(arg);
}

void Boss::_appear(const ColorChange* gray, std::size_t gsz, const Pos2& to, std::uint32_t times)
{
    TD_PRINTF("%s\n", __PRETTY_FUNCTION__);
    _function = &Boss::onAppear;
    _status = Status::Appear;

    move(INITIAL_POS);
    resetAnimation();
    disableHit();
    resume();
    pausePartials();
    show();

    applyPaletteColors(*_sprite, gray, gsz);
    _apalette->from(*_sprite);
    _sprite->makePaletteTable256();

    moveTo(to, times);
}

void Boss::onAppear(const float delta)
{
    if(pumpMove())
    {
        move(_to);
        prepare();
    }
}

void Boss::prepare()
{
    TD_PRINTF("%s\n", __PRETTY_FUNCTION__);
    _function = &Boss::onPrepare;
    _status = Status::Prepare;;
    towardOriginalColor(*_sprite, 60);
}

void Boss::onPrepare(const float delta)
{
    if(_apalette->empty())
    {
        battle();
    }
}

void Boss::battle()
{
    TD_PRINTF("%s\n", __PRETTY_FUNCTION__);
    _function = &Boss::onBattle;
    _status = Status::Battle;
    resumePartials();
    enableHit();
    resetCounter();
    if(_behavior) { _behavior->reset(); }
}

void Boss::onBattle(const float delta)
{
    if(_behavior) { _behavior->pump(delta); }
    if(_hp <= 0)
    {
        defeat();
    }
}

void Boss::_defeat(const ColorChange* gray, std::size_t gsz, const ExplosionSetting* eset, std::size_t esz)
{
    TD_PRINTF("%s\n", __PRETTY_FUNCTION__);
    _function = &Boss::onDefeat;
    _status = Status::Defeat;
    resetCounter();
    
    pausePartials();
    disableHit();
    _apalette->pause();

    goblib::TaskMessage msg;
    msg.msg = MSG_DEFEAT_BOSS;
    msg.arg = this;
    TinyDarius::instance().postBroadcastMessage(msg);

    towardPaletteColors(*_apalette, gray, gsz, 60);
    explode(eset, esz);

    _score.notify();
}

void Boss::onDefeat(const float delta)
{
    if(counter() > 3_fsec)
    {
        _apalette->resume();
        if(_apalette->empty())
        {
            leave();
        }
    }
}

void Boss::_escape(const ColorChange* gray, std::size_t gsz)
{
    TD_PRINTF("%s\n", __PRETTY_FUNCTION__);
    _function = &Boss::onEscape;
    _status = Status::Escape;
    resetCounter();
    _hp = 0;

    disableHit();
    pausePartials();
    _apalette->resume();
    towardPaletteColors(*_apalette, gray, gsz, 60);
}


void Boss::onEscape(const float delta)
{
    if(counter() > 4_fsec)
    {
        if(_apalette->empty())
        {
            leave();
        }
    }
}

void Boss::leave()
{
    TD_PRINTF("%s\n", __PRETTY_FUNCTION__);
    moveTo(LEAVE_TO, LEAVE_TIMES);
    _function = &Boss::onLeave;
    _status = Status::Leave;
    resetCounter();
}

void Boss::onLeave(const float delta)
{
    if(pumpMove())
    {
        move(_to);
        nop();
    }
}

bool Boss::pumpMove()
{
    auto t = goblib::easing::linear(static_cast<float>(counter()) / _to_times);
    auto diff = _to - _from;
    diff *= t;
    move(_from + diff);

    return counter() >= _to_times;
}

void Boss::explode(const ExplosionSetting* table, std::size_t size)
{
    while(size--)
    {
        auto e = new Explosion(table->rpos.x(), table->rpos.y(), ORDER_EXPLOSION_BOSS, static_cast<Explosion::Type>(table->type),
                               this, -1, true);
        assert(e);
        TinyDarius::instance().reserveInsertNode(e, this);
        ++table;
    }
}

void Boss::hideEffect()
{
    goblib::TaskMessage msg;
    msg.msg = MSG_HIDE_EFFECT;
    msg.arg = this;
    TinyDarius::instance().sendBroadcastMessage(msg, this);
}

void Boss::towardOriginalColor(goblib::lgfx::GCellSprite4& sprite, std::uint32_t times)
{
    int_fast8_t i = 0;
    for(auto& e : _palettes)
    {
        _apalette->toward(i++, e, times);
    }
}

//
Boss::Partial::Partial(Boss& parent, goblib::lgfx::GCellSprite4& sprite, const PartialAnimation& animation, std::int32_t relative_priority, std::int32_t relative_order, std::int32_t hp, std::uint32_t pts)
        : GameObj(PRIORITY_BOSS + relative_priority, ORDER_BOSS_BODY + relative_order, CATEGORY_BOSS, "bosspartial")
        , _parent(parent)
        , _sprite(sprite)
        , _animation(animation)
        , _hp(hp)
        , _score(pts)
{
    pauseAnimation();
    _score.insertObserver(TinyDarius::instance().game());
}

Boss::Partial::~Partial()
{
    //    TD_PRINT_FUNCTION();
}

void Boss::Partial::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool Boss::Partial::onInitialize()
{
    return GameObj::onInitialize();
}

bool Boss::Partial::onRelease()
{
    return GameObj::onRelease();
}
void Boss::Partial::onExecute(const float delta)
{
    _animation.pump();
    move(_parent.x() + _animation.rx + _animation.offsetX(), _parent.y() + _animation.ry + _animation.offsetY());
}

void Boss::Partial::onHit(GameObj* o, const Rect2& hit)
{
    if(o->category() == CATEGORY_BULLET)
    {
        Pos2i spos(hit.center().x() - static_cast<Pos2i::pos_type>(pos().x()),
                   hit.center().y() - static_cast<Pos2i::pos_type>(pos().y()));

        explode(spos);

        if(--_hp <= 0)
        {
            _score.notify();
            disableHit();
            hide();
        }
    }
}

void Boss::Partial::explode(const Pos2& pos)
{
    auto exp = new Explosion(pos, ORDER_EXPLOSION_BOSS, Explosion::Random, this);
    assert(exp);
    TinyDarius::instance().reserveInsertNode(exp, this);
}

void Boss::Partial::reflect(const Pos2& pos)
{
    auto r = new Reflection(pos.x(), pos.y());
    assert(r);
    TinyDarius::instance().reserveInsertNode(r, this);
}

void Boss::Partial::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    _sprite.pushCellTo16(target, _animation.rect(), static_cast<std::int32_t>(x()), static_cast<std::int32_t>(y()) + rarg->yorigin, 0);

    GameObj::render(arg);
}

//
Boss::Bullet::Bullet(GameObj& parent, goblib::lgfx::GCellSprite4& sprite, const PartialAnimation& animation)
        : GameObj(PRIORITY_BOSS_BULLET, ORDER_BOSS_BULLET, CATEGORY_ENEMY_BULLET, "bossbullet")
        , _parent(parent)
        , _sprite(sprite)
        , _animation(animation)
        , _behavior(nullptr)
{
}

Boss::Bullet::~Bullet()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_behavior);
}

void Boss::Bullet::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;

}

bool Boss::Bullet::onInitialize()
{
    return GameObj::onInitialize();
}

bool Boss::Bullet::onRelease()
{
    return GameObj::onRelease();
}

void Boss::Bullet::onExecute(const float delta)
{
    _animation.pump();
    if(_behavior) { _behavior->pump(delta); }
}

void Boss::Bullet::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    _sprite.pushCellTo16(target, _animation.rect(), static_cast<std::int32_t>(x()), static_cast<std::int32_t>(y()) + rarg->yorigin, 0);

    GameObj::render(arg);
}

//
Boss::Hit::Hit(Boss& parent, const HitBox& hbox)
        : GameObj(PRIORITY_BOSS, ORDER_BOSS_BODY - 2, CATEGORY_BIT_OBSTACLE, "bosshitbox")
        , _parent(parent)
        , _chbox(hbox)
{
    _hbox = hbox;
    move(_parent.x(), _parent.y());
}

void Boss::Hit::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;

}

void Boss::Hit::onExecute(const float)
{
    move(_parent.x(), _parent.y());
}

void Boss::Hit::render(void* arg)
{
    GameObj::render(arg);
}
