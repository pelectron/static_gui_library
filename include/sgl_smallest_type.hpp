#ifndef SGL_SMALLEST_TYPE_HPP
#define SGL_SMALLEST_TYPE_HPP
#include "sgl_limits.hpp"

#include <type_traits>


namespace sgl {
  /// type trait alias to get smallest type needed to store a value of N
  template <size_t N>
  using smallest_type_t = std::conditional_t<
      N <= numeric_limits<uint8_t>::max(),
      uint8_t,
      std::conditional_t<
          N <= numeric_limits<uint16_t>::max(),
          uint16_t,
          std::conditional_t<N <= numeric_limits<uint32_t>::max(), uint32_t, uint64_t>>>;
} // namespace sgl
#endif