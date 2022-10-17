// Copyright 2019 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#ifndef RYU_S2F_HPP
#define RYU_S2F_HPP

#include "ryu/common.hpp"
#include "ryu/f2s_intrinsics.hpp"
#include "ryu/ryu_parse.hpp"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


namespace ryu {
  namespace cx {
    constexpr uint32_t floor_log2(const uint32_t value) noexcept;
    constexpr float    int32Bits2Float(uint32_t bits) noexcept;
    constexpr Status   s2f_n(const char* buffer, const int len, float* result) noexcept;
  } // namespace cx

  static inline uint32_t floor_log2(const uint32_t value) noexcept;
  // The max function is already defined on Windows.
  constexpr int32_t max32(int32_t a, int32_t b) noexcept;

  static inline float int32Bits2Float(uint32_t bits) noexcept;

  using Log2Function = uint32_t (*)(const uint32_t) noexcept;

  using BitsToFloatFunction = float (*)(const uint32_t) noexcept;

  constexpr Status s2f_n(const char*         buffer,
                         const int           len,
                         float*              result,
                         Log2Function        floor_log2_func,
                         BitsToFloatFunction bit_cast) noexcept;

  inline ryu::Status s2f_n(const char* buffer, const int len, float* result) noexcept;

} // namespace ryu
#include "impl/s2f_impl.hpp"
#endif /* RYU_S2F_HPP */
