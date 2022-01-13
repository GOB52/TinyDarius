/*!
  TinyDarius

  @file game.hpp
  @brief Game status and management
*/
#pragma once
#ifndef TD_GAME_HPP
#define TD_GAME_HPP

#include "stage.hpp"
#include "score.hpp"
#include <gob_macro.hpp>
#include <gob_notification.hpp>
#include <cstdint>
#include <array>


class Game : public goblib::Observer<Score>
{
  public:
    constexpr static std::uint32_t INITIAL_REMAINING = 3;
#ifdef DEBUG
    enum : std::int8_t { HISTORY_END = -1 };
#else
    constexpr static std::int8_t HISTORY_END = -1;
#endif
    using History = std::array<std::int8_t, Stage::HISTORY_MAX>;

    Game() : _score(0), _time(0), _stage(HISTORY_END), _index(0), _remaining(INITIAL_REMAINING), _pause(true)
    {
        _history.fill(HISTORY_END);
    }
    ~Game(){}

    /// @name Property
    /// @{
    const Stage& stage() const { return _stage >= 0 ? Stage::table[_stage] : Stage::terminator; }
    const History& history() const { return _history; }

    GOBLIB_INLINE std::uint8_t remaining() const { return _remaining; }
    GOBLIB_INLINE std::int32_t remainingTime() const { return _time; }
    GOBLIB_INLINE std::uint32_t score() const { return _score; }
    /// @}

    GOBLIB_INLINE bool isPause() const { return _pause; }
    GOBLIB_INLINE bool isTimeup() const { return _time <= 0; }
    GOBLIB_INLINE bool isGameover() const { return _remaining <= 0; }
    
    GOBLIB_INLINE void pump() { if(!isPause() && _time > 0) { --_time; } }
    
    /// @name Operation
    /// @{
    void start(std::int32_t sindex = 0)
    {
        assert(sindex >= 0 && sindex < Stage::MAX && "sindex out of range");
        _stage = sindex;
        _score = 0;
        _remaining = INITIAL_REMAINING;
        _index = 0;
        _history.fill(HISTORY_END);
        _history[_index] = _stage;
        _time = stage().time;
        pause(true);
    }

    GOBLIB_INLINE void pause(bool b) { _pause = b; }
    GOBLIB_INLINE void pause()  { pause(true); }
    GOBLIB_INLINE void resume() { pause(false); }

    GOBLIB_INLINE void extend() { if(_remaining < 255) {++_remaining; }}
    GOBLIB_INLINE void dead() { if(_remaining) {--_remaining; } }

    void chooseNext(bool upper)
    {
        auto s = stage();
        if(_index >= Stage::HISTORY_MAX || s.current < 0) { return; }

        auto next = s.next();
        _stage = upper ? next.first : next.second;
        _history[++_index] = _stage;
        _time = stage().time;
        pause(true);
    }
    /// @}

    /// @name Receive notify
    /// @{
    // Score
    GOBLIB_INLINE virtual void onNotify(const Score* s,void* arg) { _score += s->score(); }
    /// @}
    
  private:
    std::uint32_t _score;
    std::int32_t _time;
    std::int8_t _stage; // current stage index
    std::int8_t _index; // history index
    std::uint8_t _remaining;
    std::int8_t _pad[1];
    History _history;
    bool _pause;

    static Stage _stages[Stage::MAX];
};

#endif
