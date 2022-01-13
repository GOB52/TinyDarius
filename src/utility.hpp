/*!
  TinyDarius

  @file utility.hpp
  @brief Utilities
*/
#ifndef TD_UTILITY_HPP
#define TD_UTILITY_HPP

#include <lgfx/gob_lgfx.hpp>
namespace goblib { namespace lgfx { class AnimatedPalette; }}

bool createFromBitmap(goblib::lgfx::GSprite& sprite,const char* bitmap_path);
std::size_t getPaletteColors(RGBColor* dest, std::size_t sz, goblib::lgfx::GSprite& sprite);

/*! @brief for partial color change */
struct ColorChange
{
    RGBColor original;
    RGBColor toward;   
};
/*! apply from ColorChange.original to toward */
void applyPaletteColors(goblib::lgfx::GSprite& sprite, const ColorChange* table, const std::size_t size);

/*! animated palette from ColorChange.original to toward */
void towardPaletteColors(goblib::lgfx::AnimatedPalette& apalette, const ColorChange* table, const std::size_t size, const std::uint32_t times);
/*! roundtrip animated palette between ColorChange.original and toward */
void roundTripPaletteColors(goblib::lgfx::AnimatedPalette& apalette, const ColorChange* table, const std::size_t size, const std::uint32_t times);

#endif
