//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_SMALLEST_TYPE_HPP
#define SGL_SMALLEST_TYPE_HPP
#include "sgl/limits.hpp"

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
#endif /* SGL_SMALLEST_TYPE_HPP */
