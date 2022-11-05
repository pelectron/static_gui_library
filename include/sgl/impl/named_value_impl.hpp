//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef SGL_IMPL_NAMED_VALUE_IMPL_HPP
#define SGL_IMPL_NAMED_VALUE_IMPL_HPP
#include "../named_value.hpp"

namespace sgl {

  template <typename Name, typename T>
  constexpr NamedValue<Name, T>::NamedValue(name_type name, const value_type& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
      : value_(value) {
    static_cast<void>(name);
  }

  template <typename Name, typename T>
  constexpr NamedValue<Name, T>::NamedValue(name_type name, value_type&& value) noexcept(
      std::is_nothrow_move_constructible_v<T>)
      : value_(std::move(value)) {
    static_cast<void>(name);
  }

  template <typename Name, typename T>
  [[nodiscard]] constexpr sgl::string_view<char> NamedValue<Name, T>::name() const noexcept {
    return Name{}.to_view();
  }

  template <typename Name, typename T>
  constexpr T& NamedValue<Name, T>::value() noexcept {
    return value_;
  }

  template <typename Name, typename T>
  constexpr const T& NamedValue<Name, T>::value() const noexcept {
    return value_;
  }

  template <typename T, char... Chars>
  constexpr NamedValue<sgl::Name<Chars...>, std::decay_t<T>> operator<<=(sgl::Name<Chars...> name,
                                                                         T&& value) {
    return NamedValue(name, value);
  }
} // namespace sgl
#endif /* SGL_IMPL_NAMED_VALUE_IMPL_HPP */
