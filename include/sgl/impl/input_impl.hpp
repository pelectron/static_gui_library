#ifndef SGL_INPUT_IMPL_HPP
#define SGL_INPUT_IMPL_HPP
#include "sgl/input.hpp"

namespace sgl {
  constexpr input operator&(input a, input b) noexcept {
    return static_cast<input>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
  }

  constexpr input operator|(input a, input b) noexcept {
    return static_cast<input>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
  }

  constexpr input get_keypad_input(sgl::input i) noexcept {
    if ((i & sgl::input::keypad_mask) == i)
      return i;
    return sgl::input::none;
  }

  constexpr bool is_keyboard_input(sgl::input i) noexcept {
    return (i & sgl::input::keyboard_type_mask) == sgl::input::keyboard_type_mask;
  }

  constexpr bool is_keypad_input(sgl::input i) noexcept {
    return (i & sgl::input::keypad_mask) == i;
  }

  constexpr input to_input(char c) noexcept {
    return static_cast<input>(c) | sgl::input::keyboard_type_mask;
  }

  constexpr input to_input(char16_t c) noexcept {
    return static_cast<input>(c) | sgl::input::keyboard_type_mask;
  }

  constexpr input to_input(char32_t c) noexcept {
    return static_cast<input>(c) | sgl::input::keyboard_type_mask;
  }

  constexpr char get_char8(sgl::input i) noexcept {
    return static_cast<char>(i & sgl::input::char8_mask);
  }

  constexpr char16_t get_char16(sgl::input i) noexcept {
    return static_cast<char16_t>(i & sgl::input::char16_mask);
  }

  constexpr char32_t get_char32(sgl::input i) noexcept {
    return static_cast<char32_t>(i & sgl::input::char32_mask);
  }

  template <typename CharT>
  constexpr CharT get_char(sgl::input i) noexcept {
    return static_cast<CharT>(i & sgl::input::char32_mask);
  }

  template <>
  constexpr char get_char<char>(sgl::input i) noexcept {
    return get_char8(i);
  }

  template <>
  constexpr char16_t get_char<char16_t>(sgl::input i) noexcept {
    return get_char16(i);
  }

  template <>
  constexpr char32_t get_char<char32_t>(sgl::input i) noexcept {
    return get_char32(i);
  }

} // namespace sgl
#endif
