/*!
  TinyDarius

  @file hit_box.hpp
  @brief Collision
*/
#pragma once
#ifndef TD_HIT_BOX_HPP
#define TD_HIT_BOX_HPP

#include "typedef.hpp"
#include <gob_macro.hpp>

struct HitBox
{
    // Pos2i
    Pos2 _rpos; // relative
    Rect2 _box; // absolute

    constexpr HitBox() : HitBox(Pos2(), Rect2()) {}
    constexpr HitBox(const Pos2& pos, const Rect2& r) : _rpos(pos), _box(r) {}
    
    GOBLIB_INLINE void move(const Pos2& pos) { _box.move(_rpos + pos); }
    GOBLIB_INLINE void move(const Pos2::pos_type x, const Pos2::pos_type y)
    {
        move(Pos2(x,y));
    }

    GOBLIB_INLINE void offset(const Pos2::pos_type x, const Pos2::pos_type y)
    {
        _box.offset(static_cast<Rect2::pos_type>(x), static_cast<Rect2::pos_type>(y));
    }
    GOBLIB_INLINE void offset(const Pos2& pos) { offset(pos.x(), pos.y()); }

    GOBLIB_INLINE Rect2 isHit(const HitBox& x) const { return (_box & x._box); }

    void render(void* arg, std::uint32_t clr);
};

GOBLIB_INLINE Rect2 operator&(const HitBox& a, const HitBox& b)
{
    return a.isHit(b);
}

#endif
