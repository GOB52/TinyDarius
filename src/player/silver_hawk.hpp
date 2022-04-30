/*!
  TinyDarius

  @file silver_hawk.hpp
  @brief My ship
*/
#pragma once
#ifndef TD_SILVER_HAWK_HPP
#define TD_SILVER_HAWK_HPP

#include "../game_obj.hpp"
#include "../constants.hpp"
#include "../partial_animation.hpp"
#include <gob_object_pool.hpp>
#include <gob_macro.hpp>

#include <type_traits>
#include <cstdio>

namespace goblib { namespace lgfx {
class GCellSprite4;
}}

/*! Silverhawk */
class SilverHawk : public GameObj
{
  public:
    constexpr static std::size_t BULLET_MAX = 4;

    /*! Bullet of shilverhawk */
    class Bullet : public GameObj
    {
      public:
        explicit Bullet(SilverHawk& sh);
        virtual ~Bullet();
    
        virtual void render(void* arg) override;

      protected:
        virtual void onUnchain() override;
        virtual bool onInitialize() override;
        virtual bool onRelease() override;
        //virtual void onReceive (const Task::Message& /*msg*/) override {}
        virtual void onExecute(const float delta) override;
        virtual void onHit(GameObj* o, const Rect2&) override
        {
            if(o->category() & (CATEGORY_BIT_ENEMY | CATEGORY_BIT_BOSS))
            {
                release();
            }
        }

      private:
        SilverHawk& _sh; // shared sprite and return to pool.
    };
    
    SilverHawk();
    virtual ~SilverHawk();
    virtual void render(void* arg) override;

    GOBLIB_INLINE bool alive() const { return _function == &SilverHawk::onPlay; }
    
    /// @name enable input
    /// @{
    GOBLIB_INLINE bool isEnable() const { return _enable; }
    GOBLIB_INLINE void enable(bool b) { _enable = b; }
    GOBLIB_INLINE void enable() { enable(true); }
    GOBLIB_INLINE void disable() { enable(false); }
    /// @}

    /// @name Instructions
    /// @{
    void restart();
    void rewind();
    void dead();
    /// @}
    
  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    //virtual void onReceive (const Task::Message& /*msg*/) override {}
    virtual void onExecute(const float delta) override;

    virtual void onHit(GameObj* o, const Rect2& hit) override { dead(); }
    
  private:
    void onPlay(const float);
    void onRewind(const float);
    void onDead(const float);
    void shoot();

    void moveTo(const Pos2& to);
    
  private:
    using Function = void(SilverHawk::*)(float);
    goblib::lgfx::GCellSprite4* _sprite;
    Function _function;
    PartialAnimation _burner;
    Pos2 _from, _to;
    bool _enable;
    goblib::ObjectPool<Bullet> _bullets;
};

#endif
