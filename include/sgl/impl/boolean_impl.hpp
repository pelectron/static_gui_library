//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef SGL_IMPL_BOOLEAN_IMPL_HPP
#define SGL_IMPL_BOOLEAN_IMPL_HPP
#include "sgl/boolean.hpp"

namespace sgl {
  /// \cond
  namespace detail {
    template <typename CharT>
    static constexpr CharT True[] = {'T', 'R', 'U', 'E', '\0'};
    template <typename CharT>
    static constexpr CharT False[] = {'F', 'A', 'L', 'S', 'E', '\0'};
  } // namespace detail

  /// \endcond
  Boolean(bool value)->Boolean<sizeof(detail::False<char>) - 1, char>;

  template <typename CharT, size_t NTrue, size_t NFalse>
  Boolean(bool value, const CharT (&true_string)[NTrue], const CharT (&false_string)[NFalse])
      -> Boolean<(NTrue > NFalse) ? NTrue : NFalse, CharT>;

  template <size_t TextSize, typename CharT>
  constexpr Boolean<TextSize, CharT>::Boolean(bool initial_value) noexcept
      : sgl::ItemBase<Boolean<TextSize, CharT>>(initial_value
                                                    ? StringView(sgl::detail::True<CharT>)
                                                    : StringView(sgl::detail::False<CharT>),
                                                &default_handle_input),
        true_string_(sgl::detail::True<CharT>), false_string_(sgl::detail::False<CharT>),
        value_(initial_value) {}

  template <size_t TextSize, typename CharT>
  constexpr Boolean<TextSize, CharT>::Boolean(bool       value,
                                              StringView true_text,
                                              StringView false_text) noexcept
      : sgl::ItemBase<Boolean<TextSize, CharT>>(value ? true_text : false_text,
                                                &default_handle_input),
        true_string_(true_text), false_string_(false_text), value_(value) {}

  template <size_t TextSize, typename CharT>
  constexpr bool Boolean<TextSize, CharT>::get_value() const noexcept {
    return value_;
  }

  template <size_t TextSize, typename CharT>
  constexpr sgl::error Boolean<TextSize, CharT>::set_value(bool value) noexcept {
    value_ = value;
    if (value) {
      this->set_text(true_string());
    } else {
      this->set_text(false_string());
    }
    return sgl::error::no_error;
  }

  template <size_t TextSize, typename CharT>
  constexpr typename Boolean<TextSize, CharT>::StringView
      Boolean<TextSize, CharT>::true_string() const noexcept {
    return true_string_;
  }

  template <size_t TextSize, typename CharT>
  constexpr typename Boolean<TextSize, CharT>::StringView
      Boolean<TextSize, CharT>::false_string() const noexcept {
    return false_string_;
  }

  template <size_t TextSize, typename CharT>
  constexpr sgl::error
      Boolean<TextSize, CharT>::default_handle_input(Boolean<TextSize, CharT>& boolean,
                                                     sgl::input                i) noexcept {
    static_cast<void>(i);
    auto ec = boolean.set_value(!boolean.get_value());
    return ec == sgl::error::no_error ? sgl::error::edit_finished : ec;
  }

  constexpr auto make_bool(bool initial_value) { return sgl::Boolean(initial_value); }

  template <typename CharT, size_t NTrue, size_t NFalse>
  constexpr auto make_bool(bool initial_value,
                           const CharT (&true_string)[NTrue],
                           const CharT (&false_string)[NFalse]) {
    return Boolean(initial_value, true_string, false_string);
  }
} // namespace sgl
#endif /* SGL_IMPL_BOOLEAN_IMPL_HPP */
