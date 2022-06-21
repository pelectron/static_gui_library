/**
 * @file sgl_named_tuple_index.hpp
 * @author Pelé Constam (you@domain.com)
 * @brief code taken from
 * https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/.
 * All credits to the compile time wizards over there.
 * @version 0.1
 * @date 2022-06-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SGL_NAMED_TUPLE_INDEX_HPP
#define SGL_NAMED_TUPLE_INDEX_HPP
#include "sgl_string_view.hpp"
namespace sgl {
  template <char... cs>
  struct Name {
    static constexpr const char             chars[]{cs..., '\0'};
    static constexpr sgl::string_view<char> view{chars, sizeof(chars) - 1};
  };

  namespace detail {
    namespace variadic_toolbox {
      template <unsigned count, template <unsigned...> class meta_functor, unsigned... indices>
      struct apply_range {
        typedef typename apply_range<count - 1, meta_functor, count - 1, indices...>::result result;
      };

      template <template <unsigned...> class meta_functor, unsigned... indices>
      struct apply_range<0, meta_functor, indices...> {
        typedef typename meta_functor<indices...>::result result;
      };
    } // namespace variadic_toolbox

    namespace compile_time {
      template <char... str>
      struct string {
        static constexpr const char chars[] = {str..., '\0'};
      };

      template <typename lambda_str_type>
      struct string_builder {
        template <unsigned... indices>
        struct produce {
          typedef sgl::Name<lambda_str_type{}.chars[indices]...> result;
        };
      };
    } // namespace compile_time
  }   // namespace detail

#define IDX(string_literal)                                                               \
  [] {                                                                                        \
    struct constexpr_string_type {                                                            \
      const char* chars = string_literal;                                                     \
    };                                                                                        \
    return sgl::detail::variadic_toolbox::apply_range<                                        \
        sizeof(string_literal) - 1,                                                           \
        typename sgl::detail::compile_time::string_builder<constexpr_string_type>::produce>:: \
        result{};                                                                             \
  }()
} // namespace sgl
#endif