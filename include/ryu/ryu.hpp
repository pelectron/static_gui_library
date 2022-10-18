#ifndef RYU_RYU_HPP
#define RYU_RYU_HPP
#include "ryu/common.hpp"
#include "ryu/d2fixed.hpp"
#include "ryu/d2s.hpp"
#include "ryu/f2s.hpp"

namespace ryu {

  template <typename CharT>
  unsigned f2s_buffered_n(float f, CharT* result) noexcept;
  template <typename CharT>
  unsigned d2s_buffered_n(double f, CharT* result) noexcept;
  template <typename CharT>
  unsigned d2fixed_buffered_n(double d, uint32_t precision, CharT* result) noexcept;
  template <typename CharT>
  unsigned d2exp_buffered_n(double d, uint32_t precision, CharT* result) noexcept;

  namespace cx {
    template <typename CharT>
    constexpr unsigned f2s_buffered_n(float f, CharT* result) noexcept {
      return ryu::detail::f2s_buffered_n(f, result, &cx::to_bits);
    }
    template <typename CharT>
    constexpr unsigned d2s_buffered_n(double f, CharT* result) noexcept;
    template <typename CharT>
    constexpr unsigned d2fixed_buffered_n(double d, uint32_t precision, CharT* result) noexcept;
    template <typename CharT>
    constexpr unsigned d2exp_buffered_n(double d, uint32_t precision, CharT* result) noexcept;
  } // namespace cx
} // namespace ryu

#endif /* RYU_RYU_HPP */
