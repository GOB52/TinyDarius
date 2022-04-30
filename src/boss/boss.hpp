/*!
  TinyDarius

  @file boss.hpp
  @brief Boss base
*/
#pragma once
#ifndef TD_BOSS_HPP
#define TD_BOSS_HPP

#include "../game_obj.hpp"
#include "../partial_animation.hpp"
#include "../score.hpp"
#include "../hit_box.hpp"
#include <gob_fixed_vector.hpp>
#include <lgfx/gob_lgfx.hpp>
#include <gob_macro.hpp>

namespace goblib { namespace lgfx {
class GCellSprite4;
class AnimatedPalette;
}}
class Behavior;
struct ColorChange;

/*! @brief Boss base class */
class Boss : public GameObj
{
  public:
    enum Status : int32_t
    {
        Nop,
        Appear,
        Prepare,
        Battle,
        Defeat,
        Escape,
        Leave,
    };
    constexpr static std::size_t PARTIAL_MAX = 12;

    /*! @brief Partial parts of Boss */
    class Partial : public GameObj
    {
      public:
        /*!
          @param parent Boss object
          @param sprite Shared sprite
          @param animation animation cell and sequences
          @param relative_priority relative priority with parent
          @param relative_order relative rendering order  with parent
          @param hp Hitpoint
          @param pts Score if breaked
        */
        Partial(Boss& parent, goblib::lgfx::GCellSprite4& sprite, const PartialAnimation& animation,std::int32_t relative_priority, std::int32_t relative_order, std::int32_t hp = 1, std::uint32_t pts = 0);
        virtual ~Partial();

        GOBLIB_INLINE void pauseAnimation(bool b) { _animation.pause(b); }
        GOBLIB_INLINE void pauseAnimation() { pauseAnimation(true); }
        GOBLIB_INLINE void resumeAnimation() { pauseAnimation(false); }
        GOBLIB_INLINE void resetAnimation() { _animation.reset(); }

        virtual void render(void* arg) override;

        GOBLIB_INLINE std::int32_t hp() const { return _hp; }
    
      protected:
        virtual void onUnchain() override;
        virtual bool onInitialize() override;
        virtual bool onRelease() override;
        virtual void onExecute(const float delta) override;

        virtual void onHit(GameObj* o, const Rect2& hit) override;

        void explode(const Pos2& pos);
        void reflect(const Pos2& pos);
    
      protected:
        Boss& _parent;
        goblib::lgfx::GCellSprite4& _sprite;
        PartialAnimation _animation;
        std::int32_t _hp;
        Score _score;
    };

    /*! @brief Bullets of Boss */
    class Bullet : public GameObj
    {
      public:
        /*!
          @param parent Parent object
          @param sprite Shared sprite
          @param animation animation cell and sequences
        */
        Bullet(GameObj& parent, goblib::lgfx::GCellSprite4& sprite, const PartialAnimation& animation);
        virtual ~Bullet();

        virtual void render(void* arg) override;

      protected:
        virtual void onUnchain() override;
        virtual bool onInitialize() override;
        virtual bool onRelease() override;
        virtual void onExecute(const float delta) override;

      protected:
        GameObj& _parent;
        goblib::lgfx::GCellSprite4& _sprite;
        PartialAnimation _animation;
        Behavior* _behavior;
    };

    /*! @brief Hitbox (NO rendering) */
    class Hit : public GameObj
    {
      public:
        Hit(Boss& parent, const HitBox& hbox /* relative */);
        virtual void render(void* arg) override;
      protected:
        virtual void onUnchain() override;
        virtual void onExecute(const float delta) override;
      private:
        Boss& _parent;
        const HitBox _chbox;
    };
    
    /*! @brief Explosions at defeat */
    struct ExplosionSetting
    {
        Pos2 rpos;    // relative position from body
        std::int32_t type;  // Explosion::Type
    };
    
    //
    static Boss* create(std::int8_t stage);
    virtual ~Boss();

    virtual void render(void* arg) override;
    
    /// @name Property
    /// @{
    virtual const char* name() const = 0;
    GOBLIB_INLINE goblib::lgfx::GCellSprite4* sprite() { return _sprite; }
    GOBLIB_INLINE Status status() const { return _status; }
    GOBLIB_INLINE bool isDead() const { return _hp <= 0; }
    GOBLIB_INLINE bool isNop() const { return status() == Status::Nop; }
    /// @}

