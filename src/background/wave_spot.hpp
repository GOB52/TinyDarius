/*!
  TinyDarius

  @file wave_spot.hpp
  @brief Background wave and spot
*/
#pragma once
#ifndef TD_WAVE_SPOT_HPP
#define TD_WAVE_SPOT_HPP

#include "../game_obj.hpp"

namespace goblib { namespace lgfx {
class GSprite4;
class AnimatedPalette;
}}

class WaveSpot : public GameObj
{
  public:
    WaveSpot();
    virtual ~WaveSpot();

    virtual void render(void* arg) override;

    goblib::lgfx::AnimatedPalette* animatedPalette() { return _apalette; }
    void roundTripPalette(std::uint8_t stageNo);
    
  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    //virtual void onReceive (const Task::Message& /*msg*/) override {}
    virtual void onExecute(const float delta) override;

  private:
    enum { Sprite_0, Sprite_1, SpriteMax };
    goblib::lgfx::GSprite4* _sprites;
    goblib::lgfx::AnimatedPalette* _apalette;
    std::array<RGBColor,16> _opalettes; // original palette colors.
};

#endif
