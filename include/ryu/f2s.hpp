#ifndef RYU_F2S_HPP
#define RYU_F2S_HPP

#include <cstdint>

namespace ryu::detail {
  using FloatCastFunction = uint32_t (*)(const float) noexcept;

  struct floating_decimal_32 {
    uint32_t mantissa{0};
    // Decimal exponent's range is -45 to 38
    // inclusive, and can fit in a short if needed.
    int32_t exponent{0};
  };

  template <typename CharT>
  constexpr unsigned
      to_chars(const floating_decimal_32 v, const bool sign, CharT* const result) noexcept;

  constexpr floating_decimal_32 f2d(const uint32_t ieeeMantissa,
                                    const uint32_t ieeeExponent) noexcept;
  template <typename CharT>
  constexpr unsigned f2s_buffered_n(float f, CharT* result, FloatCastFunction bit_cast) noexcept;

} // namespace ryu::detail
#include "impl/f2s_impl.hpp"
#endif /* RYU_F2S_HPP */
