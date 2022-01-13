/*!
  TinyDarius

  @file   version.hpp
  @brief  Semantic Versioning
*/
#pragma once
#ifndef TD_VERSION_HPP
#define TD_VERSION_HPP

#include <gob_macro.hpp>

#define TD_VERSION_MAJOR 0
#define TD_VERSION_MINOR 1
#define TD_VERSION_PATCH 0

/*! @def TD_VERSION_VALUE
  @brief Version value */
#define TD_VERSION_VALUE ((TD_VERSION_MAJOR << 16) | (TD_VERSION_MINOR << 8) | (TD_VERSION_PATCH))
/*! @def TD_VERSION_STRING
  @brief Version string */
#define TD_VERSION_STRING GOBLIB_STRINGIFY(TD_VERSION_MAJOR.TD_VERSION_MINOR.TD_VERSION_PATCH)

#endif
