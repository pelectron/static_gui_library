/// \file name.hpp
/// \author Pelé Constam (you@domain.com)
/// \brief This file defines the sgl::Name class, the type trait sgl::is_name_v and the NAME
/// convenience macro.
/// \version 0.1
/// \date 2022-07-21
/// \copyright Copyright (c) 2022

#ifndef SGL_NAME_HPP
#define SGL_NAME_HPP
#include "sgl/string_view.hpp"

#include <type_traits>

namespace sgl {

  /// \brief This class represents a compile time string used by sgl::NamedValue and
  /// sgl::NamedTuple. To create a name, there are two ways:
  ///
  /// \code
  /// #include "sgl/named_value.hpp"
  /// // using the NAME macro defined at the bottom of this file
  /// constexpr auto name1 = NAME("a string literal");
  /// // manually defining it like below, equivalent to 'constexpr auto name2 = NAME("N2");'
  /// constexpr Name<'N','2'> name2;
  /// \endcode
  ///
  /// \tparam Chars characters of the name as template parameters
  template <char... Chars>
  struct Name {
    /// static array of characters containing the name
    static constexpr const char chars[]{Chars..., '\0'};

    /// conversion operator to sgl::string_view<char>
    /// \return sgl::string_view<char>
    [[nodiscard]] constexpr explicit operator sgl::string_view<char>();

    /// get name as string_view
    /// \return sgl::string_view<char>
    [[nodiscard]] constexpr sgl::string_view<char> to_view() const;
  };
} // namespace sgl

/// \cond
// The code in this namespace is used by the NAME macro to generate the appropriate type.
// It is taken from
// https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/
// and slightly modified to evaluate to a sgl::Name.
namespace sgl {
  namespace detail {
    namespace variadic_toolbox {
      template <unsigned count, template <unsigned...> class meta_functor, unsigned... indices>
      struct apply_range {
        using result = typename apply_range<count - 1, meta_functor, count - 1, indices...>::result;
      };

      template <template <unsigned...> class meta_functor, unsigned... indices>
      struct apply_range<0, meta_functor, indices...> {
        using result = typename meta_functor<indices...>::result;
      };
    } // namespace variadic_toolbox

    namespace compile_time {

      template <typename lambda_str_type>
      struct string_builder {
        template <unsigned... indices>
        struct produce {
          using result = sgl::Name<lambda_str_type{}.Chars[indices]...>;
        };
      };
      template <typename F>
      [[nodiscard]] constexpr decltype(auto) invoke(F&& f) {
        return std::forward<F>(f)();
      }
    } // namespace compile_time
    // primary template for is_name_type
    template <typename>
    struct is_name_type : std::false_type {};
    // specialization for when T is a name type
    template <char... Chars>
    struct is_name_type<Name<Chars...>> : std::true_type {};
  } // namespace detail
  /// \endcond

  /// \brief evaluates to true if T is a name type, i.e. T = Name<Chars...>, where Chars is a
  /// parameter pack of char \tparam T type to check
  template <typename T>
  static constexpr bool is_name_type_v = sgl::detail::is_name_type<T>::value;
} // namespace sgl

/// \brief convenience macro to define compile time string literals used by the NamedTuple for
/// indexing.
/// \details he way it works on a high level is the following:
/// NAME("hello") will turn into a value of type sgl::Name<'h','e','l','l','o'>. That is all there
/// is to it. An example of how to use:
///
/// \code
///  constexpr auto Name1 = NAME("name"); // equivalent to Name<'n','a','m','e'> Name1;
///
///  constexpr auto home = NAME("homepage"); // Name<'h','o','m'','e','p','a','g','e'> home;
/// \endcode
///
/// \param string_literal any valid string literal with character type char
#define NAME(string_literal)                                                                  \
  sgl::detail::compile_time::invoke([] {                                                      \
    struct constexpr_string_type {                                                            \
      const char* Chars = string_literal;                                                     \
    };                                                                                        \
    return sgl::detail::variadic_toolbox::apply_range<                                        \
        sizeof(string_literal) - 1,                                                           \
        typename sgl::detail::compile_time::string_builder<constexpr_string_type>::produce>:: \
        result{};                                                                             \
  })

#include "impl/name_impl.hpp"
#endif