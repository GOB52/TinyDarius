/*!
  TinyDarius

  @file name_entry.hpp
  @brief Name entry
*/
#pragma once
#ifndef TD_NAME_ENTRY_HPP
#define TD_NAME_ENTRY_HPP

#include "../game_obj.hpp"
#include "../constants.hpp"
#include <gob_macro.hpp>
#include <cstring>


class NameEntry : public GameObj
{
  public:
    constexpr static std::int32_t NAME_LENGTH = 3;

    NameEntry(std::int32_t score, std::int32_t rank, std::int16_t sy);
    virtual ~NameEntry();

    GOBLIB_INLINE bool isFinish() const { return _timer <= 0 || _cursor >= NAME_LENGTH; }

    GOBLIB_INLINE const char* name() const { return _name; }
    
    virtual void render(void* arg) override;
    
  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onExecute(const float delta) override;

  private:
    std::int32_t _score;
    std::int32_t _rank;
    std::int32_t _timer;
    std::int32_t _cursor;
    char _name[NAME_LENGTH + 1]; // 3 char + '\0'
    std::int32_t _char;
    std::uint8_t _saveRC[2];
};

#endif
