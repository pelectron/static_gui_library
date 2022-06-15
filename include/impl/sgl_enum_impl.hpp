#ifndef SGL_ENUM_IMPL_HPP
#define SGL_ENUM_IMPL_HPP
#include "../sgl_enum.hpp"
namespace sgl{

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>::Enum(StringView name,
                                                           const Pair (&map)[NumEnumerators],
                                                           size_t start_index) noexcept
      : Enum<T, NumEnumerators, TextSize, CharT>(name, map, &Enum<T, NumEnumerators, TextSize, CharT>::default_handle_input, start_index) {}

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, Enum<T, NumEnumerators, TextSize, CharT>>>
  Enum<T, NumEnumerators, TextSize, CharT>::Enum(StringView name,
                                                 const Pair (&map)[NumEnumerators],
                                                 InputHandler&& handler,
                                                 size_t         start_index) noexcept
      : Base(name, map[start_index % NumEnumerators].string, forward<InputHandler>(handler)),
        map_{}, index_(start_index % NumEnumerators) {
    for (size_t i = 0; i < NumEnumerators; ++i) {
      map_[i] = map[i];
    }
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>::Enum(
      StringView name,
      const Pair (&map)[NumEnumerators],
      sgl::error (*handler)(item_type&, sgl::Input) noexcept,
      size_t start_index) noexcept
      : Base(name, map[start_index % NumEnumerators].string, handler),
        index_(start_index % NumEnumerators) {
    for (size_t i = 0; i < NumEnumerators; ++i) {
      map_[i] = map[i];
    }
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr size_t Enum<T, NumEnumerators, TextSize, CharT>::num_values() const noexcept {
    return NumEnumerators;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr typename Enum<T, NumEnumerators, TextSize, CharT>::StringView
      Enum<T, NumEnumerators, TextSize, CharT>::current_string() const noexcept {
    return map_[index_].string;
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
    return map_[index_].value;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr sgl::error Enum<T, NumEnumerators, TextSize, CharT>::set_value(T value) noexcept {
    size_t i = 0;
    for (const auto& elem : map_) {
      if (elem.val == value) {
        index_ = i;
        return sgl::error::no_error;
      }
      ++i;
    }
    return sgl::error::invalid_value;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  const typename Enum<T, NumEnumerators, TextSize, CharT>::Map_t&
      Enum<T, NumEnumerators, TextSize, CharT>::get_map() const noexcept {
    return map_;
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
}
#endif