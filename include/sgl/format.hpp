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

  /**
    @defgroup formatting Formatting

    This module contains various overloads of sgl::to_chars to format integer, fix point and
    floating point values into a string. Integer and fix point formatting is constexpr by default.
    For constexpr floating point formatting, have a look at sgl::cx::to_chars(). It is not constexpr
    by default, because the bit pattern extraction is very expensive when done at compile time
    instead of a simple memcpy.The floating point formatting is based on ryu, which is the fastest
    floating point to string conversion rouitne to date. You can find out more about ryu
    [here](https://github.com/ulfjack/ryu).

    @note If your system has a different endianness for its integer and floating point types, these
    functions may not work!
    @{
   */

  /**
    This enum is used to specify how to format a numerical value into a buffer in the
    sgl::to_chars() and sgl::cx::to_chars() functions.
  */
  enum class format {
    integer,  ///< rounds to nearest integer and prints that value.
    floating, ///< general/shortest floating point representation i.e. 52.5f is printed as "5.25E1"
              ///< regardless of precision. The sign of the exponent is only printed for negitve
              ///< exponents.
    exponential, ///< exponential in the form of "+-x.yyye+-vv", where there are precision number of
                 ///< digits between the '.' and 'e'. The sign of the exponent is always printed.
    fixed, ///< fixed number of digits after the decimal point and full decimal part, i.e. 52.5 with
           ///< precision 3 is formatted to "52.500".
    hex    ///< binary representation in hex. i.e. 2.0f formats to 0x40000000.
  };

  /// This class represent a format result. It contains an sgl::error value which is set when
  /// an error occured and the number of characters formatted in case of success.
  struct format_result {
    sgl::error ec{sgl::error::no_error}; ///< conversion error result
    size_t     size{0};                  ///< number of characters written
  };

  /**
   formats an integer value into str.
   @param str buffer to format into
   @param len buffer length
   @param value to format.
   @tparam CharT character type
   @tparam T integer type to format.
   */
  template <typename CharT, typename T>
  constexpr sgl::format_result to_chars(CharT* str, size_t len, T value) noexcept;

  /**
   formats value into str with the specified precision and format.
   @note this function is not constexpr. For the constexpr version see sgl::cx::to_chars().
   @param str buffer to format into
   @param len buffer length
   @param value value to format
   @param precision number of digits after the decimal point
   @param fmt output format
   @tparam CharT character type
   */
  template <typename CharT>
  sgl::format_result
      to_chars(CharT* str, size_t len, float value, uint32_t precision, sgl::format fmt) noexcept;

  /**
   formats value into str with the specified precision and format.
   @note this function is not constexpr. For the constexpr version see sgl::cx::to_chars().
   @param str buffer to format into
   @param len buffer length
   @param value value to format
   @param precision number of digits after the decimal point
   @param fmt output format
   @tparam CharT character type
   */
  template <typename CharT>
  sgl::format_result
      to_chars(CharT* str, size_t len, double value, uint32_t precision, sgl::format fmt) noexcept;

  /**
   formats value into str with the specified precision and format.
   @param str buffer to format into
   @param len buffer length
   @param value value to format
   @param precision number of digits after the decimal point
   @param fmt output format
   @tparam CharT character type
   @tparam I number of integer digits
   @tparam F number of fractional digits
   */
  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                    str,
                                        size_t                    len,
                                        sgl::unsigned_fixed<I, F> value,
                                        uint32_t                  precision,
                                        sgl::format               fmt) noexcept;
  /**
   formats value into str with the specified precision and format.
   @param str buffer to format into
   @param len buffer length
   @param value value to format
   @param precision number of digits after the decimal point
   @param fmt output format
   @tparam CharT character type
   @tparam I number of integer digits
   @tparam F number of fractional digits
   */
  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                  str,
                                        size_t                  len,
                                        sgl::signed_fixed<I, F> value,
                                        uint32_t                precision,
                                        sgl::format             fmt) noexcept;

  namespace cx {
    /// @ingroup formatting
    /// @{

    /**
     formats value into str with the specified precision and format.

     @note Only use this function in a constexpr context! The floating point to bit representation
     conversion is expensive when making it constexpr compliant. See sgl::to_chars() for a
     efficient version to use at runtime.

     @param str buffer to format into
     @param len buffer length
     @param value value to format
     @param precision number of digits after the decimal point
     @param fmt output format
     @tparam CharT character type
     */
    template <typename CharT>
    constexpr sgl::format_result
        to_chars(CharT* str, size_t len, float value, uint32_t precision, sgl::format fmt) noexcept;

    /**
     formats value into str with the specified precision and format.

     @note Only use this function in a constexpr context! The floating point to bit representation
     conversion is expensive when making it constexpr compliant. See sgl::to_chars() for a
     efficient version to use at runtime.

     @param str buffer to format into
     @param len buffer length
     @param value value to format
     @param precision number of digits after the decimal point
     @param fmt output format
     @tparam CharT character type
     */
    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          double      value,
                                          uint32_t    precision,
                                          sgl::format fmt) noexcept;
    /// @}
  } // namespace cx

  /// @}

} // namespace sgl

#include "sgl/impl/format_impl.hpp"
#endif /* SGL_FORMAT_HPP */
