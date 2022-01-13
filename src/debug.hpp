/*!
  TinyDarius

  @file debug.hpp
  @brief For debug
*/
#pragma once
#ifndef TD_DEBUG_HPP
#define TD_DEBUG_HPP

#if !defined(NDEBUG) || defined(DOXYGEN_PROCESS)

#include <cstdio>
#include <gob_profile.hpp>
#include <gob_m5s_clock.hpp>


#define TD_PRINT_FUNCTION() do { printf("%s\n", __PRETTY_FUNCTION__); }while(0)
#define TD_PRINTF(fmt, ...) do { printf((fmt), __VA_ARGS__); }while(0)

#else

#define TD_PRINT_FUNCTION() /* nop */
#define TD_PRINTF(fmt, ...) /* nop */

#endif


#if defined(GOBLIB_ENABLE_PROFILE)
using EspInstrument = goblib::profile::MeasuringInstrument<goblib::m5s::esp_clock, goblib::m5s::esp_clock::duration>;
#define TD_SCOPED_PROFILE(tag) EspInstrument GOBLIB_CONCAT(pf_,__LINE__)((#tag))

#else

#define TD_SCOPED_PROFILE(tag) /* nop */

#endif

//
#endif
