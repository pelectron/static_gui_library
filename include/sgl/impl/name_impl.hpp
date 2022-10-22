//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef SGL_IMPL_NAME_IMPL_HPP
#define SGL_IMPL_NAME_IMPL_HPP
#include "../name.hpp"

namespace sgl {
  template <char... Chars>
  constexpr Name<Chars...>::operator sgl::string_view<char>() {
    return {chars, sizeof(chars) - 1};
  }

  template <char... Chars>
  constexpr sgl::string_view<char> Name<Chars...>::to_view() const {
    return sgl::string_view<char>(*this);
  }

  /// \cond
  // The code in this namespace is used by the NAME macro to generate the appropriate type.
  // It is taken from
  // https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/
  // and slightly modified to evaluate to a sgl::Name.
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


} // namespace sgl

#endif /* SGL_IMPL_NAME_IMPL_HPP */
