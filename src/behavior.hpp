/*!
  TinyDarius

  @file behavior.hpp
  @brief Behavior for objects
*/
#pragma once
#ifndef TD_BEHAVIOR_HPP
#define TD_BEHAVIOR_HPP

#include "typedef.hpp"
#include "math_table.hpp"
#include <gob_macro.hpp>
#include <gob_math.hpp>
#include <gob_ellipse2d.hpp>
#include <gob_vector2d.hpp>
#include <cstdint>
#include <cmath>

class GameObj;


/*! @brief Behavior base */
class Behavior
{
  public:
    explicit Behavior(GameObj& attach) : _attach(attach), _counter(0) {}
    virtual ~Behavior(){}

    GOBLIB_INLINE virtual void pump(const float /*delta*/){ ++_counter; }
    void reset() { _counter = 0; }

  protected:
    std::uint32_t counter() const { return _counter; }
    
  protected:
    GameObj& _attach;
    
  private:
    std::uint32_t _counter;
};

/*! @brief No movement */
class FixedBehavior : public Behavior
{
  public:
    FixedBehavior(GameObj& attach) : Behavior(attach) {}
};

/*! @brief Uniform linear motion */
class UniformLinearBehavior : public Behavior
{
  public:
    UniformLinearBehavior(GameObj& attach, float velocity, float radian)
            : Behavior(attach), _velocity(velocity), _radian(radian) {}

    virtual void pump(const float delta) override;

  protected:
    float _velocity;
    float _radian;
};

/*! @brief Linear motion with accelaration */
class AccelarationBehavior : public UniformLinearBehavior
{
  public:
    AccelarationBehavior(GameObj& attach, float velocity, float radian, float acc)
            : UniformLinearBehavior(attach, velocity, radian), _acc(acc) {}

    virtual void pump(const float delta) override
    {
        UniformLinearBehavior::pump(delta);
        _velocity += _acc;
    }

  protected:
    float _acc;
};

#if 0
// @brief Homing to target.
class HomingBehavior : public AccelarationBehavior
{
  public:
    HomingBehavior(GameObj& attach, float velocity, float radian, float acc, GameObj& target)
            : AccelarationBehavior(attach, velocity, radian, acc), _target(target) {}

    virtual void pump(const float delta) override;

  protected:
    GameObj& _target;
};
#endif

// @brief Flutter
class FlutteringBehavior : public Behavior
{
  public:
    FlutteringBehavior(GameObj& attach, const Pos2& bpos, std::uint32_t range)
            : Behavior(attach)
            , _bpos(bpos)
            , _range(range)
    {  assert(goblib::math::is_powerof2(range) && "range must be power of 2"); }

    virtual void pump(const float delta) override;

  protected:
    const Pos2 _bpos; // base position
    std::uint32_t _range;
};

// @brief Orbital
class OrbitalBehavior : public Behavior
{
  public:
    /*!
      @param attach target object
      @param a ellipse circumference step a
      @param b ellipse circumference step b
      @param startAngle start angle for calculate circumference 
      @param avelocity angular velocity
      @param offset velocity
      @rotate ellipse rotation angle
    */
    OrbitalBehavior(GameObj& attach, fx16 a, fx16 b, Tangle startAngle, fx16 avelocity, VecFx16 velocity, Tangle rotate = 0);

    virtual void pump(const float delta) override;

  protected:
    VecFx16 _pos;
    fx16 _a, _b; // radius A,B
    fx16 _angle; // calc for elliptice circumference.
    fx16 _avelocity;
    VecFx16 _off;
    fx16 _rotate; // ellipse rotate angle.
};



// @brief for Mine
class MineBehavior : public OrbitalBehavior
{
  public:
    MineBehavior(GameObj& attach, fx16 a, fx16 b, Tangle startAngle, fx16 avelocity, VecFx16 velocity, Tangle rotate = 0);
    virtual void pump(const float delta) override;

  private:
    bool _orbit;
};

#endif
