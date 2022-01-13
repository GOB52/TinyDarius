/*!
TinyDarius

  @file scene_game.cpp
  @brief Game main
*/
#include "scene_game.hpp"
#include "scene_stage.hpp"
#include "../debug.hpp"
#include "../player/silver_hawk.hpp"
#include "../enemy/enemy.hpp"
#include "../effect/explosion.hpp"
#include "../app.hpp"
#include "../game.hpp"

#include <gob_task.hpp>
#include <gob_scene.hpp>
#include <gob_m5s_debug.hpp>

#if 0
#include "../sound.hpp"
#include "../typedef.hpp"
#include "../constants.hpp"

#include "../boss/boss.hpp"
#include "../background/rock_surface.hpp"
#include "../background/wave_spot.hpp"
#include "../background/branch_rock.hpp"
#include "../info/information.hpp"
#include "../info/message.hpp"
#include "../info/name_entry.hpp"
#include "../background/mask.hpp"
#include "../effect/block_mask.hpp"

#include <gob_utility.hpp>
#include <lgfx/gob_lgfx_animated_palette.hpp>


#include <cassert>
#include <cstdio>
#endif

SceneGame::SceneGame()
        : goblib::SceneTask(SCENE_GAME, PRIORITY_GAME, "game")
        , _player(nullptr)
#ifndef NDEBUG
        , _firstStage(nullptr)
#endif
{
    Explosion::setup();
    Enemy::setup();
    _player = new SilverHawk();
    assert(_player);
    _player->hide();
}

#ifndef NDEBUG
SceneGame::SceneGame(const Stage& stage)
        : goblib::SceneTask(SCENE_GAME, PRIORITY_GAME, "game")
        , _player(nullptr)
        , _firstStage(&stage)
{
    Explosion::setup();
    Enemy::setup();
    _player = new SilverHawk();
    assert(_player);
    _player->hide();
}
#endif

SceneGame:: ~SceneGame()
{
    //    TD_PRINT_FUNCTION();
    Enemy::finalize();
    Explosion::finalize();
} 

void SceneGame::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    delete this;
}

bool SceneGame::onInitialize()
{
    TinyDarius& app = TinyDarius::instance();
    app.reserveInsertNode(_player, this);

    auto s =
#ifndef NDEBUG
        new SceneStage(*this, _firstStage ? *_firstStage : app.game().stage(), *_player);
#else
        new SceneStage(*this, app.game().stage(), *_player);
#endif    
    assert(s);
    app.reserveInsertNode(s, this);

    return true;
}

bool SceneGame::onRelease()
{
    return true;
}

void SceneGame::onExecute(const float delta)
{
}

void SceneGame::onReceive (const goblib::TaskMessage& msg)
{
    switch(msg.msg)
    {
    case MSG_ROUND_CLEAR:
        {
            TinyDarius& app = TinyDarius::instance();
            app.game().chooseNext( static_cast<bool>(msg.arg) );
            auto s = new SceneStage(*this, app.game().stage(), *_player, false);
            assert(s);
            app.reserveInsertNode(s, this);
        }
        break;
    case MSG_GAME_OVER:
        popScene();
        TinyDarius::instance().endGame();
        break;
    }


}
