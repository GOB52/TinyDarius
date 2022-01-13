/*!
  TinyDarius

  @file king_fossil.hpp
  @brief Coelacanth
*/
#pragma once
#ifndef TD_KING_FOSSIL_HPP
#define TD_KING_FOSSIL_HPP

#include "boss.hpp"
#include <gob_object_pool.hpp>

namespace goblib { namespace lgfx { class GCellSprite4; }}


class KingFossil : public Boss
{
  public:
    class Head;
    class PelvicFin;
    class DorsaFin;
    class AnalFin;
    class AdiposeFin;
    class Tail;
    class Launcher;

    class Bullet : public Boss::Bullet
    {
        constexpr static float VELOCITY = 6.0f;

      public:
        Bullet(GameObj& parent, goblib::lgfx::GCellSprite4& sprite, const float radian, KingFossil& kf);
        ~Bullet();

      protected:
        virtual void onUnchain() override;
        virtual void onExecute(const float delta) override;

      private:
        KingFossil& _kf;
    };

    KingFossil();
    virtual ~KingFossil();

    virtual const char* name() const { return _name; }

    virtual void render(void* arg) override;

  protected:
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    //virtual void onReceive (const Task::Message& /*msg*/) override {}
    virtual void onExecute(const float delta) override;

    virtual void appear() override;
    virtual void escape() override;
    virtual void defeat() override;
    
  private:

    goblib::ObjectPool<Bullet> _bullets; 

    constexpr static std::size_t BULLET_MAX = 15; // 5way * 3times
    static const char _name[];
    //    constexpr static std::int32_t _width = 200;
    //    constexpr static std::int32_t _height = 80;
};
#endif
