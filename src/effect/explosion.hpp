/*!
  TinyDarius

  @file explosion.hpp
  @brief Explosion
*/
#pragma once
#ifndef TD_EXPLOSION_HPP
#define TD_EXPLOSION_HPP

#include "../game_obj.hpp"
#include "../typedef.hpp"
#include "../partial_animation.hpp"
#include "../constants.hpp"
#include <lgfx/gob_lgfx.hpp>

namespace goblib { namespace lgfx {
class GCellSprite4;
}}

class Explosion : public GameObj
{
  public:
    enum Type : std::int32_t
    {
        Random = -1,
        Type_0,
        Type_1,
        Type_2,
        Max
    };

    static void setup();
    static void finalize();

    Explosion(const Pos2& pos, std::uint32_t order, Type t = Type::Random, GameObj* follow = nullptr, std::int32_t apos = 0, bool repeat = false)
            : Explosion(pos.x(), pos.y(), order, t, follow, apos, repeat) {}
    /*
      @param cx Relative center-left from following GameObj (Absolute if follow is nullptr)
      @param cy Relative center-top  from following GameObj (Absolute if follow is nullptr)
      @param order rendering order
      @param t Animation type
      @param follow GameObj to follow
      @param apos Start sequence position for animation(start random position if negative)
      @param repeat Playing animation loop inifinty?
    */
    Explosion(const Pos2::pos_type cx, const Pos2::pos_type cy, std::uint32_t order, Type t = Type::Random, GameObj* follow = nullptr, std::int32_t apos = 0, bool repeat = false);
    virtual ~Explosion();

    virtual void render(void* arg) override;

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onReceive (const goblib::TaskMessage& msg) override;
    virtual void onExecute(const float delta) override;

    static goblib::lgfx::GCellSprite4* _sprite;;
    static const PartialAnimation _animations[Type::Max];

  private:
    const Pos2 _cpos;
    PartialAnimation _animation;
    const GameObj* _follow;
    const bool _repeat;

    constexpr static std::int32_t WIDTH = 32;
    constexpr static std::int32_t HEIGHT = 32;
};

#endif
