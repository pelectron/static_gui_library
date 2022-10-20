//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_INPUT_HPP
#define SGL_INPUT_HPP
#include <cstdint>
namespace sgl {

  /// \brief This enum class represents a universal input, i.e. character or keypad input
  enum class Input : uint64_t {
    none = 0,                                ///< invalid input
    keyboard_type_mask = 0x8000000000000000, ///< mask for keyboard type inputs
    down = 0x0000000200000000,               ///< keypad down
    left = 0x0000000300000000,               ///< keypad left
    up = 0x0000000100000000,                 ///< keypad up
    right = 0x0000000400000000,              ///< keypad right
    enter = 0x0000000500000000,              ///< keypad enter
    keypad_mask = 0x0000000700000000,        ///< keypad value mask
    char8_mask = 0x00000000000000FF,         ///< 8 bit character mask
    char16_mask = 0x000000000000FFFF,        ///< 16 bit character mask
    char32_mask = 0x00000000FFFFFFFF,        ///< 32 bit character mask
  };

  /// \brief bitwise or operator for Input
  /// \param a first input
  /// \param b second input
  /// \return Input
  constexpr Input operator&(Input a, Input b) {
    return static_cast<Input>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
  }

  /// \brief bitwise and operator for Input
  /// \param a first input
  /// \param b second input
  /// \return Input
  constexpr Input operator|(Input a, Input b) {
    return static_cast<Input>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
  }

  /// \defgroup input_keypad_conversion input to keypad conversion
  /// \addtogroup input_type
  /// \{

  /// \brief get input as keypad input constant
  /// \param input input to convert
  /// \return either keypad constant or Input::none
  constexpr Input get_keypad_input(Input input) {
    switch (Input::keypad_mask & input) {
      case Input::up:
        [[fallthrough]];
      case Input::down:
      case Input::left:
      case Input::right:
        return Input::keypad_mask & input;
      default:
        return Input::none;
    }
  }
  /// \}

  /// \addtogroup input_type
  /// \{

  /// check if input is a keyboard input
  /// \return bool
  constexpr bool is_keyboard_input(Input input) {
    return (input & Input::keyboard_type_mask) == Input::keyboard_type_mask;
  }

  /// check if input is a keypad input.
  /// \return bool
  constexpr bool is_keypad_input(Input input) {
    return get_keypad_input(input) != sgl::Input::none;
  }
  /// \}

  /// \addtogroup char_input_conversion
  /// \{

  /// convert char to Input
  /// \param c character to convert
  /// \return sgl::Input
  constexpr Input to_input(char c) { return static_cast<Input>(c) | Input::keyboard_type_mask; }

  /// convert char16_t to Input
  /// \param c character to convert
  /// \return sgl::Input
  constexpr Input to_input(char16_t c) { return static_cast<Input>(c) | Input::keyboard_type_mask; }

  /// convert char32_t to Input
  /// \param c character to convert
  /// \return sgl::Input
  constexpr Input to_input(char32_t c) { return static_cast<Input>(c) | Input::keyboard_type_mask; }

  /// convert Input to char
  /// \param i input to convert
  /// \return char
  constexpr char get_char8(Input i) { return static_cast<char>(i & Input::char8_mask); }

  /// convert Input to char16_t
  /// \param i input to convert
  /// \return char16_t
  constexpr char16_t get_char16(Input i) { return static_cast<char16_t>(i & Input::char16_mask); }

  /// convert Input to char32_t
  /// \param i input to convert
  /// \return char32_t
  constexpr char32_t get_char32(Input i) { return static_cast<char32_t>(i & Input::char32_mask); }

  /// \brief convert Input to CharT
  /// \tparam CharT character type
  /// \param i input to convert
  /// \return CharT
  template <typename CharT>
  constexpr CharT get_char(Input i) {
    return static_cast<CharT>(i & Input::char32_mask);
  }
  /// \cond
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
  /// \endcond
  /// \}

} // namespace sgl
#endif /* SGL_INPUT_HPP */
