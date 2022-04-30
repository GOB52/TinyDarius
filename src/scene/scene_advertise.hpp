/*!
  TinyDarius

  @file scene_advertise.hpp
  @brief Advertise
*/
#pragma once
#ifndef TD_SCENE_ADVERTISE_HPP
#define TD_SCENE_ADVERTISE_HPP

#include <gob_scene.hpp>
#include <gob_renderer.hpp>
#include <lgfx/gob_lgfx.hpp>

namespace goblib { namespace lgfx {
class GSprite4;
}}

class SceneAdvertise : public goblib::SceneTask, public goblib::graph::RenderObj2D
{
  public:
    SceneAdvertise();
    virtual ~SceneAdvertise();

    virtual void render(void* arg) override;
    
  protected:
    virtual void onUnchain() override { delete this; }
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;
    
    virtual void onEnterCurrentScene(SceneType pre, bool resume) override;
    virtual void onLeaveCurrentScene(SceneType cur) override;

  private:
    goblib::lgfx::GSprite4* _sprite;
};

#endif
