/*!
  TinyDarius

  @file main.cpp
  @brief Program entry
*/

#include <M5Stack.h>
#ifdef min
#undef min
#endif
#include <LovyanGFX.hpp>

#include <esp_system.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#if __has_include (<esp_idf_version.h>)
#include <esp_idf_version.h>
#else // esp_idf_version.h has been introduced in Arduino 1.0.5 (ESP-IDF3.3)
#define ESP_IDF_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))
#define ESP_IDF_VERSION ESP_IDF_VERSION_VAL(3,2,0)
#endif

#include "app.hpp"
#include "sound.hpp"
#include <gob_version.hpp>
#include <gob_m5s_version.hpp>
#include <gob_m5s_cpu_usage.hpp>
#include <gob_m5s_sd.hpp>

static LGFX lcd;

void setup()
{
    auto presetup_heap = esp_get_free_heap_size();

    // Heap memory increases.
    // We release the memory for BT as it is pre-allocated depending on the environment and configuration.
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_BTDM);
    //    esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

    M5.begin(false /* LCD */, false /* SD */ , true /* Serial */);
    Wire.begin();
    while(!Serial){ delay(10); }

    SdFat& sd = goblib::m5s::SD::instance().sd();
    while(!sd.begin((unsigned)TFCARD_CS_PIN, SD_SCK_MHZ(25))) { delay(10); }

    auto m5setup_heap = esp_get_free_heap_size();
    
    TinyDarius::instance().setup(&lcd);
    (void)SoundSystem::instance();
    goblib::m5s::CpuUsage::setup();

    printf("Free heap before M5.begin: %u\n", presetup_heap);
    printf("Free heap after begin: %u\n", m5setup_heap);
    printf("Free heap after my setup: %u\n", esp_get_free_heap_size());

    printf("ESP-IDF Version %d.%d.%d\n",
           (ESP_IDF_VERSION>>16) & 0xFF, (ESP_IDF_VERSION>>8)&0xFF, ESP_IDF_VERSION & 0xFF);
#ifdef LGFX_USE_V1
    const char* lgfx_v = "v1";
#else
    const char* lgfx_v = "v0";
#endif
    printf("LovyanGFX %s\n", lgfx_v);
    printf("goblib %s:%08xH\n", GOBLIB_VERSION_STRING, GOBLIB_VERSION_VALUE);
    printf("goblib_m5s %s:%08xH\n", GOBLIB_M5S_VERSION_STRING, GOBLIB_M5S_VERSION_VALUE);
}
    
void loop()
{
    TinyDarius::instance().pump();
}
