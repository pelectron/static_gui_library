#include "sgl/input.hpp"

#include <catch2/catch.hpp>

TEST_CASE("sgl::Input") {
  SECTION("to_input") {
    STATIC_REQUIRE((sgl::to_input('A') & sgl::Input::keyboard_type_mask) ==
                   sgl::Input::keyboard_type_mask);
    STATIC_REQUIRE((sgl::to_input((char16_t)'A') & sgl::Input::keyboard_type_mask) ==
                   sgl::Input::keyboard_type_mask);
    STATIC_REQUIRE((sgl::to_input((char32_t)'A') & sgl::Input::keyboard_type_mask) ==
                   sgl::Input::keyboard_type_mask);
  }
  SECTION("is_keyboard_input") {
    STATIC_REQUIRE(sgl::is_keyboard_input(sgl::to_input('A')));
    STATIC_REQUIRE(sgl::is_keyboard_input(sgl::to_input((char16_t)'A')));
    STATIC_REQUIRE(sgl::is_keyboard_input(sgl::to_input((char32_t)'A')));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::Input::enter));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::Input::left));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::Input::right));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::Input::up));
    STATIC_REQUIRE_FALSE(sgl::is_keyboard_input(sgl::Input::down));
  }
  SECTION("get_char") {
    STATIC_REQUIRE(sgl::get_char<char>(sgl::to_input('A')) == 'A');
    STATIC_REQUIRE(sgl::get_char<char16_t>(sgl::to_input((char16_t)531)) == ((char16_t)531));
    STATIC_REQUIRE(sgl::get_char<char32_t>(sgl::to_input((char32_t)70000)) == ((char32_t)70000));
  }
  SECTION("is_keypad_input") {
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::Input::enter));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::Input::left));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::Input::right));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::Input::up));
    STATIC_REQUIRE(sgl::is_keypad_input(sgl::Input::down));
    STATIC_REQUIRE_FALSE(sgl::is_keypad_input(sgl::to_input((char)0)));
    STATIC_REQUIRE_FALSE(sgl::is_keypad_input(sgl::to_input((char16_t)0)));
    STATIC_REQUIRE_FALSE(sgl::is_keypad_input(sgl::to_input((char32_t)0)));
  }
  SECTION("get_keypad_input") {
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::Input::enter) == sgl::Input::enter);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::Input::left) == sgl::Input::left);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::Input::right) == sgl::Input::right);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::Input::up) == sgl::Input::up);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::Input::down) == sgl::Input::down);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::to_input((char)0)) == sgl::Input::none);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::to_input((char16_t)0)) == sgl::Input::none);
    STATIC_REQUIRE(sgl::get_keypad_input(sgl::to_input((char32_t)0)) == sgl::Input::none);
  }
}