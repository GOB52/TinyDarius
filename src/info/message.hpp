/*!
  TinyDarius

  @file message.hpp
  @brief Display message
*/
#pragma once
#ifndef TD_MESSAGE_HPP
#define TD_MESSAGE_HPP

#include "../game_obj.hpp"
#include "../constants.hpp"
#include <gob_macro.hpp>
#include <algorithm>
#include <vector>
#include <cstring>

namespace goblib { namespace lgfx { class GSprite4; }}

/*! @brief Centering messages to be displayed one character at a time */
class CenteringMessage : public GameObj
{
  public:
    constexpr static std::size_t MESSAGE_MAX = 3;

    /*! @brief Message displayed one character at a time */
    struct Message
    {
      public:
        constexpr static std::size_t BUFFER_LENGTH = SCREEN_WIDTH/8 + 1;
        char source[BUFFER_LENGTH];
        char dest[BUFFER_LENGTH];
        const char* ps;
        char* pd; // display str.
        std::int16_t x, y;
        std::uint32_t clr;
        std::int32_t counter;
        const std::int32_t wait;

        Message(const char* str, std::int16_t sy, std::uint32_t c, std::uint32_t w = 4);

        GOBLIB_INLINE bool isFinish() const { return !ps || *ps == '\0'; }

        bool pump()
        {
            if(!--counter)
            {
                counter = wait;
                if(ps && *ps && pd) { *pd++ = *ps++; }
            }
            return isFinish();
        }
    };

    CenteringMessage();
    CenteringMessage(const char* source, std::int32_t sy, std::uint32_t clr, std::uint32_t wait = 4);
    virtual ~CenteringMessage();

    GOBLIB_INLINE void insert(const char* str, std::int32_t sy, std::uint32_t c, std::uint32_t w = 4)
    {
        assert(_messages.size() < MESSAGE_MAX);
        _messages.emplace_back(str, sy, c, w);
    }

    GOBLIB_INLINE void clear() { _messages.clear(); }
    GOBLIB_INLINE bool isFinish() const
    {
        return _messages.empty()
                || std::all_of(_messages.begin(), _messages.end(), [](const Message& m) { return m.isFinish(); });
    }
    
    virtual void render(void* arg) override;

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

  private:
    goblib::FixedVector<Message, MESSAGE_MAX> _messages;
};

/*! @brief Blinking centering message */
class BlinkingMessage : public GameObj
{
  public:
    constexpr static std::size_t BUFFER_LENGTH = SCREEN_WIDTH/8 + 1;

    /*!
      @param source message string
      @param sx center position
      @param sy top position
      @param cycle show/hide cycle.
    */
    BlinkingMessage(const char* source, std::int16_t sy, std::uint32_t clr, std::int32_t cycle = 16);
    virtual ~BlinkingMessage();

    virtual void render(void* arg) override;

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

  private:
    goblib::lgfx::GSprite4* _sprite;

    char _buffer[BUFFER_LENGTH];
    const std::uint32_t _clr;
    const std::int32_t _cycle;
    bool _show;
};

/*! @brief Remain bonus message */
class RemainBonusMessage : public GameObj
{
  public:
    constexpr static std::size_t BUFFER_LENGTH = SCREEN_WIDTH/8 + 1;

    RemainBonusMessage(std::int32_t pts, std::int16_t sy, std::uint32_t clr);
    virtual ~RemainBonusMessage();

    virtual void render(void* arg) override;

    void setPts(std::int32_t pts) { _pts = pts; apply(_pts); }
    void increasePts(std::int32_t pts) { _pts += pts; apply(_pts); }
    
  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

  private:
    void apply(std::int32_t pts);
    
  private:
    char _buffer[BUFFER_LENGTH];
    std::int32_t _pts;
    const std::uint32_t _clr;
    constexpr static const char* FORMAT_STR = "%07d POINTS";
};

#endif
