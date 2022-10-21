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

} // namespace sgl
#endif /* SGL_IMPL_NAME_IMPL_HPP */
