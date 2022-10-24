// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_RYU_HPP
#define RYU_RYU_HPP
#include "ryu/common.hpp"
#include "ryu/d2fixed.hpp"
#include "ryu/d2s.hpp"
#include "ryu/f2s.hpp"
#include "ryu/ryu_parse.hpp"

namespace ryu {

  /**
   * \brief formats f into result with the shortest representation.
   * \tparam CharT character type
   * \param f value to format
   * \param result string to format into
   * \return number of characters written
   */
  template <typename CharT>
  unsigned f2s_buffered_n(float f, CharT* result) noexcept;

  /**
   * \brief formats f into result with the shortest representation.
   * \tparam CharT character type
   * \param f value to format
   * \param result string to format into
   * \return number of characters written
   */
  template <typename CharT>
  unsigned d2s_buffered_n(double f, CharT* result) noexcept;

  /**
   * \brief formats f into result with a specified precision.
   * \tparam CharT character type.
   * \param f value to format.
   * \param precision number of digits after the decimal point.
   * \param result string to format into.
   * \return number of characters written.
   */
  template <typename CharT>
  unsigned d2fixed_buffered_n(double d, uint32_t precision, CharT* result) noexcept;

  /**
   * \brief formats f into result with a specified precision into exponential format, for
   * example 1.25E-10.
   * \tparam CharT character type.
   * \param f value to format.
   * \param precision number of digits after the decimal point.
   * \param result string to format into.
   * \return number of characters written.
   */
  template <typename CharT>
  unsigned d2exp_buffered_n(double d, uint32_t precision, CharT* result) noexcept;

  namespace cx {
    /**
     * \brief constexpr version of ryu::f2s_buffered_n
     * \tparam CharT character type
     * \param f value to format
     * \param result string t
     * \return number of characters written.
     */
    template <typename CharT>
    constexpr unsigned f2s_buffered_n(float f, CharT* result) noexcept;

    /**
     * \brief constexpr version of ryu::d2s_buffered_n
     * \tparam CharT character type
     * \param f value to format
     * \param result string t
     * \return number of characters written.
     */
    template <typename CharT>
    constexpr unsigned d2s_buffered_n(double f, CharT* result) noexcept;

    /**
     * \brief constexpr version of ryu::d2fixed_buffered_n
     * \tparam CharT character type
     * \param f value to format
     * \param precision number of digits after the decimal point
     * \param result string t
     * \return number of characters written.
     */
    template <typename CharT>
    constexpr unsigned d2fixed_buffered_n(double d, uint32_t precision, CharT* result) noexcept;

    /**
     * \brief constexpr version of ryu::d2exp_buffered_n
     * \tparam CharT character type
     * \param f value to format
     * \param precision number of digits after the decimal point
     * \param result string t
     * \return number of characters written.
     */
    template <typename CharT>
    constexpr unsigned d2exp_buffered_n(double d, uint32_t precision, CharT* result) noexcept;
  } // namespace cx
} // namespace ryu
#endif /* RYU_RYU_HPP */
