/*!
  TinyDarius

  @file information.cpp
  @brief Information on play
*/
#include <LovyanGFX.hpp>

#include "information.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../game.hpp"
#include "../typedef.hpp"
#include "../renderer.hpp"
#include "../utility.hpp"
#include "../constants.hpp"

#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::GSprite;
using goblib::lgfx::GSprite4;
#include <gob_utility.hpp>

#include <cmath> // std::log10
#include <cassert>

namespace
{
constexpr char BITMAP_PATH[] = "/res/td/zone.bmp";

constexpr Pos2i ZONE_POS(160 - 8*7, 0);
constexpr Pos2i ZONE_NAME_POS(SCREEN_WIDTH/2 - 8, 20);
constexpr Pos2i REMAINING_POS(SCREEN_WIDTH/2-56+16+4, 24);
constexpr Pos2i SCORE_POS(8,16);
constexpr Pos2i TIME_POS(SCREEN_WIDTH - (12*8), 16);
}

Information::Information()
        : GameObj(PRIORITY_INFORMATION, ORDER_INFORMATION, CATEGORY_NONE, "information")
        , _sprite(nullptr)
{
    _sprite = new GSprite4();
    assert(_sprite);
}

Information::~Information()
{
    //    TD_PRINT_FUNCTION();
    goblib::safeDelete(_sprite);
}

void Information::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool Information::onInitialize()
{
    if(!createFromBitmap(*_sprite, BITMAP_PATH))
    {
        kill();
        return false;
    }
    _sprite->makePaletteTable256();
    return GameObj::onInitialize();
}

bool Information::onRelease()
{
    return GameObj::onRelease();
}

void Information::onExecute(const float delta)
{
}

void Information::render(void* arg)
{
    static const char* score_tbl[] = {"", "0", "00","000", "0000", "00000", "000000", "0000000", "00000000" };
    Game& game = TinyDarius::instance().game();
    
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    _sprite->pushSpriteTo16(target, ZONE_POS.x(), ZONE_POS.y() + rarg->yorigin);

    target->setTextColor(CLR_WHITE, CLR_BLACK);
    target->setTextSize(2);
    target->setCursor(ZONE_NAME_POS.x(), ZONE_NAME_POS.y() + rarg->yorigin);
    target->printf("%c", game.stage().toChar());
    target->setTextSize(1);

    target->setCursor(REMAINING_POS.x(), REMAINING_POS.y() + rarg->yorigin);
    target->printf("%u", game.remaining());

    auto pts = game.score();
    auto column = (pts == 0) ? 7 : 8 - (static_cast<std::int32_t>(std::log10(static_cast<float>(pts)) + 1));
    target->setTextColor(CLR_GRAY, CLR_BLACK);
    target->drawString(score_tbl[column], SCORE_POS.x(),SCORE_POS.y() + rarg->yorigin);
    target->setTextColor(CLR_WHITE);
    target->setCursor(SCORE_POS.x(),SCORE_POS.y() + rarg->yorigin);
    target->printf("%8u", pts);

    target->setCursor(TIME_POS.x(), TIME_POS.y() + rarg->yorigin);
    auto tm = game.remainingTime();
    if(tm <= 60_fsec) { target->setTextColor(CLR_RED, CLR_BLACK); }
    target->printf("TIME:%02u:%02u", tm/(1_fmin), (tm/APP_FPS) % 60);
    target->setTextColor(CLR_WHITE, CLR_BLACK);
}
