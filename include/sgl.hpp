#ifndef SGL_HPP
#define SGL_HPP
#include "sgl_items.hpp"
#include "sgl_menu.hpp"
#include "sgl_page.hpp"
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
  using PageLink_t = PageLink<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
} // namespace sgl
#endif
#endif