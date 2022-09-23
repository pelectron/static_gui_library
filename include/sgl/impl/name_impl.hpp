#ifndef SGL_NAME_IMPL_HPP
#define SGL_NAME_IMPL_HPP
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
#endif