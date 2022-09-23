#ifndef SGL_ENUM_IMPL_HPP
#define SGL_ENUM_IMPL_HPP
#include "sgl/enum.hpp"
namespace sgl {

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  template <
      typename InputHandler,
      enable_if_is_input_handler<InputHandler,
                                 typename Enum<T, NumEnumerators, TextSize, CharT>::item_type>>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>::Enum(
      const sgl::EnumMap<T, NumEnumerators, CharT>& map,
      InputHandler&&                                handler,
      size_t                                        start_index) noexcept
      : Base(map.get_view(start_index % NumEnumerators), std::forward<InputHandler>(handler)),
        map_{map}, index_(start_index % NumEnumerators) {}

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr size_t Enum<T, NumEnumerators, TextSize, CharT>::num_values() const noexcept {
    return NumEnumerators;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr typename Enum<T, NumEnumerators, TextSize, CharT>::StringView
      Enum<T, NumEnumerators, TextSize, CharT>::current_string() const noexcept {
    return map_.get_view(index_);
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr size_t Enum<T, NumEnumerators, TextSize, CharT>::index() const noexcept {
    return index_;
  }
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr void Enum<T, NumEnumerators, TextSize, CharT>::set_index(size_t index) noexcept {
    index_ = index % NumEnumerators;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr T Enum<T, NumEnumerators, TextSize, CharT>::get_value() const noexcept {
    return map_.get_value(index_);
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr sgl::error Enum<T, NumEnumerators, TextSize, CharT>::set_value(T value) noexcept {
    size_t i = 0;
    for (const auto& elem : map_) {
      if (elem.first == value) {
        index_ = i;
        return sgl::error::no_error;
      }
      ++i;
    }
    return sgl::error::invalid_value;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr sgl::error
      Enum<T, NumEnumerators, TextSize, CharT>::default_handle_input(item_type& enum_item,
                                                                     sgl::Input input) noexcept {
    switch (input) {
      case sgl::Input::right:
        [[fallthrough]];
      case sgl::Input::up:
        enum_item.set_index((enum_item.index() + 1) % enum_item.num_values());
        break;
      case sgl::Input::left:
        [[fallthrough]];
      case sgl::Input::down:
        enum_item.set_index(enum_item.index() == 0 ? enum_item.num_values() - 1
                                                   : enum_item.index() - 1);
        break;
      default:
        break;
    }
    enum_item.set_text(enum_item.current_string());
    return error::no_error;
  }
  
  template <typename E, typename Str, typename... EnumStringArgs>
  constexpr auto
      make_enum(const E& enum_val, const Str& enum_string, const EnumStringArgs&... args) {
    using char_type = typename get_char_type<const Str&>::type;
    constexpr auto Size = biggest_str_len<const Str&, const EnumStringArgs&...>();
    return sgl::Enum<E, sizeof...(EnumStringArgs) / 2 + 1, Size, char_type>{
        enum_map(enum_val, enum_string, args...)};
  }

  template <typename E, typename Str, typename... EnumStringArgs>
  constexpr auto make_enum(size_t     start_index,
                           const E&   enum_val,
                           const Str& enum_string,
                           const EnumStringArgs&... args) {
    using char_type = typename get_char_type<const Str&>::type;
    constexpr auto Size = biggest_str_len<const Str&, const EnumStringArgs&...>();
    return Enum<E, sizeof...(EnumStringArgs) + 1, Size, char_type>(
        enum_map(enum_val, enum_string, args...),
        start_index);
  }

  template <typename InputHandler, typename E, typename Str, typename... EnumStringArgs>
  constexpr auto make_enum(InputHandler&& handler,
                           size_t         start_index,
                           const E&       enum_val,
                           const Str&     enum_string,
                           const EnumStringArgs&... args) {
    using char_type = typename get_char_type<const Str&>::type;
    constexpr auto Size = biggest_str_len<const Str&, const EnumStringArgs&...>();
    return Enum<E, sizeof...(EnumStringArgs), Size, char_type>(
        enum_map(enum_val, enum_string, args...),
        std::forward<InputHandler>(handler),
        start_index);
  }
} // namespace sgl
#endif