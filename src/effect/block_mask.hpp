/*!
  TinyDarius

  @file block_mask.hpp
  @brief Mask by block
*/
#pragma once
#ifndef TD_BLOCK_MASK_HPP
#define TD_BLOCK_MASK_HPP

#include <gob_macro.hpp>
#include "../game_obj.hpp"
#include <lgfx/gob_lgfx.hpp>

/*! @brief Masking by spiral blocks */
class BlockMask : public GameObj
{
  public:
    constexpr static std::int32_t BLOCK_WIDTH = 16;
    constexpr static std::int32_t BLOCK_HEIGHT = 16;
    constexpr static std::int32_t INNER_BLOCK_WIDTH = 4;
    constexpr static std::int32_t INNER_BLOCK_HEIGHT = 4;

    BlockMask();
    virtual~BlockMask();
    virtual void render(void* arg) override;

    GOBLIB_INLINE bool busy() const { return step() < 16; }
    
  protected:
    virtual void onUnchain() override;
    virtual void onExecute(const float delta);


  private:
    GOBLIB_INLINE std::uint32_t step() const { return counter() >> 1; }
    void fill(goblib::lgfx::GSprite* sprite, const std::int32_t x, const std::int32_t y, const std::uint32_t step);
};
#endif
