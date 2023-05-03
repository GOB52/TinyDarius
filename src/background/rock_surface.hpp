/*!
  TinyDarius

  @file rock_surface.hpp
  @brief upper/lower rock surface
*/
#pragma  once
#ifndef TD_ROCK_SURFACE_HPP
#define TD_ROCK_SURFACE_HPP

#include "../game_obj.hpp"

namespace goblib { namespace lgfx {
class LGFX_Sprite4;
class AnimatedPalette;
}}


class RockSurfaceUpper : public GameObj
{
  public:
    explicit RockSurfaceUpper(std::int32_t left, std::int32_t top);
    virtual ~RockSurfaceUpper();

    virtual void render(void* arg) override;

    enum Velocity : std::uint8_t { Normal, Boss, Max };
    void setVelocity(const Velocity v) { assert(v < Velocity::Max && "Too big"); _velocity = VELOCITY_TABLE[v]; }

    goblib::lgfx::AnimatedPalette* animatedPalette() { return _apalette; }
    
  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    //virtual void onReceive (const Task::Message& /*msg*/) override {}
    virtual void onExecute(const float delta) override;

  protected:
    goblib::lgfx::LGFX_Sprite4* _sprite; // array ptr
    goblib::lgfx::AnimatedPalette* _apalette;
    Pos2 _velocity;

    static const std::array<Pos2, Velocity::Max> VELOCITY_TABLE;
    constexpr static std::int32_t RENDER_WIDTH = 320;
};

class RockSurfaceLower : public RockSurfaceUpper
{
  public:
    explicit RockSurfaceLower(std::int32_t left, std::int32_t top);
  protected:
    virtual bool onInitialize() override;

};

#endif
