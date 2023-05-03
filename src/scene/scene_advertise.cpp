/*!
  TinyDarius

  @file scene_advertise.cpp
  @brief Advertise
*/
#include "scene_advertise.hpp"
#include "../app.hpp"
#include "../constants.hpp"
#include "../renderer.hpp"
#include "../version.hpp"
#include "../utility.hpp"
using goblib::SceneTask;

#include <gob_utility.hpp>
#include <lgfx/gob_lgfx_sprite.hpp>
#include <gob_m5s_faces.hpp>
using goblib::m5s::FaceGB;

namespace
{
constexpr  const char* BITMAP_PATH = "res/td/logo.bmp";
}

SceneAdvertise::SceneAdvertise()
        : SceneTask(SCENE_ADVERTISE, PRIORITY_ADVERTISE, "advertise")
        , RenderObj2D(ORDER_ADVERTISEE)
        , _sprite(nullptr)
{
    _sprite = new goblib::lgfx::GCellSprite4();
    assert(_sprite);
}

SceneAdvertise:: ~SceneAdvertise()
{
    goblib::safeDelete(_sprite);
}

bool SceneAdvertise::onInitialize()
{
    if(createFromBitmap(*_sprite, BITMAP_PATH))
    {
        _sprite->makePaletteTable256();
        return true;
    }
    kill();
    return false;
}

bool SceneAdvertise::onRelease()
{
    return true;
}

void SceneAdvertise::onExecute(const float delta)
{
    FaceGB& input = TinyDarius::instance().input();
    if(input.wasPressed(FaceGB::Button::Start) && TinyDarius::instance().credits())
    {
        popScene();
        TinyDarius::instance().startGame();
    }
#ifndef NDEBUG
    else if(input.wasPressed(FaceGB::Button::B))
    {
        popScene();
    }
#endif
}

void SceneAdvertise::onEnterCurrentScene(SceneType pre, bool resume)
{
    TinyDarius::instance().insertRenderObj(this);
}

void SceneAdvertise::onLeaveCurrentScene(SceneType cur)
{
    TinyDarius::instance().removeRenderObj(this);
}

void SceneAdvertise::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;

    _sprite->pushSpriteTo16(target, (SCREEN_WIDTH - _sprite->width()) / 2, 64 + rarg->yorigin, 0);
    
    auto old = target->getTextDatum();

    target->setTextSize(3);
    target->setTextColor(CLR_BLACK); // shadow
    target->drawString("TINY",16+2, 56+2 + rarg->yorigin);
    target->setTextColor(CLR_WHITE);
    target->drawString("TINY",16+0, 56+0 + rarg->yorigin);

    target->setTextSize(1);
    const char m[] = "DEVELOPED ON M5STACK";
    std::size_t slen = goblib::size(m);
    target->drawString(m, (SCREEN_WIDTH - 8*slen)/2, 152 + rarg->yorigin);

    const char v[] = TD_VERSION_STRING;
    slen = goblib::size(v);
    target->drawString(v, (SCREEN_WIDTH - 8*slen)/2, 160 + rarg->yorigin);

    target->setTextDatum(old);
}
