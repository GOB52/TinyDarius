/*!
  TinyDarius

  @file main.cpp
  @brief Program entry
*/
#if __has_include (<M5Unified.h>)
# pragma message "[TD] Using M5Unified"
# if TD_ENABLE_DISPLAY_MODULE
#   pragma message "[TD] Enable Display module"
#   include <M5ModuleDisplay.h>
# endif
# include <SdFat.h>
# include <M5Unified.h>
#else
# pragma message "[TD] Using LovyanGFX"
# include <M5stack.h>
//#undef _SD_H_
# ifdef min
#   undef min
# endif
# include <SdFat.h>
#endif

#include "lgfx.hpp"

#ifndef M5UNIFIED_VERSION
static LGFX lcd;
#endif
# ifndef TFCARD_CS_PIN
#   define TFCARD_CS_PIN (4)
# endif

#if defined(TD_ENABLE_SD_UPDATER)
# pragma message "[TD] Using SD-Updater"
# define SDU_NO_AUTODETECT
# define USE_SDFATFS
# define HAS_M5_API
# define SDU_USE_DISPLAY
# define HAS_LGFX
# define SDU_Sprite LGFX_Sprite
# ifdef M5UNIFIED_VERSION
#   define SDU_DISPLAY_TYPE M5GFX*
#   define SDU_DISPLAY_OBJ_PTR &M5.Display
#   define SDU_TouchButton LGFX_Button
# else
#   define SDU_DISPLAY_TYPE LGFX*
#   define SDU_DISPLAY_OBJ_PTR &lcd
# endif
# include <M5StackUpdater.h>
#endif

#include <Wire.h>

#include <esp_idf_version.h>
#include <esp_system.h>
#include <esp_bt.h>
#include <esp_bt_main.h>

#include "app.hpp"
#include "sound.hpp"
#include <gob_version.hpp>
#include <gob_m5s_version.hpp>
#include <gob_m5s_cpu_usage.hpp>
#include <gob_m5s_sd.hpp>

void setup()
{
    auto presetup_heap = esp_get_free_heap_size();

    // Incrase internal heap (Not effective in some environments)
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_BTDM);

#ifdef M5UNIFIED_VERSION
    auto cfg = M5.config();
# if defined(__M5GFX_M5MODULEDISPLAY__)
    cfg.module_display.logical_width = 320;
    cfg.module_display.logical_height = 240;
# endif
    cfg.external_speaker.module_display = true;
    M5.begin(cfg);
    M5.setPrimaryDisplayType(m5::board_t::board_M5ModuleDisplay);
#else    
    M5.begin(false /* LCD */, false /* SD */ , true /* Serial */);
    Wire.begin();
#endif
    while(!Serial){ delay(10); }

#if defined(TD_ENABLE_SD_UPDATER)
    auto SdFatSPIConfig = SdSpiConfig( TFCARD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(25) );
    checkSDUpdater(goblib::m5s::SD::instance().sd(),String(MENU_BIN), 5000, &SdFatSPIConfig);
#endif

    // Re-mount by SdFat
    SdFat& sd = goblib::m5s::SD::instance().sd();
    while(!sd.begin((unsigned)TFCARD_CS_PIN, SD_SCK_MHZ(25))) { delay(5000); }
    
    auto m5setup_heap = esp_get_free_heap_size();

#ifdef M5UNIFIED_VERSION
    TinyDarius::instance().setup(&M5.Display);
#else    
    TinyDarius::instance().setup(&lcd);
#endif
    (void)SoundSystem::instance();
    goblib::m5s::CpuUsage::setup();

    printf("Free heap before M5.begin: %u\n", presetup_heap);
    printf("Free heap after begin: %u\n", m5setup_heap);
    printf("Free heap after my setup: %u\n", esp_get_free_heap_size());

    printf("ESP-IDF Version %d.%d.%d\n",
           (ESP_IDF_VERSION>>16) & 0xFF, (ESP_IDF_VERSION>>8)&0xFF, ESP_IDF_VERSION & 0xFF);
    printf("goblib %s:%08xH\n", GOBLIB_VERSION_STRING, GOBLIB_VERSION_VALUE);
    printf("goblib_m5s %s:%08xH\n", GOBLIB_M5S_VERSION_STRING, GOBLIB_M5S_VERSION_VALUE);
}
    
void loop()
{
    TinyDarius::instance().pump();
}
