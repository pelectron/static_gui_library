#ifndef SGL_INPUT_HPP
#define SGL_INPUT_HPP
#include <cstdint>
namespace sgl {

  enum class Input : uint64_t {
    none = 0,
    keyboard_type_mask = 0x8000000000000000,
    up = 0x0000000100000000,
    down = 0x0000000200000000,
    left = 0x0000000300000000,
    right = 0x0000000400000000,
    enter = 0x0000000500000000,
    keypad_mask = 0x0000000F00000000,
    char8_mask = 0x00000000000000FF,
    char16_mask = 0x000000000000FFFF,
    char32_mask = 0x00000000FFFFFFFF,
  };

  constexpr Input operator&(Input a, Input b) {
    return static_cast<Input>(static_cast<uint64_t>(a) &
                              static_cast<uint64_t>(b));
  }
  constexpr Input operator|(Input a, Input b) {
    return static_cast<Input>(static_cast<uint64_t>(a) |
                              static_cast<uint64_t>(b));
  }
  constexpr bool is_keyboard_input(Input i) {
    return (i & Input::keyboard_type_mask) == Input::keyboard_type_mask;
  }
  constexpr Input to_input(char c) {
    return static_cast<Input>(c) | Input::keyboard_type_mask;
  }
  constexpr Input to_input(char16_t c) {
    return static_cast<Input>(c) | Input::keyboard_type_mask;
  }
  constexpr Input to_input(char32_t c) {
    return static_cast<Input>(c) | Input::keyboard_type_mask;
  }

  constexpr char get_char8(Input i) {
    return static_cast<char>(i & Input::char8_mask);
  }
  constexpr char16_t get_char16(Input i) {
    return static_cast<char16_t>(i & Input::char16_mask);
  }
  constexpr char32_t get_char32(Input i) {
    return static_cast<char32_t>(i & Input::char32_mask);
  }
  constexpr Input get_keypad_input(Input i) { return Input::keypad_mask & i; }

  template <typename CharT>
  constexpr CharT get_char(Input i) {
    return static_cast<CharT>(i & Input::char32_mask);
  }
  template <>
  constexpr char get_char<char>(Input i) {
    return get_char8(i);
  }
  template <>
  constexpr char16_t get_char<char16_t>(Input i) {
    return get_char16(i);
  }

  template <>
  constexpr char32_t get_char<char32_t>(Input i) {
    return get_char32(i);
  }
} // namespace sgl
#endif