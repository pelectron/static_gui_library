/**
 * \file sgl/name.hpp
 * \author Pelé Constam (pelectron1602@gmail.com)
 * \brief This file defines the sgl::Name class, the type trait sgl::is_name_v and the NAME
 * convenience macro.
 * \version 0.1
 * \date 2022-07-21
 */

//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SGL_NAME_HPP
#define SGL_NAME_HPP
#include "sgl/string_view.hpp"

#include <type_traits>

namespace sgl {
  /**
   * \headerfile name.hpp "slg/name.hpp"
   * \brief This class represents a compile time string used by sgl::NamedValue and
   * sgl::NamedTuple. To create a name, there are two ways:
   *
   * \code
   * #include "sgl/named.hpp"
   * // using the NAME macro defined at the bottom of this file
   * constexpr auto name1 = NAME("a string literal");
   * // manually defining it like below, equivalent to 'constexpr auto name2 = NAME("N2");'
   * constexpr Name<'N','2'> name2;
   * \endcode
   *
   * \tparam Chars characters of the name as template parameters
   */
  template <char... Chars>
  struct Name {
    /// static array of characters containing the name.
    static constexpr const char chars[]{Chars..., '\0'};

    /// \brief conversion operator to sgl::string_view<char>
    /// \return sgl::string_view<char>
    [[nodiscard]] constexpr explicit operator sgl::string_view<char>();

    /// \brief get name as sgl::string_view<char>
    /// \return sgl::string_view<char>
    [[nodiscard]] constexpr sgl::string_view<char> to_view() const;
  };

  /// \cond
  namespace detail {
    // primary template for is_name_type
    template <typename>
    struct is_name_type : std::false_type {};

    // specialization for when T is a name type
    template <char... Chars>
    struct is_name_type<Name<Chars...>> : std::true_type {};
  } // namespace detail

  /// \endcond

  /**
   * \brief evaluates to true if T is a name type, i.e. T = Name<Chars...>, where Chars is a
   * parameter pack of char
   * \tparam T type to check
   */
  template <typename T>
  static constexpr bool is_name_type_v = sgl::detail::is_name_type<T>::value;
} // namespace sgl

#include "impl/name_impl.hpp"

/**
 * \brief convenience macro to define compile time string literals used by the NamedTuple for
 * indexing.
 * \details he way it works on a high level is the following:
 * NAME("hello") will turn into a value of type sgl::Name<'h','e','l','l','o'>. That is all there
 * is to it. An example of how to use:
 *
 * \code
 *  constexpr auto name1 = NAME("name"); // equivalent to Name<'n','a','m','e'> name1;
 *
 *  constexpr auto home = NAME("homepage"); // Name<'h','o','m'','e','p','a','g','e'> home;
 * \endcode
 *
 * \param string_literal any valid string literal with character type char
 */
#define NAME(string_literal)                                                                  \
  sgl::detail::compile_time::invoke([] {                                                      \
    struct constexpr_string_type {                                                            \
      const char* Chars = string_literal;                                                     \
    };                                                                                        \
    return sgl::detail::variadic_toolbox::apply_range<                                        \
        sizeof(string_literal) - 1,                                                           \
        sgl::detail::compile_time::string_builder<constexpr_string_type>::produce>::result{}; \
  })

#endif /* SGL_NAME_HPP */
