/*!
  TinyDarius

  @file scene_manager.hpp
  @brief Scene management
*/
#pragma once
#ifndef TD_SCENE_MANAGER_HPP
#define TD_SCENE_MANAGER_HPP

#include <gob_scene.hpp>

class SceneManager : public goblib::SceneManageTask
{
  public:
    SceneManager(goblib::TaskTree<goblib::Task>& tree, goblib::Task::PriorityType pri, goblib::Task* parent = nullptr)
            : goblib::SceneManageTask(tree, pri, parent) {}

  protected:
    virtual void onEnterCurrentScene(goblib::SceneTask::SceneType cur, goblib::SceneTask::SceneType pre);
    virtual void onLeaveCurrentScene(goblib::SceneTask::SceneType cur, goblib::SceneTask::SceneType pre);
};

#endif
