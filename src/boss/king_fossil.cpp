/*!
  TinyDarius

  @file king_fossil.cpp
  @brief Coelacanth
*/
#include "../lgfx.hpp"
#include "king_fossil.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../utility.hpp"
#include "../constants.hpp"
#include "../behavior.hpp"
#include "../effect/explosion.hpp"

#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GCellSprite4;
#include <lgfx/gob_lgfx_animated_palette.hpp>
#include <gob_utility.hpp>
#include <gob_math.hpp>
#include <gob_suffix.hpp>
using goblib::suffix::operator"" _u8;
using goblib::suffix::operator"" _i16;

#include <cassert>

using goblib::lgfx::CellRect;
using Seq = goblib::graph::Sequence;

namespace
{
constexpr char BITMAP_PATH[] = "/res/td/kf.bmp";
constexpr CellRect BODY_RECT = {0, 0, 152, 72};

constexpr Pos2 APPEAR_TO(SCREEN_WIDTH - 196, SCREEN_HEIGHT/2 - 40);
constexpr std::int32_t APPEAR_TIMES = 6_fsec;

// Hitpoint
#if 1
constexpr std::int32_t HP_BODY = 30;
#else
constexpr std::int32_t HP_BODY = 1;
#endif
constexpr std::int32_t HP_DORSAFIN = 16;
constexpr std::int32_t HP_ANALFIN = 16;

// Partial gray scale
constexpr ColorChange GRAY_SCALE[] =
{
    { RGBColor(0,24,90),  RGBColor(32,32,64) },
    { RGBColor(0,90,189), RGBColor(80,80,128) },
    { RGBColor(0,41,123), RGBColor(64,64,96) },
};


// Explosion on defeat
constexpr Boss::ExplosionSetting EXPLOSION_TABLE[] =
{
    { Pos2( 16, 32), Explosion::Type::Type_1 },
    { Pos2( 80, 16), Explosion::Type::Type_0 },
    { Pos2( 64, 48), Explosion::Type::Type_2 },
    { Pos2(152, 16), Explosion::Type::Type_1 },
    { Pos2(128, 56), Explosion::Type::Type_0 },
    { Pos2(184, 40), Explosion::Type::Type_2 },
};

// Hitbox
constexpr HitBox HBOX_BULLET =  { Pos2(1,1), Rect2(0,0,8-1*2, 8-1*2)};
constexpr HitBox HBOX_HEAD =    { Pos2(24,-8), Rect2(0, 0, 24, 32) };
constexpr HitBox HBOX_DORSA =   { Pos2(8,4), Rect2(0, 0, 32, 12) };
constexpr HitBox HBOX_ANAL =    { Pos2(8,0), Rect2(0, 0, 32, 16) };
constexpr HitBox HBOX_BODY[] =
{
    { Pos2( 40, 16), Rect2(0, 0, 80, 48) },
    { Pos2(120, 24), Rect2(0, 0, 72, 32) },
    { Pos2(88, 0),  Rect2(0, 0, 32, 16) },
};

//
}

//
class KingFossil::Head : public Boss::Partial
{
  public:
    Head(KingFossil* parent) 
            : Boss::Partial(*parent, *parent->sprite(),
                            {
                                -8, 32,
                                {
                                    CellRect(  0, 72, 56, 40),
                                    CellRect( 56, 72, 56, 40),
                                    CellRect(112, 72, 56, 48),
                                    CellRect(168, 72, 56, 56),
                                },
                                {
                                    Seq(Seq::Draw, 0, 12_u8), // 0 
                                    Seq(Seq::Draw, 1,  6_u8), // 12
                                    Seq(Seq::Draw, 2,  6_u8), // 18
                                    Seq(Seq::Draw, 3, 48_u8), // 24
                                    Seq(Seq::Draw, 2,  6_u8), // 72
                                    Seq(Seq::Draw, 1,  6_u8), // 78
                                    Seq(Seq::Draw, 0, 12_u8), // 84
                                    Seq(Seq::Goto, 0_u8), // 96
                                }
                            },
                            1, -1)
    {
        _hbox = HBOX_HEAD;
    }

