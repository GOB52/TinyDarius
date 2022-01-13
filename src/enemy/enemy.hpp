/*!
  TinyDarius

  @file enemy.hpp
  @brief Enemies and effect
*/
#pragma once
#ifndef TD_ENEMY_HPP
#define TD_ENEMY_HPP

#include "../typedef.hpp"
#include "../math_table.hpp"
#include "../game_obj.hpp"
#include "../partial_animation.hpp"
#include "../score.hpp"
#include <gob_object_pool.hpp>
#include <gob_utility.hpp>
#include <gob_macro.hpp>

namespace goblib { namespace lgfx { class GCellSprite4; }}
class Behavior;

/*! @brief Enemy base */
class Enemy : public GameObj
{
  public:
    enum Type { Mine, YazukaCW, YazukaCCW, Crash, Reflection, Max };

    Enemy(const Pos2 pos, const PartialAnimation& animation,
          const std::uint32_t priority, const std::uint32_t order, const std::uint32_t category, const std::uint32_t score, const char* tag = "enemy")
            : Enemy(pos.x(), pos.y(), animation, priority, order, category, score, tag) {}
    Enemy(const Pos2::pos_type cx, const Pos2::pos_type cy, const PartialAnimation& animation,
          const std::uint32_t priority, const std::uint32_t order, const std::uint32_t category, const std::uint32_t score, const char* tag = "enemy");
    virtual ~Enemy();

    static void setup();
    static void finalize();

    virtual void render(void* arg) override;
    virtual void onHit(GameObj* o, const Rect2& r) override;

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onReceive (const goblib::TaskMessage& msg) override;
    virtual void onExecute(const float delta) override;

    void defeat();
    GOBLIB_INLINE bool alive() const { return _alive; }
    
  protected:
    PartialAnimation _animation;
    Behavior* _behavior;
    Score _score;
    bool _alive;
    
    static goblib::lgfx::GCellSprite4* _sprite; // shared sprite
    static const PartialAnimation _animation_table[Type::Max];
};

/*! Crash effect (Shared enemy sprite)
 */
class Crash : public Enemy
{
    constexpr static std::int16_t WIDTH = 16;
    constexpr static std::int16_t HEIGHT = 16;

  public:
    Crash(const Pos2& pos) : Crash(pos.x(), pos.y()) {}
    Crash(const Pos2::pos_type cx, const Pos2::pos_type cy);
    Crash(GameObj& attach);
    virtual ~Crash();

  protected:
    virtual void onUnchain() override;
    virtual void onExecute(const float delta) override;

  private:
    GameObj* _attach;
    
};

/*! Reflection effect (Shared enemy sprite)
 */
class Reflection : public Enemy
{
    constexpr static std::int16_t WIDTH = 16;
    constexpr static std::int16_t HEIGHT = 16;

  public:
    Reflection(const Pos2& pos) : Reflection(pos.x(), pos.y()) {}
    Reflection(const Pos2::pos_type cx, const Pos2::pos_type cy);
    virtual ~Reflection();

  protected:
    virtual void onUnchain() override;
};


class MineGenerator;

/*! Floating mine */
class Mine : public Enemy
{
  public:
    Mine(MineGenerator& generator, const Pos2& cpos, const std::int32_t split);
    virtual ~Mine();

    virtual void onHit(GameObj* o, const Rect2& r) override;
    
  protected:
    virtual void onUnchain() override;
    virtual void onExecute(const float delta) override;
    void split();
    
  private:
    MineGenerator& _generator;
    std::int32_t _split;
    constexpr static std::uint32_t SPLIT_TIME = 3_fsec;
};


/*! Generator for Mine */
class MineGenerator
{
  public:
    explicit MineGenerator(goblib::Task& t) : _belongto(t), _pool(POOL_MAX) {}
    void spawn();
    void spawn(const Pos2& pos, const std::int32_t split);
    
  private:
    friend class Mine;
    goblib::Task& _belongto;
    goblib::ObjectPool<Mine> _pool;
    constexpr static std::size_t POOL_MAX = 24;
};

class YazukaGenerator;

/*! Yazuka */
class Yazuka : public Enemy
{
  public:
    Yazuka(YazukaGenerator& generator, const Pos2& cpos, bool cw,
           fx16 a, fx16 b, Tangle start, fx16 av, VecFx16 ov, Tangle rotate, std::int32_t wait);
    virtual ~Yazuka();

  protected:
    virtual void onUnchain() override;
    virtual void onExecute(const float delta) override;

  private:
    YazukaGenerator& _generator;
    std::int32_t _wait;
};

/*! Generator for Yazuka */
class YazukaGenerator
{
  public:
    explicit YazukaGenerator(goblib::Task& t) : _belongto(t), _pool(POOL_MAX), _generated(0) {}
    void reset() { _generated = 0; }
    void spawn();
    
  private:
    friend class Yazuka;
    goblib::Task& _belongto;
    goblib::ObjectPool<Yazuka> _pool;
    std::uint32_t _generated;
    constexpr static std::size_t POOL_MAX = 18;
#ifndef DEBUG
    constexpr static std::uint32_t MEMBER_MAX = 6;
#else
    static const std::uint32_t MEMBER_MAX;
#endif
};

#endif
