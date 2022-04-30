/*!
  TinyDarius

  @file game_obj.hpp
  @brief Game object
*/
#pragma once
#ifndef TD_GAME_OBJ_HPP
#define TD_GAME_OBJ_HPP

#include "typedef.hpp"
#include "hit_box.hpp"
#include <gob_macro.hpp>
#include <gob_task.hpp>
#include <gob_renderer.hpp>
#include <gob_fixed_vector.hpp>
#include <cassert>

class GameObj : public goblib::Task, public goblib::graph::RenderObj2D
{
  public:
    using Category = std::uint32_t;
    static constexpr std::size_t COLLIDE_OBJECTS_MAX = 80;
    using CollideObjects = goblib::FixedVector<GameObj*, COLLIDE_OBJECTS_MAX>;
    
    GameObj(goblib::Task::PriorityType pri, goblib::graph::RenderObj2D::OrderType order, Category category, const char* tag = "")
            : goblib::Task(pri, tag), goblib::graph::RenderObj2D(order)
            , _hbox{}, _pos() ,_category(category), _counter(0), _hitable(true)
    {}
    virtual ~GameObj(){}

    virtual void render(void* arg) override;

    GOBLIB_INLINE Pos2::pos_type x() const { return _pos.x(); }
    GOBLIB_INLINE Pos2::pos_type y() const { return _pos.y(); }
    GOBLIB_INLINE Pos2 pos() const { return _pos; }
    GOBLIB_INLINE Category category() const { return _category; }

    GOBLIB_INLINE const HitBox hitBox() const { return _hbox; }
    GOBLIB_INLINE bool hitable() const{ return _hitable; }
    GOBLIB_INLINE void virtual setHitable(bool b) { _hitable = b; }
    GOBLIB_INLINE void enableHit()  { setHitable(true);  }
    GOBLIB_INLINE void disableHit() { setHitable(false); }
    
    GOBLIB_INLINE void move(const Pos2::pos_type& x, const Pos2::pos_type& y)
    {
        _pos.move(x, y);
        _hbox.move(x, y);
    }
    GOBLIB_INLINE void move(const Pos2& to)
    {
        _pos = to;
        _hbox.move(to.x(), to.y());
    }
    GOBLIB_INLINE void offset(const Pos2::pos_type& ox, const Pos2::pos_type& oy)
    {
        _pos.offset(ox, oy);
        //        _hbox.offset(ox, oy);
        _hbox.move(_pos);
    }
    GOBLIB_INLINE void offset(const Pos2& off)
    {
        _pos += off;
        //        _hbox.offset(off);
        _hbox.move(_pos);
    }

    static CollideObjects& collideObjects() { return _collideObjects; }
    static bool checkHit(GameObj* obj);
    
    static void showHitBox(bool b) { _showHitBox = b; }
    static bool isShowHitBox() { return _showHitBox; }
    static void print();
    
  protected:
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    //    virtual void onReceive (const TaskMessage& /*msg*/) override {}
    virtual void onExecute(const float /*delta*/) override { ++_counter; }

    virtual void onChain() override
    {
        if(category())
        {
            assert(_collideObjects.size() < _collideObjects.capacity() && "Too many objects");
            _collideObjects.push_back(this);
        }
    }
    virtual void onUnchain() override
    {
        _collideObjects.erase(std::remove_if(_collideObjects.begin(), _collideObjects.end(),
                                             [this](GameObj* o) { return this == o; }),
                              _collideObjects.end());
    }
    virtual void onHit(GameObj* o, const Rect2& hit) {}

    void resetCounter() { _counter = 0; }
    std::uint32_t counter() const { return _counter; }
    
  private:
    GameObj() = delete;
    GameObj(const GameObj&) = delete;
    GameObj& operator=(const GameObj&) = delete;

  protected:
    HitBox _hbox;

  private:
    Pos2 _pos;
    Category _category;
    std::uint32_t _counter;    
    bool _hitable;

    static CollideObjects _collideObjects;
    static bool _showHitBox;
};

#endif
