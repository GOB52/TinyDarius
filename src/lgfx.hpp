#ifndef TD_LGFX_HPP
#define TD_LGFX_HPP

#if __has_include(<M5GFX.h>)
# pragma message "Using M5GFX"
# include <M5GFX.h>
#else
# pragma message "Using LovyanGFX"
# include <LovyanGFX.h>
#endif

#endif
