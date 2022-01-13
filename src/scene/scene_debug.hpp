/*!
  TinyDarius

  @file scene_debug.hpp
  @brief For debug
*/
#pragma once
#ifndef TD_SCENE_DEBUG_HPP
#define TD_SCENE_DEBUG_HPP

#ifndef NDEBUG

#include <gob_scene.hpp>
#include <gob_renderer.hpp>

class SceneDebug : public goblib::SceneTask, public goblib::graph::RenderObj2D
{
  public:
    SceneDebug();
    virtual ~SceneDebug(){}

    virtual void render(void* arg) override;
    
  protected:
    virtual void onUnchain() override { delete this; }
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

    virtual void onEnterCurrentScene(SceneType pre, bool resume) override;
    virtual void onLeaveCurrentScene(SceneType cur) override;

    void onExecuteStage();
    void onExecuteSound();
    void onExecuteAdvertise();

    using Function = void(SceneDebug::*)(void);
    
  private:
    enum Menu : int32_t { Advertise, Stage, Sound, Max };
    std::uint32_t _cursorY;
    std::int32_t _stageIndex;
    Function _function;
};

#endif
#endif
