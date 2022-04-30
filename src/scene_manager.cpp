/*!
  TinyDarius

  @file scene_manager.cpp
  @brief Scene management
*/
#include "scene_manager.hpp"
#include "constants.hpp"
#include "app.hpp"
#include "game.hpp"
#include "scene/scene_advertise.hpp"
#include "scene/scene_stage.hpp"

void SceneManager::onEnterCurrentScene(goblib::SceneTask::SceneType cur, goblib::SceneTask::SceneType pre)
{
}
    
void SceneManager::onLeaveCurrentScene(goblib::SceneTask::SceneType cur, goblib::SceneTask::SceneType pre)
{
    //    Postmessage!!
#if 0
    //    if(cur == scene_debug) { return; }

    printf("%s: %u <= %u\n", __func__, cur, pre);
    
    if(pre == scene_stage)
    {
        Game& game = TinyDarius::instance().game();
    
        if(game.isGameover())
        {
            auto s = new SceneAdvertise();
            assert(s);
            push(s);
            return;    
        }
    
        auto s = new SceneStage(game.stage());
        assert(s);
        push(s);
    }
#endif
}