    virtual void show(bool b) override
    {
        for(auto& e : _partials) { e->show(b && (e->hp() > 0)); }
        RenderObj2D::show(b);
    }
    GOBLIB_INLINE void show() { show(true); }
    GOBLIB_INLINE void hide() { show(false); }

    GOBLIB_INLINE void pause(bool b)
    {
        Task::pause(b, false);
        pausePartials(b);
    }
    GOBLIB_INLINE void pause() { pause(true); }
    GOBLIB_INLINE void resume() { pause(false); }

    GOBLIB_INLINE void virtual setHitable(bool b) override
    {
        GameObj::setHitable(b);
        setHitablePartials(b);
        setHitableHits(b);
    }
    GOBLIB_INLINE void enableHit()  { setHitable(true);  }
    GOBLIB_INLINE void disableHit() { setHitable(false); }
    
    void hideEffect();
    
    GOBLIB_INLINE std::int32_t hp() const { return _hp; }

    /// @name Change status
    /// @{
    virtual void appear() = 0;
    virtual void escape() = 0;
    /// @}

    virtual void onHit(GameObj* o, const Rect2& hit) override;
    
  protected:
    explicit Boss(std::uint32_t pts);

    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

    /// @name function for status
    /// @{
    virtual void onAppear(const float delta);
    virtual void onPrepare(const float delta);
    virtual void onBattle(const float delta);
    virtual void onDefeat(const float delta);
    virtual void onEscape(const float delta);
    virtual void onLeave(const float delta);
    /// @}

    /// @name Change status
    /// @{
    void prepare();
    void battle();
    virtual void defeat() = 0;
    void leave();

    void _appear(const ColorChange* gray, std::size_t gsz, const Pos2& to, std::uint32_t times);
    void _defeat(const ColorChange* gray, std::size_t gsz, const ExplosionSetting* eset, std::size_t esz);
    void _escape(const ColorChange* gray, std::size_t gsz);
    /// @}

    void explode(const ExplosionSetting* table, std::size_t size);

    void pausePartials(bool b)
    {
        for(auto& e : _partials) { e->pauseAnimation(b); }
    }
    GOBLIB_INLINE void pausePartials() { pausePartials(true); }
    GOBLIB_INLINE void resumePartials() { pausePartials(false); }

    GOBLIB_INLINE void setHitablePartials(bool b)
    {
        for(auto& e : _partials) { e->setHitable(b); }
    }
    GOBLIB_INLINE void enableHitPartials() { setHitablePartials(true); }
    GOBLIB_INLINE void disableHitPartials() { setHitablePartials(true); }

    GOBLIB_INLINE void setHitableHits(bool b)
    {
        for(auto& e : _hits) { e->setHitable(b); }
    }
    GOBLIB_INLINE void enableHitHits() { setHitableHits(true); }
    GOBLIB_INLINE void disableHitHits() { setHitableHits(true); }

    
    GOBLIB_INLINE void resetAnimation()
    {
        for(auto& e : _partials) { e->resetAnimation(); }
    }
    
    void moveTo(const Pos2& to, std::int32_t times)
    {
        _from = pos();
        _to = to;
        _to_times = times;
        resetCounter();
    }

    void towardOriginalColor(goblib::lgfx::GCellSprite4& sprite, std::uint32_t times);
    
  private:
    void nop()
    {
        _function = &Boss::onNop;
        _status = Status::Nop;
        resetCounter();
    }
    void onNop(const float) {}

    bool pumpMove();
    
  protected:
    goblib::lgfx::GCellSprite4* _sprite;
    goblib::lgfx::CellRect _bodyRect;
    Behavior* _behavior;
    std::int32_t _hp;
    goblib::FixedVector<Partial*, PARTIAL_MAX> _partials;
    goblib::FixedVector<Hit*, PARTIAL_MAX> _hits;
    goblib::lgfx::AnimatedPalette* _apalette;
    std::array<RGBColor,16> _palettes; // Original palettes.
    
    using pump_function = void(Boss::*)(float);
    pump_function _function;

    Pos2 _from, _to;
    std::int32_t _to_times;
    Status _status;
    Score _score;
};

#endif
