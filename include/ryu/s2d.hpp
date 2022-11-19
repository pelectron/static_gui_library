// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RYU_S2D_HPP
#define RYU_S2D_HPP
#include "ryu/common.hpp"

namespace ryu {
  /**
   * @brief parse a double from buffer.
   * @param buffer string to parse.
   * @param len length of the string.
   * @param result where to store the result.
   * @return status result of the parse.
   */
  [[nodiscard]] inline status s2d_n(const char* buffer, const int len, double* result) noexcept;

  namespace cx {
    /**
     * @brief parse a double from buffer. This function can be called in a constexpr context.
     * @param buffer string to parse.
     * @param len length of the string.
     * @param result where to store the result.
     * @return status result of the parse.
     */
    [[nodiscard]] constexpr status
        s2d_n(const char* buffer, const int len, double* result) noexcept;

  } // namespace cx
} // namespace ryu

#include "ryu/impl/s2d_impl.hpp"
#endif // !RYU_S2D_HPP