  protected:
    void onHit(GameObj* o, const Rect2& r) override
    {
        if(o->category() != CATEGORY_BULLET) { return; }

        if(_animation.index() == 3)
        {
            Pos2i spos(r.center().x() - static_cast<Pos2i::pos_type>(pos().x()),
                       r.center().y() - static_cast<Pos2i::pos_type>(pos().y()));
            explode(spos);

            _parent.onHit(o, r);
            if(_parent.hp() <= 0) { disableHit(); }
        }
        else
        {
            reflect(r.center());
        }
    }
};

//
class KingFossil::PelvicFin : public Boss::Partial
{
  public:
    PelvicFin(KingFossil* parent)
            : Boss::Partial(*parent, *parent->sprite(),
                            { 48, 56,
                              {
                                  CellRect(  0, 168, 48, 24),
                                  CellRect( 48, 168, 48, 24),
                                  CellRect( 96, 168, 48, 16),
                              },
                              {
                                  Seq(Seq::Draw, 0, 12_u8),
                                  Seq(Seq::Draw, 1, 12_u8),
                                  Seq(Seq::Draw, 2, 12_u8),
                                  Seq(Seq::Draw, 1, 12_u8),
                                  Seq(Seq::Goto, 0_u8),
                              }
                            },
                            1, -1)
    {}
};

//
class KingFossil::DorsaFin : public Boss::Partial
{
  public:
    DorsaFin(KingFossil* parent)
            : Boss::Partial(*parent, *parent->sprite(),
                            { 88, -16,
                              {
                                  CellRect(0, 200,  64,  32)
                              },
                              { // No animation
                                  Seq(Seq::Draw, 0, 255_u8),
                                  Seq(Seq::Goto, 0_u8),

                              }
                            },
                            1, -1,
                            HP_DORSAFIN,
                            PTS_BOSS_PARTIAL)
    {
        _hbox = HBOX_DORSA;
    }
};

//
class KingFossil::AnalFin : public Boss::Partial
{
  public:
    AnalFin(KingFossil* parent)
            : Boss::Partial(*parent, *parent->sprite(),
                            { 112, 56,
                              {
                                  CellRect(152, 184, 64, 24),
                                  CellRect(152, 128, 64, 24),
                                  CellRect(152, 152, 64, 32),
                              },
                              {
                                  Seq(Seq::Draw, 0, 6_u8),
                                  Seq(Seq::Draw, 1, 6_u8),
                                  Seq(Seq::Draw, 2, 16_u8),
                                  Seq(Seq::Draw, 1, 6_u8),
                                  Seq(Seq::Draw, 0, 6_u8),
                                  Seq(Seq::Goto, 0_u8),
                              }
                            },
                            1, -1,
                            HP_ANALFIN,
                            PTS_BOSS_PARTIAL)
    {
        _hbox = HBOX_ANAL;
    }
};

//
class KingFossil::AdiposeFin : public Boss::Partial
{
  public:
    AdiposeFin(KingFossil* parent)
            : Boss::Partial(*parent, *parent->sprite(),
                            { 124, 0,
                              {
                                  CellRect( 64, 200, 32, 24),
#if 0
                                  CellRect( 96, 200, 32, 24),
                                  CellRect(128, 200, 32, 24),
#endif
                              },
                              {
                                  Seq(Seq::Draw, 0, 2_u8),
#if 0
                                  Seq(Seq::Draw, 1, 4_u8),
                                  Seq(Seq::Draw, 2, 4_u8),
                                  Seq(Seq::Draw, 1, 4_u8),
                                  Seq(Seq::Draw, 0, 2_u8),
#endif
                                  Seq(Seq::Goto, 0_u8),
                              }
                            },
                            1, -1)
    {}
};

