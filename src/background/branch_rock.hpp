/*!
  TinyDarius

  @file branch_rock.hpp
  @brief Rocks for branching that appear when cleared.
*/
#pragma  once
#ifndef TD_BRANCH_ROCK_HPP
#define TD_BRANCH_ROCK_HPP

#include <gob_macro.hpp>
#include "../game_obj.hpp"

namespace goblib { namespace lgfx {
class LGFX_Sprite4;
class AnimatedPalette;
}}

/*! @brief Rock for branching */
class BranchRock : public GameObj
{
  public:
    BranchRock();
    virtual ~BranchRock();

    virtual void render(void* arg) override;

    GOBLIB_INLINE virtual void show(bool b) override { RenderObj2D::show(b); setHitable(b); }
    GOBLIB_INLINE void show() { show(true);  }
    GOBLIB_INLINE void hide() { show(false); }

    GOBLIB_INLINE goblib::lgfx::AnimatedPalette* animatedPalette() { return _apalette; }

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

  protected:
    goblib::lgfx::LGFX_Sprite4* _sprite;
    goblib::lgfx::AnimatedPalette* _apalette;
};

#endif
