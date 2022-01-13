/*!
  TinyDarius

  @file wave_spot.cpp
  @brief Background wave and spot
*/
#include <LovyanGFX.hpp>

#include "wave_spot.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../renderer.hpp"
#include "../utility.hpp"
#include "../constants.hpp"
#include "../stage.hpp"

#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GSprite;
using goblib::lgfx::GSprite4;
#include <lgfx/gob_lgfx_animated_palette.hpp>
using goblib::lgfx::AnimatedPalette;
#include <gob_utility.hpp>

#include <array>
#include <cassert>

using WSColorChange = std::array<ColorChange, 3>;

namespace
{
constexpr char BITMAP_PATH_0[] = "/res/td/bg0.bmp";
constexpr char BITMAP_PATH_1[] = "/res/td/bg1.bmp";

/* orignal
   RGBColor(48,96,64),
   RGBColor(72,160,104),
   RGBColor(112,224,144)
*/

constexpr WSColorChange wscc_blue =
{
    ColorChange{ RGBColor(48,96,64),    RGBColor(48,66,96) },
    ColorChange{ RGBColor(72,160,104),  RGBColor(72,104,160) },
    ColorChange{ RGBColor(112,224,144), RGBColor(112,153,224) }
};

constexpr WSColorChange wscc_orange =
{
    ColorChange{ RGBColor(48,96,64),    RGBColor(96,66,48) },
    ColorChange{ RGBColor(72,160,104),  RGBColor(160,104,72) },
    ColorChange{ RGBColor(112,224,144), RGBColor(224,153,112), }
};

constexpr WSColorChange wscc_purple =
{
    ColorChange{ RGBColor(48,96,64),    RGBColor(96,48,96) },
    ColorChange{ RGBColor(72,160,104),  RGBColor(160,104,160) },
    ColorChange{ RGBColor(112,224,144), RGBColor(224,144,224) }
};

const WSColorChange wscc_table[Stage::MAX] =
{
    wscc_blue, //A
    //
    wscc_purple, //B
    wscc_orange, //C
    //
    wscc_blue, // D
    wscc_blue, // E
    wscc_blue, // F
    //
    wscc_blue, // G
    wscc_blue, // H
    wscc_blue, // I
    wscc_blue, // J
    //
    wscc_blue, // K
    wscc_blue, // L
    wscc_blue, // M
    wscc_blue, // N
    wscc_blue, // O
    //
    wscc_blue, // P
    wscc_blue, // Q
    wscc_blue, // R
    wscc_blue, // S
    wscc_blue, // T
    wscc_blue, // U
    //
    wscc_blue, // V
    wscc_blue, // W
    wscc_blue, // X
    wscc_blue, // Y
    wscc_blue, // Z
    wscc_blue, // V'
    wscc_blue, // Z'
};

constexpr std::int32_t ROUND_TRIP_TIMES = 180;

constexpr Pos2 VELOCITY = Pos2(4, 0);
constexpr Pos2 INITIAL_POS = Pos2(0, 40);

constexpr std::int32_t WIDTH_0 = 64;
constexpr std::int32_t WIDTH_1 = 96;
constexpr std::int32_t WIDTH_ALL = WIDTH_0 * 3 + WIDTH_1 * 2;
//
}

WaveSpot::WaveSpot()
        : GameObj(PRIORITY_WAVESPOT, ORDER_WAVESPOT, CATEGORY_WALL, "wavespot")
        , _sprites(nullptr)
        , _apalette(nullptr)
        , _opalettes()
{
    _sprites = new GSprite4[SpriteMax];
    assert(_sprites);
    _apalette = new AnimatedPalette(16, _sprites);
    assert(_apalette);

    move(INITIAL_POS);
    _opalettes.fill(RGBColor());
}

WaveSpot:: ~WaveSpot()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_apalette);
    goblib::safeDeleteArray(_sprites);
}

void WaveSpot::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool WaveSpot::onInitialize()
{
    if(!createFromBitmap(_sprites[Sprite_0], BITMAP_PATH_0))
    {
        kill();
        return false;
    }
    if(!createFromBitmap(_sprites[Sprite_1], BITMAP_PATH_1))
    {
        kill();
        return false;
    }

    getPaletteColors(_opalettes.data(), _opalettes.size(), _sprites[Sprite_0]);

    _apalette->from(_sprites[Sprite_0]);
    _sprites[Sprite_0].makePaletteTable256();
    _sprites[Sprite_1].makePaletteTable256();    

    return GameObj::onInitialize();
}
bool WaveSpot::onRelease()
{
    return GameObj::onRelease();
}

void WaveSpot::onExecute(const float delta)
{
    //    move(Pos2::pos_type(static_cast<std::int32_t>(x() + VELOCITY_X)), y());
    // loop
    auto np = pos() + VELOCITY;
    if(np.x() > WIDTH_ALL)
    {
        auto left = static_cast<std::int32_t>(np.x()) % WIDTH_ALL;
        np = Pos2(Pos2::pos_type(left), y());
    }
    move(np);

    offset(VELOCITY);

    if(_apalette->pump())
    {
        _apalette->to(_sprites[Sprite_1]);
        _sprites[Sprite_0].makePaletteTable256();
        _sprites[Sprite_1].makePaletteTable256();    
    }
}

void WaveSpot::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    std::int32_t left = -(static_cast<int32_t>(x()) % WIDTH_ALL);
    std::int32_t top = static_cast<int32_t>(y()) + rarg->yorigin;

    _sprites[Sprite_0].pushSpriteTo16(target, left, top); left += WIDTH_0;
    _sprites[Sprite_1].pushSpriteTo16(target, left, top); left += WIDTH_1;
    _sprites[Sprite_0].pushSpriteTo16(target, left, top); left += WIDTH_0;
    _sprites[Sprite_0].pushSpriteTo16(target, left, top); left += WIDTH_0;
    _sprites[Sprite_1].pushSpriteTo16(target, left, top); left += WIDTH_1;

    _sprites[Sprite_0].pushSpriteTo16(target, left, top); left += WIDTH_0;
    _sprites[Sprite_1].pushSpriteTo16(target, left, top); left += WIDTH_1;
    _sprites[Sprite_0].pushSpriteTo16(target, left, top); left += WIDTH_0;
    _sprites[Sprite_0].pushSpriteTo16(target, left, top); left += WIDTH_0;
    _sprites[Sprite_1].pushSpriteTo16(target, left, top); left += WIDTH_1;
}

void WaveSpot::roundTripPalette(std::uint8_t stageNo)
{
    assert(stageNo <  goblib::size(wscc_table) && "Invalid stageno");

    auto table = &wscc_table[goblib::clamp(static_cast<std::size_t>(stageNo), 0U, goblib::size(wscc_table))];
    roundTripPaletteColors(*_apalette, table->data(), table->size(), ROUND_TRIP_TIMES);
}
