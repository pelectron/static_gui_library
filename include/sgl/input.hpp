//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_INPUT_HPP
#define SGL_INPUT_HPP
#include <cstdint>

namespace sgl {

  /**
   * @brief This enum class represents a user input, i.e. character or keypad input.
   * @details Its underlying type is uint64_t to support the following requirements:
   * - accept all character type inputs, be it 8, 16 or 32 bits without modifying the input.
   * - accept more input types, like left, right up, down, enter.
   * - since a static cast conversion from custom integer constants is ok, a user of the library can
   * easily install his own masking in top of this libraries masking.
   *
   * The bits are split up as follows:
   * | Bit Nr  | Meaning                                                                      |
   * |---------|------------------------------------------------------------------------------|
   * | 63      | If 0-> keypad or custom input. If 1 -> character input.                      |
   * | 56 - 59 | reserved for keypad inputs.                                                  |
   * | 0 - 55  | free for use, but sgl assumes character inputs are at most 32 bits in size.  |
   *
   */
  enum class input : uint64_t {
    none = 0, ///< invalid input

    keyboard_type_mask = 0x8000000000000000, ///< mask for keyboard type inputs

    up = 0x0100000000000000,    ///< keypad up
    down = 0x0200000000000000,  ///< keypad down
    left = 0x0300000000000000,  ///< keypad left
    right = 0x0400000000000000, ///< keypad right
    enter = 0x0500000000000000, ///< keypad enter

    keypad_mask = 0x0F00000000000000, ///< mask for keypad type inputs
    char8_mask = 0x00000000000000FF,  ///< 8 bit character mask
    char16_mask = 0x000000000000FFFF, ///< 16 bit character mask
    char32_mask = 0x00000000FFFFFFFF, ///< 32 bit character mask
  };

  /**
   * @brief bitwise OR operator for input
   * @param a first input
   * @param b second input
   * @return input
   */
  constexpr input operator&(input a, input b) noexcept;

  /**
   * @brief bitwise AND operator for input
   * @param a first input
   * @param b second input
   * @return input
   */
  constexpr input operator|(input a, input b) noexcept;

  /**
   * @addtogroup input_conversion Input Conversion Functions
   * @{
   */

  /**
   * @brief get input as keypad input constant, or none if it is not a key pad input.
   * @param i input to convert
   * @return either keypad constant or input::none
   */
  constexpr input get_keypad_input(sgl::input i) noexcept;

  /**
   * @brief check if input is a keyboard input.
   * @param i input to check
   * @return bool
   */
  constexpr bool is_keyboard_input(sgl::input i) noexcept;

  /**
   * @brief check if input is a keypad input.
   * @param i input to check
   * @return bool
   */
  constexpr bool is_keypad_input(sgl::input i) noexcept;

  /**
   * @brief convert char to sgl::input.
   * @param c character to convert
   * @return sgl::input
   */
  constexpr input to_input(char c) noexcept;

  /**
   * @brief convert char16_t to sgl::input.
   * @param c character to convert
   * @return sgl::input
   */
  constexpr input to_input(char16_t c) noexcept;

  /**
   * @brief convert char32_t to input.
   * @param c character to convert
   * @return sgl::input
   */
  constexpr input to_input(char32_t c) noexcept;

  /**
   * @brief convert sgl::input to char.
   * @param i input to convert
   * @return char
   */
  constexpr char get_char8(sgl::input i) noexcept;

  /**
   * @brief convert sgl::input to char16_t
   * @param i input to convert
   * @return char16_t
   */
  constexpr char16_t get_char16(sgl::input i) noexcept;

  /**
   * @brief convert sgl::input to char32_t
   * @param i input to convert
   * @return char32_t
   */
  constexpr char32_t get_char32(sgl::input i) noexcept;

  /**
   * @brief convert sgl::input to CharT.
   * @details this will clear the upper 32 bits of i and then static_cast to charT
   * @tparam CharT character type
   * @param i input to convert
   * @return CharT
   */
  template <typename CharT>
  constexpr CharT get_char(sgl::input i) noexcept;
  /// @}

} // namespace sgl

#include "sgl/impl/input_impl.hpp"

#endif /* SGL_INPUT_HPP */
