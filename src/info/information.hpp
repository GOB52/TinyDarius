/*!
  TinyDarius

  @file information.hpp
  @brief Information on play
*/
#pragma once
#ifndef TD_INFORMATION_HPP
#define TD_INFORMATION_HPP

#include "../game_obj.hpp"

namespace goblib { namespace lgfx {
class LGFX_Sprite4;
}}

class Information : public GameObj
{
  public:
    Information();
    virtual ~Information();

    virtual void render(void* arg) override;

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    //virtual void onReceive (const Task::Message& /*msg*/) override {}
    virtual void onExecute(const float delta) override;

  private:
    goblib::lgfx::LGFX_Sprite4* _sprite;

};

#endif