//
class KingFossil::Tail : public Boss::Partial
{
  public:
    Tail(KingFossil* parent)
            : Boss::Partial(*parent, *parent->sprite(),
                            { 152, 16,
                              {
                                  CellRect(152,  16, 64, 48),
                                  CellRect(  0, 112, 56, 48),
                                  CellRect( 56, 112, 48, 48),
                              },
                              {
                                  Seq(Seq::Draw, 0, 8_u8),
                                  Seq(Seq::Draw, 1, 4_u8),
                                  Seq(Seq::Draw, 2, 8_u8),
                                  Seq(Seq::Draw, 1, 4_u8),
                                  Seq(Seq::Draw, 0, 8_u8),
                                  Seq(Seq::Goto, 0_u8),
                              }
                            },
                            1, -1)
    {}
};

//
KingFossil::Bullet::Bullet(GameObj& parent, GCellSprite4& sprite, float radian, KingFossil& kf)
        : Boss::Bullet(parent, sprite,
                       { -4, 4,
                         {
                             CellRect(176,208, 8, 8),
                             CellRect(184,208, 8, 8),
                             CellRect(192,208, 8, 8),
                             CellRect(200,208, 8, 8),
                             CellRect(208,208, 8, 8),
                             CellRect(216,208, 8, 8),
                         },
                         {
                             Seq(Seq::Draw, 0, 4_u8),
                             Seq(Seq::Draw, 1, 4_u8),
                             Seq(Seq::Draw, 2, 4_u8),
                             Seq(Seq::Draw, 3, 4_u8),
                             Seq(Seq::Draw, 4, 4_u8),
                             Seq(Seq::Draw, 5, 4_u8),
                             Seq(Seq::Goto, 3_u8),
                         }
                       })
        , _kf(kf)
{
    _behavior = new UniformLinearBehavior(*this, VELOCITY, radian);
    assert(_behavior);
    _hbox = HBOX_BULLET;
    move(parent.x() + _animation.rx, parent.y() + _animation.ry);
}
KingFossil::Bullet::~Bullet()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_behavior);
}

void KingFossil::Bullet::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    // Don't delete, return to object pool.
    _kf._bullets.destruct(this);
    GameObj::onUnchain();
}

void KingFossil::Bullet::onExecute(const float delta)
{
    Rect2 r = Rect2(pos(), 8, 8);
    Boss::Bullet::onExecute(delta);
    if(!r.overlaps(FIELD_RECT)) { release(); }
}

