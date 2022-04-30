/*!
  TinyDarius

  @file name_entry.cpp
  @brief Name entry
*/
#include <LovyanGFX.hpp>
#include "name_entry.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../renderer.hpp"
#include "../constants.hpp"
#include "../typedef.hpp"

#include <gob_utility.hpp>
#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;
#include <gob_m5s_faces.hpp>
using goblib::m5s::FaceGB;

#include <cstring>

namespace
{
constexpr const char VALID_CHAR[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ. ";
constexpr const char* ENTER_MESSAGE[] =
{
    "ENTER YOUR INITIALS !",
    "RANK      SCORE  NAME",
};
constexpr const char* ENTRY_FORMAT = "PROCO  %4d   %8d             ";
constexpr std::int32_t INPUT_OFFSET_X = 60;
constexpr const char* DEFAULT_NAME = "M5S";
constexpr std::int32_t INPUT_TIME = 60_fsec;
}

NameEntry::NameEntry(std::int32_t score, std::int32_t rank, std::int16_t sy)
        : GameObj(PRIORITY_MESSAGE, ORDER_MESSAGE, CATEGORY_NONE, "inputranking")
        , _score(score)
        , _rank(rank)
        , _timer(INPUT_TIME)
        , _cursor(0)
        , _name{0}
        , _char(0)
        , _saveRC{}
{
    move(Pos2(SCREEN_WIDTH/2, sy));
}

NameEntry::~NameEntry()
{
    //    TD_PRINT_FUNCTION();
}

void NameEntry::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

bool NameEntry::onInitialize()
{
    FaceGB& input = TinyDarius::instance().input();
    _saveRC[0] = input.getRepeatCycle(FaceGB::Button::Left);
    _saveRC[1] = input.getRepeatCycle(FaceGB::Button::Right);
    input.setRepeatCycle(FaceGB::Button::Up, 10);
    input.setRepeatCycle(FaceGB::Button::Down, 10);
    
    return GameObj::onInitialize();
}

bool NameEntry::onRelease()
{
    FaceGB& input = TinyDarius::instance().input();
    input.setRepeatCycle(FaceGB::Button::Left, _saveRC[0]);
    input.setRepeatCycle(FaceGB::Button::Right, _saveRC[1]);
    return GameObj::onRelease();
}

void NameEntry::onExecute(const float delta)
{
    // Cut off when time is over.
    if(_timer <= 0)
    {
        _name[_cursor] = '\0';
        // Unentered?
        if(_name[0] == '\0')
        {
            std::strncpy(_name, DEFAULT_NAME, sizeof(_name));
            _name[sizeof(_name)-1] = '\0';
        }
        pause(true);
    }

    FaceGB& input = TinyDarius::instance().input();

    // Change character
    if(input.wasRepeated(FaceGB::Button::Left))
    {
        _char = (_char - 1 + goblib::size(VALID_CHAR)) % goblib::size(VALID_CHAR);
    }
    else if(input.wasRepeated(FaceGB::Button::Right))
    {
        _char = (_char + 1) % goblib::size(VALID_CHAR);
    }
    // Delete
    if(input.wasPressed(FaceGB::Button::B))
    {
        if(_cursor > 0)
        {
            _name[--_cursor] = '\0';
            _char = 0;
        }
    }
    // Input
    else if(input.wasPressed(FaceGB::Button::A))
    {
        _name[_cursor++] = VALID_CHAR[_char];
        _char = 0;

        // Done
        if(_cursor >= NAME_LENGTH)
        {
            _name[_cursor] = '\0';
            pause(true);
        }
    }

    --_timer;
    GameObj::onExecute(delta);
}

void NameEntry::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* target = rarg->sprite;
    std::int32_t xx = static_cast<std::int32_t>(x());
    std::int32_t yy = static_cast<std::int32_t>(y());
    
    auto old = target->getTextDatum();
    target->setTextDatum(textdatum_t::middle_center);
    target->setTextColor(CLR_WHITE);

    target->drawString(ENTER_MESSAGE[0], xx, yy + rarg->yorigin);
    target->drawString(ENTER_MESSAGE[1], xx, yy + 16 + rarg->yorigin);

    char tmp[64];
    snprintf(tmp, sizeof(tmp), ENTRY_FORMAT, _rank, _score);
    tmp[sizeof(tmp)-1] = '\0';
    target->drawString(tmp, xx, yy + 32 + rarg->yorigin);

    target->setTextDatum(textdatum_t::middle_left);
    target->drawString(_name, xx + INPUT_OFFSET_X, yy + 32 + rarg->yorigin);

    if(_cursor < NAME_LENGTH)
    {
        snprintf(tmp, sizeof(tmp), "%c", VALID_CHAR[_char]);
        tmp[sizeof(tmp)-1] = '\0';
        target->setTextColor((counter() & 8) ? CLR_RED : CLR_WHITE);
        target->drawString(tmp, xx + INPUT_OFFSET_X + 8 *_cursor, yy + 32 + rarg->yorigin);
    }
    
    target->setTextColor(CLR_WHITE);
    target->setTextDatum(old);
}
