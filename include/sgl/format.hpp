//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_FORMAT_HPP
#define SGL_FORMAT_HPP
#include "sgl/callable.hpp"
#include "sgl/error.hpp"
#include "sgl/fix_point.hpp"

#include <cstdint>
#include <type_traits>

namespace sgl {

  /// \brief format type enum
  enum class format { integer, floating, exponential, fixed, hex };

  /// \brief This class represent a format result. It contains an sgl::error value and the number of
  /// characters formatted.
  struct format_result {
    sgl::error ec;   ///< conversion error result
    size_t     size; ///< number of characters written
  };

  template <typename CharT, typename T>
  constexpr sgl::format_result to_chars(CharT* str, size_t len, T value) noexcept;

  template <typename CharT>
  sgl::format_result to_chars(CharT*      str,
                              size_t      len,
                              float       value,
                              uint32_t    precision,
                              sgl::format format) noexcept;

  template <typename CharT>
  sgl::format_result to_chars(CharT*      str,
                              size_t      len,
                              double      value,
                              uint32_t    precision,
                              sgl::format format) noexcept;

  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                    str,
                                        size_t                    len,
                                        sgl::unsigned_fixed<I, F> value,
                                        uint32_t                  precision,
                                        sgl::format               format) noexcept;

  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                  str,
                                        size_t                  len,
                                        sgl::signed_fixed<I, F> value,
                                        uint32_t                precision,
                                        sgl::format             format) noexcept;

  namespace cx {

    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          float       value,
                                          uint32_t    precision,
                                          sgl::format format) noexcept;

    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          double      value,
                                          uint32_t    precision,
                                          sgl::format format) noexcept;
  } // namespace cx

} // namespace sgl

#include "sgl/impl/format_impl.hpp"
#endif /* SGL_FORMAT_HPP */
