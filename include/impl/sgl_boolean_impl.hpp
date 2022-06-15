#ifndef SGL_BOOLEAN_IMPL_HPP
#define SGL_BOOLEAN_IMPL_HPP
#include "../sgl_boolean.hpp"
namespace sgl {
  
  /// @cond
  namespace detail {
    template <typename CharT>
    static constexpr CharT True[] = {'T', 'R', 'U', 'E', '\0'};
    template <typename CharT>
    static constexpr CharT False[] = {'F', 'A', 'L', 'S', 'E', '\0'};
  }; // namespace detail
  /// @endcond

  template <size_t TextSize, typename CharT>
  constexpr Boolean<TextSize, CharT>::Boolean(StringView name, bool initial_value) noexcept
      : Base(name,
             initial_value ? StringView(sgl::detail::True<CharT>)
                           : StringView(sgl::detail::False<CharT>),
             &default_handle_input),
         true_string_(sgl::detail::True<CharT>),
        false_string_(sgl::detail::False<CharT>),value_(initial_value) {}

  template <size_t TextSize, typename CharT>
  constexpr Boolean<TextSize, CharT>::Boolean(StringView name,
                                              bool       value,
                                              StringView true_text,
                                              StringView false_text) noexcept
      : Base(name, value ? true_text : false_text, &default_handle_input), true_string_(true_text),
        false_string_(false_text), value_(value) {}

  template <size_t TextSize, typename CharT>
  constexpr bool Boolean<TextSize, CharT>::get_value() const noexcept {
    return value_;
  }

  template <size_t TextSize, typename CharT>
  constexpr sgl::error Boolean<TextSize, CharT>::set_value(bool value) noexcept {
    value_ = value;
    if (value)
      this->set_text(true_string());
    else
      this->set_text(false_string());
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
  constexpr sgl::error Boolean<TextSize, CharT>::default_handle_input(item_type& Boolean,
                                                                      sgl::Input) noexcept {
    if (Boolean.get_value()) {
      Boolean.set_value(false);
    } else {
      Boolean.set_value(true);
    }
    return sgl::error::no_error;
  }
} // namespace sgl
#endif