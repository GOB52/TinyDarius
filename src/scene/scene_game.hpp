/*!
  TinyDarius

  @file scene_game.hpp
  @brief Game main
*/
#pragma once
#ifndef TD_SCENE_GAME_HPP
#define TD_SCENE_GAME_HPP

#include <gob_scene.hpp>
#ifndef NDEBUG
#include "../stage.hpp"
#endif

class SilverHawk;

// Management of shared objects at each stage.
class SceneGame : public goblib::SceneTask
{
  public:
    SceneGame();
#ifndef NDEBUG
    explicit SceneGame(const Stage& stage);
#endif
    virtual ~SceneGame();

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onReceive (const goblib::TaskMessage& /*msg*/) override;
    virtual void onExecute(const float delta) override;

  private:
    SilverHawk* _player;
#ifndef NDEBUG
    const Stage* _firstStage;
#endif

    
};

#endif
