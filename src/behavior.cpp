/*!
  TinyDarius

  @file behavior.cpp
  @brief Behavior for objects
*/
#include "behavior.hpp"
#include "game_obj.hpp"
#include "constants.hpp"

#include <gob_utility.hpp>
#include <gob_math.hpp>
#include <gob_easing.hpp>
#include <cmath>
#include <cstdio>

//
void UniformLinearBehavior::pump(const float delta)
{
    _attach.offset(Pos2(std::cos(_radian) * _velocity, std::sin(_radian) * _velocity));
    Behavior::pump(delta);
}

#if 0
//
void HomingBehavior::pump(const float delta)
{
    Pos2 vpos = _target.pos() - _attach.pos();
    _radian = std::atan2(vpos.y(), vpos.x());

    AccelarationBehavior::pump(delta);
}
#endif

//
void FlutteringBehavior::pump(const float delta)
{
    _attach.move(_bpos.x() + goblib::roundTrip(counter(), _range),
                 _bpos.y() + _range - goblib::roundTrip(counter() + _range, _range * 2));
    Behavior::pump(delta);
}

OrbitalBehavior::OrbitalBehavior(GameObj& attach, fx16 a, fx16 b, Tangle startAngle, fx16 avelocity, VecFx16 velocity, Tangle rotate)
        : Behavior(attach)
        , _pos(VecFx16::pos_type(attach.x()), VecFx16::pos_type(attach.y()))
        , _a(a), _b(b)
        , _angle(startAngle), _avelocity(avelocity), _off(velocity), _rotate(rotate)
{
    //    printf("%s:%d,%d\n", __func__, _pos.x().value<int16_t>(), _pos.y().value<int16_t>());
}

void OrbitalBehavior::pump(const float delta)
{
    fx16 rc = table_cos(_rotate.value<std::int16_t>());
    fx16 rs = table_sin(_rotate.value<std::int16_t>());
    fx16 ac = table_cos(_angle.value<std::int16_t>());
    fx16 as = table_sin(_angle.value<std::int16_t>());
    
    _pos += VecFx16( _a * ac * rc - _b * as * rs,
                     _a * ac * rs + _b * as * rc);
    _pos += _off;
    
    //    printf("%d,%d : %f\n", _pos.x().value<int16_t>(), _pos.y().value<int16_t>(), _angle.value<float>());

    _attach.move(static_cast<Pos2::pos_type>(_pos.x()), static_cast<Pos2::pos_type>(_pos.y()));

    _angle += _avelocity;
}

//
MineBehavior::MineBehavior(GameObj& attach, fx16 a, fx16 b, Tangle startAngle, fx16 avelocity, VecFx16 velocity, Tangle rotate)
        : OrbitalBehavior(attach, a, b, startAngle, avelocity, velocity, rotate)
{}

void MineBehavior::pump(const float delta)
{
    OrbitalBehavior::pump(delta);
    if(_attach.x() < 64) { _off.zero(); }
}
