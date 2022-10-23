/**
 * \file sgl.hpp
 * \author Pelé Constam (pelectron1602@gmail.com)
 * \brief 
 * \version 0.1
 * @date 2022-10-23
 * 
*/ 
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_HPP
#define SGL_HPP
#include "sgl/config.h"
#include "sgl/items.hpp"
#include "sgl/menu.hpp"
#include "sgl/page.hpp"
#if SGL_INSTANTIATE == 1
namespace sgl {
  using Bool_t = Boolean<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using Int8_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, int8_t>;
  using UInt8_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, uint8_t>;
  using Int16_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, int16_t>;
  using UInt16_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, uint16_t>;
  using Int32_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, int32_t>;
  using UInt32_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, uint32_t>;
  using Int64_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, int64_t>;
  using UInt64_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, uint64_t>;
  using Float_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, float>;
  using Double_t = Numeric<SGL_LINE_WIDTH, SGL_CHAR_TYPE, double>;
} // namespace sgl
#endif
#endif /* SGL_HPP */
