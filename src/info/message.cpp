/*!
  TinyDarius

  @file message.cpp
  @brief Display message
*/
#include "../lgfx.hpp"
#include "message.hpp"
#include "../debug.hpp"
#include "../renderer.hpp"
#include "../constants.hpp"
#if __has_include("../df88.cpp")
#include "../df88.hpp"
#endif
#include <lgfx/gob_lgfx.hpp>
#include <lgfx/gob_lgfx_sprite.hpp>
using goblib::lgfx::LGFX_Sprite4;
#include <cstring>
#include <cctype>
#include <algorithm>
#include <cstdio>

CenteringMessage::Message::Message(const char* str, std::int16_t sy, std::uint32_t c, std::uint32_t w)
        : source{0,}, dest{0}
        , ps(source), pd(dest)
        , x(0), y(sy), clr(c)
        , counter(w), wait(w)
{
    std::strncpy(source, str, sizeof(source));
    source[sizeof(source)-1] = '\0';
    // Keep strlen(dest) equal strlen(source)
    std::fill(std::begin(dest), std::begin(dest) + sizeof(dest), ' ');
    dest[std::strlen(source)] = '\0';

    auto width = std::strlen(str) * 8;
    x = (SCREEN_WIDTH - width) / 2;
}
      
CenteringMessage::CenteringMessage()
        : GameObj(PRIORITY_MESSAGE, ORDER_MESSAGE, CATEGORY_NONE, "message")
        , _messages()
{}
    

CenteringMessage::CenteringMessage(const char* str, std::int32_t sy, std::uint32_t clr, std::uint32_t wait)
        : GameObj(PRIORITY_MESSAGE, ORDER_MESSAGE, CATEGORY_NONE, "message")
        , _messages()
{
    insert(str, sy, clr, wait);
}

CenteringMessage::~CenteringMessage()
{
    //    TD_PRINT_FUNCTION();
}

void CenteringMessage::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();    
    delete this;
}

bool CenteringMessage::onInitialize()
{
    return GameObj::onInitialize();
}

bool CenteringMessage::onRelease()
{
    return GameObj::onRelease();
}

void CenteringMessage::onExecute(const float delta)
{
    for(auto& e : _messages)
    {
        if(!e.pump()) { break; }
    }
    GameObj::onExecute(delta);
}

void CenteringMessage::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;

    auto old = target->getTextDatum();
    //    target->setTextDatum(textdatum_t::middle_center);
    target->setTextDatum(textdatum_t::middle_left);

    for(auto& e : _messages)
    {
        target->setTextColor(e.clr);
        target->drawString(e.dest, e.x, e.y + rarg->yorigin);
    }
    target->setTextDatum(old);
}

//
BlinkingMessage::BlinkingMessage(const char* source, std::int16_t sy, std::uint32_t clr, std::int32_t cycle)
        : GameObj(PRIORITY_MESSAGE, ORDER_MESSAGE, CATEGORY_NONE, "blinkingmsg")
        , _sprite(nullptr)
        , _buffer{0}
        , _clr(clr)
        , _cycle(cycle)
        , _show(false)
{
    std::strncpy(_buffer, source, sizeof(_buffer));
    _buffer[sizeof(_buffer)-1] = '\0';

    _sprite = new LGFX_Sprite4(std::strlen(_buffer)*16, 16);
    assert(_sprite);
#if __has_include("../df88.cpp")
    _sprite->setFont(&df88_gfx_font);
#endif
    _sprite->setPaletteColor(1, _clr); // 0:transparent 1;text color
    _sprite->makePaletteTable256();
    auto xx = (SCREEN_WIDTH - strlen(_buffer) * 16) / 2;

    move(Pos2(xx, sy));
}

BlinkingMessage:: ~BlinkingMessage()
{
    goblib::safeDelete(_sprite);
    //    TD_PRINT_FUNCTION();
}

void BlinkingMessage::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool BlinkingMessage::onInitialize()
{
    _sprite->setTextSize(2); // double size
    _sprite->setTextColor(_clr);
    _sprite->drawString(_buffer, 0, 0);
    return GameObj::onInitialize();
}
    
bool BlinkingMessage::onRelease()
{
    return GameObj::onRelease();
}

void BlinkingMessage::onExecute(const float delta)
{
    _show = (_cycle == 0) ? true : ((counter() % _cycle) == 0 ? !_show : _show);
    show(_show);
    GameObj::onExecute(delta);
}

void BlinkingMessage::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;

    /*
      auto old = target->getTextDatum();
      target->setTextDatum(textdatum_t::middle_center);
      target->setTextSize(2);
      target->setTextColor(_clr);
      target->drawString(_buffer, x(), y() + rarg->yorigin);
      target->setTextSize(1);
      target->setTextDatum(old);
    */

    _sprite->pushSpriteTo16(target, static_cast<std::int32_t>(x()) , static_cast<std::int32_t>(y()) - 8 + rarg->yorigin, 0);

}


//
RemainBonusMessage::RemainBonusMessage(std::int32_t pts, std::int16_t sy, std::uint32_t clr)
        : GameObj(PRIORITY_MESSAGE, ORDER_MESSAGE, CATEGORY_NONE, "bonusmessage")
        , _buffer{0}
        , _pts(pts)
        ,_clr(clr)
{
    apply(_pts);

    move(Pos2(SCREEN_WIDTH/2, sy));
}

RemainBonusMessage:: ~RemainBonusMessage()
{
    //    TD_PRINT_FUNCTION();
}

void RemainBonusMessage::apply(std::int32_t pts)
{
    snprintf(_buffer, sizeof(_buffer), FORMAT_STR, pts);
    _buffer[sizeof(_buffer)-1] = '\0';
}

void RemainBonusMessage::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool RemainBonusMessage::onInitialize()
{
    return GameObj::onInitialize();
}
    
bool RemainBonusMessage::onRelease()
{
    return GameObj::onRelease();
}

void RemainBonusMessage::onExecute(const float delta)
{
    GameObj::onExecute(delta);
}

void RemainBonusMessage::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;

    auto old = target->getTextDatum();
    target->setTextDatum(textdatum_t::middle_center);
    target->setTextSize(2);
    target->setTextColor(_clr);
    target->drawString(_buffer, static_cast<std::int32_t>(x()), static_cast<std::int32_t>(y()) + rarg->yorigin);
    target->setTextSize(1);
    target->setTextDatum(old);
}
