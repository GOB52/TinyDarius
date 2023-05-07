/*!
  TinyDarius

  @file app.hpp
  @brief Application class
*/
#ifndef TD_APP_HPP
#define TD_APP_HPP

#include "typedef.hpp"
#include <gob_macro.hpp>
#include <gob_app.hpp>
#include <gob_singleton.hpp>
#include <array>
#include <gob_m5s_clock.hpp>
#include <lgfx/gob_lgfx.hpp>

namespace goblib
{
class Task;
class SceneTask;
class SceneManageTask;
struct TaskMessage;
template<class T>class TaskTree;
namespace graph
{
class RenderObj2D;
class Renderer2D;
}

namespace m5s
{
class FaceGB;
}
}
class Game;
using TdTaskTree = goblib::TaskTree<goblib::Task>;
using AppClock = goblib::m5s::esp_clock;
//using app_clock = std::chrono::steady_clock;

/*! @brief Application class */
class TinyDarius : public goblib::App<AppClock, APP_FPS, APP_FPS>, goblib::Singleton<TinyDarius>
{
  public:
    using PointerType = std::unique_ptr<TinyDarius>;
    using Singleton<TinyDarius>::instance;
    using Singleton<TinyDarius>::create;

    virtual ~TinyDarius();

#if __has_include(<M5Unified.h>)
    void setup(M5GFX* output);
#else
    void setup(LGFX* output);
#endif
    void finalize();
    
    virtual void fixedUpdate() override;
    virtual void update(float delta) override;
    virtual void render() override;

    std::uint8_t credits() const { return _credits; }

    void startGame();
    void endGame();
    
    /// @name Bus release.occupancy
    /// @warning Need to release BUS to access SD card.
    // @{
    void endWrite();    //!< Release
    void startWrite();  //!< Occupy
    // @}

    /// @name Any instance
    /// @{
    GOBLIB_INLINE goblib::m5s::FaceGB& input() { return *_input; }
    GOBLIB_INLINE Game& game() { return *_game; }
    GOBLIB_INLINE TdTaskTree& task() { return *_taskTree; }
    /// @}

    /// @name Wrapped
    /// @{
    void reserveInsertNode(goblib::Task* t, goblib::Task* parent = nullptr);
    void sendBroadcastMessage(goblib::TaskMessage& m, goblib::Task* t = nullptr);
    void postBroadcastMessage(goblib::TaskMessage& m, goblib::Task* t = nullptr);

    void insertRenderObj(goblib::graph::RenderObj2D* r);
    void removeRenderObj(goblib::graph::RenderObj2D* r);
    /// @}
    
  protected:
    friend class goblib::Singleton<TinyDarius>;
    TinyDarius();

    virtual void sleep_until(const std::chrono::time_point<AppClock, UpdateDuration>& abs_time) override
    {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(abs_time - AppClock::now()).count();
        auto ms = us > 0 ? us / 1000 : 0;
        delay(ms);
        while(AppClock::now() < abs_time){ taskYIELD(); }
    }
    
  private:
#ifndef NDEBUG
    void printDebugInformation() const;
    void drawDebugInformation(LGFX_Sprite* spr) const;
#endif
    
  private:
#if __has_include (<M5Unified.h>)
    M5GFX* _output;
#else
    LGFX* _output;
#endif
    std::array<LGFX_Sprite*,2> _sprite; // for translate by DMA.
    TdTaskTree* _taskTree;
    goblib::SceneManageTask* _sceneManager;
    goblib::graph::Renderer2D* _renderer;
    goblib::m5s::FaceGB* _input;
    Game* _game;
    std::uint8_t _credits;

    constexpr static std::uint8_t CREDIT_MAX = 9;
    
#ifndef NDEBUG
    mutable std::uint32_t _freeHeap;
    bool _debug;
#endif
    
};

/*! Scoped release and occupay BUS */
class ScopedReleaseBus
{
  public:
    ScopedReleaseBus()  { if(_nest++ == 0) { TinyDarius::instance().endWrite();   }}
    ~ScopedReleaseBus() { if(--_nest == 0) { TinyDarius::instance().startWrite(); }}

  private:
    static std::int32_t _nest;
};
#define SCOPED_RELEASE_BUS() ScopedReleaseBus  GOBLIB_CONCAT(sr_,__LINE__)

#endif
