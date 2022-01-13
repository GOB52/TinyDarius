/*!
  TinyDarius

  @file scene_debug_sound.hpp
  @brief Sound test
*/
#pragma once
#ifndef TD_SCENE_DEBUG_SOUND_HPP
#define TD_SCENE_DEBUG_SOUND_HPP

#ifndef NDEBUG

#include "../constants.hpp"
#include <gob_scene.hpp>
#include <gob_renderer.hpp>


class SceneDebugSound : public goblib::SceneTask, public goblib::graph::RenderObj2D
{
  public:
    SceneDebugSound() : goblib::SceneTask(SCENE_DEBUG_SOUND, PRIORITY_DEBUG, "debug_sound")
                      , goblib::graph::RenderObj2D(ORDER_DEBUG)
                      , _bgm(BGM::InsertCoin)
                      , _cursorY(0)
                      , _function(&SceneDebugSound::onExecuteBgm)
    {}
    virtual ~SceneDebugSound(){}

    virtual void render(void* arg) override;
    
  protected:
    virtual void onUnchain() override { printf("%s\n", __PRETTY_FUNCTION__); delete this; }
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

    void onExecuteBgm();
    void onExecuteGain();
    void onExecuteReturn();
    using Function = void(SceneDebugSound::*)(void);
    
  private:
    enum Menu : int32_t { Bgm, Gain, Return, Max };
    std::underlying_type<BGM>::type _bgm;
    std::int32_t _cursorY;
    Function _function;

};

#endif
#endif