//
class KingFossil::Launcher : public Boss::Partial
{
  public:
    Launcher(KingFossil* parent)
            : Boss::Partial(*parent, *parent->sprite(),
                            { 16, 40,
                              {
                                  CellRect(104,120, 32, 16),
                                  CellRect(104,136, 32, 16),
                                  CellRect(104,152, 32, 16)
                              },
                              { // Synchronize with the animation of head.
                                  Seq(Seq::Offset, 0_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 12_u8), // 0
                                  Seq(Seq::Offset, -4_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 12
                                  Seq(Seq::Offset, -8_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 15
                                  Seq(Seq::Offset, -12_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 18
                                  Seq(Seq::Offset, -16_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 21
                                  Seq(Seq::LoopS, 3),
                                  Seq(Seq::Draw, 1, 4_u8), // 24,40,56
                                  Seq(Seq::Draw, 2, 4_u8), // 28,44,60
                                  Seq(Seq::Callback),
                                  Seq(Seq::Draw, 2, 4_u8), // 32,48,64
                                  Seq(Seq::Draw, 0, 4_u8), // 36,52,68
                                  Seq(Seq::LoopE),
                                  Seq(Seq::Offset, -12_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 72
                                  Seq(Seq::Offset, -8_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 75
                                  Seq(Seq::Offset, -4_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 78
                                  Seq(Seq::Offset, -0_i16, 0_i16),
                                  Seq(Seq::Draw, 0, 3_u8), // 81
                                  Seq(Seq::Draw, 0, 12_u8), // 84
                                  Seq(Seq::Goto, 0_u8), // 96
                              }
                            },
                            1, 1)
            , _kf(*parent)
    {
        _animation.sequencer.setCallback(shoot, this);
    }

    static void shoot(void* arg, std::uint8_t cur)
    {
        static_cast<Launcher*>(arg)->_shoot(cur);
    }

    void _shoot(std::uint8_t /*cur*/)
    {
        for(std::int_fast8_t i=0; i < 5; ++i)
        {
            //      auto bullet = new KingFossil::Bullet(*this, _sprite, goblib::math::deg2rad(180.0f - 45.0f + i * 22.5f));
            auto bullet = _kf._bullets.construct(*this, _sprite, goblib::math::deg2rad(180.0f - 45.0f + i * 22.5f),_kf);
            assert(bullet);
            TinyDarius::instance().reserveInsertNode(bullet, this);
        }
    }

  private:
    KingFossil& _kf;
};


//
const char KingFossil::_name[] = "KING FOSSIL";

KingFossil::KingFossil() : Boss(PTS_BOSS_BASE * 1), _bullets(BULLET_MAX)
               
{
    _bodyRect = BODY_RECT;
    _hp = HP_BODY;

    _behavior = new FlutteringBehavior(*this, APPEAR_TO, 32);
}

KingFossil::~KingFossil()
{
    //    TD_PRINT_FUNCTION();
}

bool KingFossil::onInitialize()
{
    // *1 Wait until all children are initialized.
    if(_sprite->getBuffer())
    {
        return std::all_of(_partials.begin(), _partials.end(), [](const Partial* p) { return p->isExecute(); })
               &&  std::all_of(_hits.begin(), _hits.end(), [](const Hit* p) { return p->isExecute(); });
    }

    if(createFromBitmap(*_sprite, BITMAP_PATH))
    {
        _partials.emplace_back(new Head(this));
        _partials.emplace_back(new PelvicFin(this));
        _partials.emplace_back(new DorsaFin(this));
        _partials.emplace_back(new AnalFin(this));
        _partials.emplace_back(new AdiposeFin(this));
        _partials.emplace_back(new Tail(this));
        _partials.emplace_back(new Launcher(this));

        TinyDarius& app = TinyDarius::instance();
        for(auto& e : _partials)
        {
            assert(e);
            app.reserveInsertNode(e, this);
        }

        std::for_each(std::begin(HBOX_BODY), std::end(HBOX_BODY), [this](const HitBox& h)
        {
            auto hb = new Boss::Hit(*this, h);
            assert(hb);
            this->_hits.push_back(hb);
            TinyDarius::instance().reserveInsertNode(hb, this);
        });

        Boss::onInitialize();

        applyPaletteColors(*_sprite, GRAY_SCALE, goblib::size(GRAY_SCALE));
        _apalette->from(*_sprite);
        _sprite->makePaletteTable256();
        return false; // see also *1
    }

    kill();
    return false;
}

bool KingFossil::onRelease()
{
    return Boss::onRelease();
}

void KingFossil::onExecute(const float delta)
{
    Boss::onExecute(delta);
}

void KingFossil::render(void* arg)
{
    Boss::render(arg);
}

void KingFossil::appear()
{
    Boss::_appear(GRAY_SCALE, goblib::size(GRAY_SCALE), APPEAR_TO, APPEAR_TIMES);
}

void KingFossil::defeat()
{
    Boss::_defeat(GRAY_SCALE, goblib::size(GRAY_SCALE), EXPLOSION_TABLE, goblib::size(EXPLOSION_TABLE));
}

void KingFossil::escape()
{
    Boss::_escape(GRAY_SCALE, goblib::size(GRAY_SCALE));
}
