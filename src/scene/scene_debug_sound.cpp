/*!
  TinyDarius

  @file scene_debug_sound.cpp
  @brief Sound test
*/
#ifndef NDEBUG

#include <LovyanGFX.hpp>
#include "scene_debug_sound.hpp"
#include "../renderer.hpp"
#include "../app.hpp"
#include "../sound.hpp"
#include <gob_m5s_faces.hpp>
using goblib::m5s::FaceGB;
#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;
#include <gob_utility.hpp>

namespace
{
constexpr const char *BGM_TABLE[BGM::Max] =
{
    "INSERT COIN",
    "INSERT COIN B",
    "CAPTAIN NEO",
    "CHAOS MAIN THEME",
    "COSMIC AIRWAY",
    "INORGANIC BEAT",
    "THE SEA",
    "WARNING",
    "BOSS 1",
    "BOSS 2",
    "BOSS 3",
    "BOSS 4",
    "BOSS 5",
    "BOSS 6",
    "BOSS 7",
    "ROUND CLEAR",
    "ENDING",
    "NAME",
    "GAMEOVER",
};

constexpr const char* MENU_TABLE[] =
{
    "NAME:",
    "GAIN:",
    " RETURN",
};
}

bool SceneDebugSound::onInitialize()
{
    TinyDarius::instance().insertRenderObj(this);
    return true;
}

bool SceneDebugSound::onRelease()
{
    TinyDarius::instance().removeRenderObj(this);
    return true;
}

void SceneDebugSound::onExecute(const float delta)
{
    Function f_tbl[] =
            {
                &SceneDebugSound::onExecuteBgm,
                &SceneDebugSound::onExecuteGain,
                &SceneDebugSound::onExecuteReturn,
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

void SceneDebugSound::onExecuteBgm()
{
    FaceGB& input = TinyDarius::instance().input();

    if(input.wasPressed(FaceGB::Button::Left))
    {
        _bgm = (_bgm + BGM::Max - 1) % BGM::Max;
    }
    else if(input.wasPressed(FaceGB::Button::Right))
    {
        _bgm = (_bgm + 1) % BGM::Max;
    }
    if(input.wasPressedEqual(FaceGB::Button::A))
    {
        SoundSystem::instance().playBgm(static_cast<BGM>(_bgm));
    }
    if(input.wasPressedEqual(FaceGB::Button::B))
    {
        SoundSystem::instance().stopBgm();
    }

}

void SceneDebugSound::onExecuteGain()
{
    FaceGB& input = TinyDarius::instance().input();
    SoundSystem& ss = SoundSystem::instance();
    
    if(input.wasPressed(FaceGB::Button::Left))
    {
        ss.setVolume(ss.volume() - 0.1f);
    }
    else if(input.wasPressed(FaceGB::Button::Right))
    {
        ss.setVolume(ss.volume() + 0.1f);
    }
}

void SceneDebugSound::onExecuteReturn()
{
    FaceGB& input = TinyDarius::instance().input();
    if(input.wasPressed(FaceGB::Button::A))
    {
        SoundSystem::instance().stopBgm();
        popScene();
    }    
}

void SceneDebugSound::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    GSprite* sprite = rarg->sprite;
    SoundSystem& ss = SoundSystem::instance();

    sprite->setTextColor(TFT_WHITE, TFT_BLACK);
    sprite->setCursor(16,32 + rarg->yorigin);
    sprite->printf("SOUND TEST");

    std::int32_t y = 48 + rarg->yorigin;

    sprite->setCursor(16, y + _cursorY * 8);
    sprite->printf("#");
    
    for(std::int32_t i = 0; i < goblib::size(MENU_TABLE); ++i)
    {
        sprite->setCursor(24, y);
        sprite->printf(MENU_TABLE[i]);
        sprite->setCursor(24 + 48, y);
        switch(i)
        {
        case Menu::Bgm:
            sprite->printf(BGM_TABLE[_bgm]);
            break;
        case Menu::Gain:
            {
                float fi, fp;
                fp = std::modf(ss.volume(), &fi);
                sprite->printf("%d.%02d", (int)fi, int(fp*100));
            }
            break;
        }
        y += 8;
    }

    sprite->setCursor(16, 120 + rarg->yorigin);
    sprite->printf("PLAYABLE:%zu AVAILABLE:%zu\n", ss.playable(), ss.available());
}
#endif
