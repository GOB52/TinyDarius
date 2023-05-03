/*!
  TinyDarius

  @file utility.cpp
  @brief Utilities
*/
#include "lgfx.hpp"
#include "utility.hpp"
#include "app.hpp"
#include <gob_m5s_sd.hpp>
#include <lgfx/gob_lgfx_animated_palette.hpp>
#include <esp_log.h>
#include <functional>

bool createFromBitmap(LGFX_Sprite& sprite,const char* bitmap_path)
{
    SCOPED_RELEASE_BUS();

    goblib::m5s::File file;
    file.open(bitmap_path, O_READ);
    
    if(!file)
    {
        ESP_LOGE(__func__, "Unable to open file %s\n", bitmap_path);
        return false;
    }

    std::size_t len = file.available();
    if(len > 0)
    {
        auto bmp = new std::uint8_t[len];
        if(!bmp) { return false; }

        file.read(bmp, len);
        sprite.createFromBmp(bmp, len);
        delete[] bmp;
    }
    return true;
}

std::size_t getPaletteColors(RGBColor* dest, std::size_t sz, LGFX_Sprite& sprite)
{
    auto s = sprite.getPalette();
    sz = std::min(sz, sprite.getPaletteCount());
    auto ret = sz;
    while(sz--) { *dest++ = *s++; }

    return ret;
}

namespace
{
using palclr_func = std::function< void(const int_fast8_t, const ColorChange&) >;

void functionPaletteColors(LGFX_Sprite& sprite, const ColorChange* table, const std::size_t size, palclr_func func)
{
    RGBColor* pal = sprite.getPalette();
    auto psz = sprite.getPaletteCount();

    for(std::size_t i = 0; i < size; ++i)
    {
        for(int_fast8_t j = 0; j < psz; ++j)
        {
            if(table[i].original == pal[j])
            {
                func(j, table[i]);
            }
        }
    }
}
//
}

void applyPaletteColors(LGFX_Sprite& sprite, const ColorChange* table, const std::size_t size)
{
    assert(table);
    functionPaletteColors(sprite, table, size, [&sprite](const int_fast8_t idx, const ColorChange& cc)
    {
        sprite.setPaletteColor(idx, cc.toward);
    });
}

void towardPaletteColors(goblib::lgfx::AnimatedPalette& apalette, const ColorChange* table, const std::size_t size, const std::uint32_t times)
{
    assert(table);
    assert(apalette.sprite());
    functionPaletteColors(*apalette.sprite(), table, size, [&apalette, &times](const int_fast8_t idx, const ColorChange& cc)
    {
        apalette.toward(idx, cc.toward, times);
    });
}


void roundTripPaletteColors(goblib::lgfx::AnimatedPalette& apalette, const ColorChange* table, const std::size_t size, const std::uint32_t times)
{
    assert(table);
    assert(apalette.sprite());
    functionPaletteColors(*apalette.sprite(), table, size, [&apalette, &times](const int_fast8_t idx, const ColorChange& cc)
    {
        apalette.roundTrip(idx, cc.toward, times);
    });
}
