// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_RYU_PARSE_HPP
#define RYU_RYU_PARSE_HPP

// This is an experimental implementation of parsing strings to 64-bit floats
// using a Ryu-like algorithm. At this time, it only support up to 17 non-zero
// digits in the input, and also does not support all formats. Use at your own
// risk.
//
// This implementation does not currently support -DRYU_OPTIMIZE_SIZE and always
// compiles against the large lookup tables.

namespace ryu {
  enum class Status { success, input_too_short, input_too_long, malformed_input };

  Status s2d_n(const char* buffer, const int len, double* result) noexcept;
  Status s2d(const char* buffer, double* result) noexcept;

  Status s2f_n(const char* buffer, const int len, float* result) noexcept;
} // namespace ryu

#endif /* RYU_RYU_PARSE_HPP */
