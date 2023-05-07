/*!
  TinyDarius

  @file block_mask.cpp
  @brief Mask by block
*/
#include "../lgfx.hpp"
#include "block_mask.hpp"
#include "../debug.hpp"
#include "../renderer.hpp"
#include "../constants.hpp"
#include <lgfx/gob_lgfx.hpp>

namespace
{
/* fill order
   00,01,02,03
   11,12,13,04  
   10,15,14,05  
   09,08,07,06  
*/
constexpr Pos2i MASK_TABLE[16] =
{
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 0, BlockMask::INNER_BLOCK_HEIGHT * 0),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 1, BlockMask::INNER_BLOCK_HEIGHT * 0),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 2, BlockMask::INNER_BLOCK_HEIGHT * 0),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 3, BlockMask::INNER_BLOCK_HEIGHT * 0),
    //
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 3, BlockMask::INNER_BLOCK_HEIGHT * 1),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 3, BlockMask::INNER_BLOCK_HEIGHT * 2),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 3, BlockMask::INNER_BLOCK_HEIGHT * 3),
    //
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 2, BlockMask::INNER_BLOCK_HEIGHT * 3),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 1, BlockMask::INNER_BLOCK_HEIGHT * 3),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 0, BlockMask::INNER_BLOCK_HEIGHT * 3),
    //
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 0, BlockMask::INNER_BLOCK_HEIGHT * 2),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 0, BlockMask::INNER_BLOCK_HEIGHT * 1),
    //
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 1, BlockMask::INNER_BLOCK_HEIGHT * 1),
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 2, BlockMask::INNER_BLOCK_HEIGHT * 1),
    //
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 2, BlockMask::INNER_BLOCK_HEIGHT * 2),
    //
    Pos2i(BlockMask::INNER_BLOCK_WIDTH * 1, BlockMask::INNER_BLOCK_HEIGHT * 2),
};

// If the bit is standing, draw inner block.
constexpr std::uint32_t STEP_BIT[16] =
{
    0xFFFF, 0xFFFE, 0xFFFC,0xFFF8,
    0xFFF0, 0xFFE0, 0xFFC0,0xFF80,
    0xFF00, 0xFE00, 0xFC00,0xF800,
    0xF000, 0xE000, 0xC000,0x8000,
};

    
}
BlockMask::BlockMask()
        : GameObj(PRIORITY_MASK, ORDER_MASK, CATEGORY_NONE, "blockmask")
{
}

BlockMask::~BlockMask()
{
    //    TD_PRINT_FUNCTION();
}

void BlockMask::onUnchain()
{
    //    TD_PRINT_FUNCTION();
    GameObj::onUnchain();
    delete this;
}

void BlockMask::onExecute(const float delta)
{
    if(!busy()) { release(); }
    GameObj::onExecute(delta);
}

void BlockMask::render(void* arg)
{
    RenderArg* rarg = static_cast<RenderArg*>(arg);
    LGFX_Sprite* target = rarg->sprite;
    ScopedClip(*target, FIELD_RECT, rarg->yorigin);

    for(std::int32_t y = 0; y < FIELD_RECT.height(); y += BLOCK_HEIGHT)
    {
        for(std::int32_t x = 0; x < FIELD_RECT.width(); x += BLOCK_WIDTH)
        {
            fill(target, x, y + rarg->yorigin, step());
        }
    }
}

void BlockMask::fill(LGFX_Sprite* sprite, const std::int32_t x, const std::int32_t y, const std::uint32_t step)
{
    if(step >=16) { return; }

    std::uint32_t bit = STEP_BIT[step];

    for(std::int_fast8_t i = 0; i < 16; ++i)
    {
        if(bit & (1UL << i))
        {
            sprite->fillRect(x + MASK_TABLE[i].x(), y + MASK_TABLE[i].y(), INNER_BLOCK_WIDTH, INNER_BLOCK_HEIGHT, CLR_BLACK);
        }
    }
}
