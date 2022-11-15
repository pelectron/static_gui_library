#include "sgl/input.hpp"

#include <catch2/catch.hpp>

TEST_CASE("sgl::input") {
  SECTION("to_input") {
    STATIC_REQUIRE((sgl::to_input('A') & sgl::input::keyboard_type_mask) ==
                   sgl::input::keyboard_type_mask);
    STATIC_REQUIRE((sgl::to_input((char16_t)'A') & sgl::input::keyboard_type_mask) ==
                   sgl::input::keyboard_type_mask);
    STATIC_REQUIRE((sgl::to_input((char32_t)'A') & sgl::input::keyboard_type_mask) ==
                   sgl::input::keyboard_type_mask);
  }
  SECTION("is_keyboard_input") {
    STATIC_REQUIRE(sgl::is_keyboard_input(sgl::to_input('A')));
    STATIC_REQUIRE(sgl::is_keyboard_input(sgl::to_input((char16_t)'A')));
    STATIC_REQUIRE(sgl::is_keyboard_input(sgl::to_input((char32_t)'A')));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::input::enter));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::input::left));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::input::right));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::input::up));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::input::down));
  }
  SECTION("get_char") {
    STATIC_REQUIRE(sgl::get_char<char>(sgl::to_input('A')) == 'A');
    STATIC_REQUIRE(sgl::get_char<char16_t>(sgl::to_input((char16_t)531)) == ((char16_t)531));
    STATIC_REQUIRE(sgl::get_char<char32_t>(sgl::to_input((char32_t)70000)) == ((char32_t)70000));
  }
  SECTION("is_keypad_input") {
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::input::enter));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::input::left));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::input::right));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::input::up));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::input::down));
    STATIC_REQUIRE_FALSE(sgl::is_keypad_input(sgl::to_input((char)0)));
    STATIC_REQUIRE_FALSE(sgl::is_keypad_input(sgl::to_input((char16_t)0)));
    STATIC_REQUIRE_FALSE(sgl::is_keypad_input(sgl::to_input((char32_t)0)));
  }
  SECTION("get_keypad_input") {
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::input::enter) == sgl::input::enter);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::input::left) == sgl::input::left);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::input::right) == sgl::input::right);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::input::up) == sgl::input::up);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::input::down) == sgl::input::down);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::to_input((char)0)) == sgl::input::none);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::to_input((char16_t)0)) == sgl::input::none);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::to_input((char32_t)0)) == sgl::input::none);
  }
}