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

  /// @brief This enum is used to specify how to format a numerical value into a buffer in the sgl::o_chars() and sgl::cx::to_chars() functions.
  enum class format {
    integer,     ///< format like an integer. For fp values this means roudning.
    floating,    ///< shortest floating point representation, ie 0.3 gets formatted as '0.3'
    exponential, ///< exponential notation, i.e. x.yyyEz.
    fixed,       ///< fixed number of digits after the decimal point
    hex          ///< binary representation in hex.
  };

  /// @brief This class represent a format result. It contains an sgl::error value and the number of
  /// characters formatted.
  struct format_result {
    sgl::error ec{sgl::error::no_error}; ///< conversion error result
    size_t     size{0};                  ///< number of characters written
  };

  /**
   * @brief formats an integer value into str.
   * @param str buffer to format into
   * @param len buffer length
   * @value value to format.
   * @tparam CharT character type
   * @tparam T integer type to format.
   */
  template <typename CharT, typename T>
  constexpr sgl::format_result to_chars(CharT* str, size_t len, T value) noexcept;

  /**
   * @brief formats value into str with the specified precision and format.
   * @note this function is not constexpr. For the constexpr version see sgl::cx::to_chars().
   * @param str buffer to format into
   * @param len buffer length
   * @param value value to format
   * @param precision number of digits after the decimal point
   * @param format output format
   * @tparam CharT character type
   */
  template <typename CharT>
  sgl::format_result to_chars(CharT*      str,
                              size_t      len,
                              float       value,
                              uint32_t    precision,
                              sgl::format format) noexcept;

  /**
   * @brief formats value into str with the specified precision and format.
   * @note this function is not constexpr. For the constexpr version see sgl::cx::to_chars().
   * @param str buffer to format into
   * @param len buffer length
   * @param value value to format
   * @param precision number of digits after the decimal point
   * @param format output format
   * @tparam CharT character type
   */
  template <typename CharT>
  sgl::format_result to_chars(CharT*      str,
                              size_t      len,
                              double      value,
                              uint32_t    precision,
                              sgl::format format) noexcept;

  /**
   * @brief formats value into str with the specified precision and format.
   * @param str buffer to format into
   * @param len buffer length
   * @param value value to format
   * @param precision number of digits after the decimal point
   * @param format output format
   * @tparam CharT character type
   * @tparm I number of integer digits
   * @tparam F number of fractional digits
   */
  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                    str,
                                        size_t                    len,
                                        sgl::unsigned_fixed<I, F> value,
                                        uint32_t                  precision,
                                        sgl::format               format) noexcept;
  /**
   * @brief formats value into str with the specified precision and format.
   * @param str buffer to format into
   * @param len buffer length
   * @param value value to format
   * @param precision number of digits after the decimal point
   * @param format output format
   * @tparam CharT character type
   * @tparm I number of integer digits
   * @tparam F number of fractional digits
   */
  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                  str,
                                        size_t                  len,
                                        sgl::signed_fixed<I, F> value,
                                        uint32_t                precision,
                                        sgl::format             format) noexcept;

  namespace cx {

    /**
     * @brief formats value into str with the specified precision and format.
     * @note Only use this function in a constexpr context! The floating point to bit representation
     * conversion is expensive when making it constexpr compliant. See sgl::to_chars() for a
     * efficient version to use at runtime.
     * @param str buffer to format into
     * @param len buffer length
     * @param value value to format
     * @param precision number of digits after the decimal point
     * @param format output format
     * @tparam CharT character type
     */
    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          float       value,
                                          uint32_t    precision,
                                          sgl::format format) noexcept;

    /**
     * @brief formats value into str with the specified precision and format.
     * @note Only use this function in a constexpr context! The floating point to bit representation
     * conversion is expensive when making it constexpr compliant. See sgl::to_chars() for a
     * efficient version to use at runtime.
     * @param str buffer to format into
     * @param len buffer length
     * @param value value to format
     * @param precision number of digits after the decimal point
     * @param format output format
     * @tparam CharT character type
     */
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
