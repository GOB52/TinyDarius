/*!
  TinyDarius

  @file scene_debug.cpp
  @brief For debug
*/
#ifndef NDEBUG

#include <LovyanGFX.hpp>
#include "scene_debug.hpp"
#include "scene_debug_sound.hpp"
#include "scene_game.hpp"
#include "scene_advertise.hpp"
#include "../renderer.hpp"
#include "../app.hpp"
#include "../game.hpp"
#include "../constants.hpp"
#include "../stage.hpp"
#include "../math_table.hpp"

#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;
#include <gob_m5s_faces.hpp>
using goblib::m5s::FaceGB;
#include <gob_m5s_debug.hpp>

#include <gob_template_helper.hpp>
#include <gob_math.hpp>
#include <gob_fixed_point_number.hpp>
#include <gob_ring_buffer.hpp>
#include <gob_vector2d.hpp>


namespace
{
constexpr const char* MENU_TBL[] =
{
    "ADVERTISE(ENTRY ON MASTER BUILD)",
    "STAGE:",
    "SOUND TEST",
};
//
}

SceneDebug::SceneDebug()
        : goblib::SceneTask(SCENE_DEBUG, PRIORITY_DEBUG, "debug")
        , goblib::graph::RenderObj2D(ORDER_DEBUG)
        , _cursorY(0) ,_stageIndex(0), _function(nullptr)
{}

bool SceneDebug::onInitialize()
{
    return true;
}

bool SceneDebug::onRelease()
{
    return true;
}

void SceneDebug::onExecute(const float delta)
{
    Function f_tbl[] =
            {
                &SceneDebug::onExecuteAdvertise,
                &SceneDebug::onExecuteStage,
                &SceneDebug::onExecuteSound,
            };
    
    FaceGB& input = TinyDarius::instance().input();

    if(input.wasPressed(FaceGB::Button::Up))
    {
        _cursorY = (_cursorY + Menu::Max - 1) % Menu::Max;
    }
    else if(input.wasPressed(FaceGB::Button::Down))
    {
        _cursorY = (_cursorY + 1) % Menu::Max;
    }

    _function = f_tbl[_cursorY];
    (this->*_function)();
}

void SceneDebug::onExecuteStage()
{
    FaceGB& input = TinyDarius::instance().input();

    if(input.wasRepeated(FaceGB::Button::Left))
    {
        _stageIndex = (_stageIndex - 1 + Stage::MAX) % Stage::MAX;
    }
    if(input.wasRepeated(FaceGB::Button::Right))
    {
        _stageIndex = (_stageIndex + 1) % Stage::MAX;
    }
    
    if(input.wasPressed(FaceGB::Button::A))
    {
        TinyDarius::instance().game().start(_stageIndex);
        pushScene(new SceneGame( Stage::table[_stageIndex] ) );
    }

}

void SceneDebug::onExecuteSound()
{
    FaceGB& input = TinyDarius::instance().input();

    if(input.wasPressed(FaceGB::Button::A))
    {
        pushScene(new SceneDebugSound() );
    }
}

void SceneDebug::onExecuteAdvertise()
{
    FaceGB& input = TinyDarius::instance().input();

    if(input.wasPressed(FaceGB::Button::A))
    {
        pushScene(new SceneAdvertise() );
    }
}


void SceneDebug::onEnterCurrentScene(SceneType pre, bool resume)
{
    TinyDarius::instance().insertRenderObj(this);    
}

void SceneDebug::onLeaveCurrentScene(SceneType cur)
{
    TinyDarius::instance().removeRenderObj(this);
}

void SceneDebug::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* sprite = rarg->sprite;

    sprite->setTextColor(TFT_WHITE, TFT_BLACK);
    sprite->setCursor(16,32 + rarg->yorigin);
    sprite->printf("DEBUG MENU");

    std::int32_t y = 48 + rarg->yorigin;

    sprite->setCursor(16, y + _cursorY * 8);
    sprite->printf("#");
    
    for(std::int32_t i = 0; i < goblib::size(MENU_TBL); ++i)
    {
        sprite->setCursor(24, y);
        sprite->printf(MENU_TBL[i]);
        sprite->setCursor(24 + 48, y);
        switch(i)
        {
        case Menu::Stage:
            sprite->printf("%c:(%u)", Stage::table[_stageIndex].toChar(), _stageIndex);
            break;
        }
        y += 8;
    }
}

#endif
