#include "sgl.hpp"

#include <catch2/catch.hpp>

using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;

enum class Setting { opt1, opt2, opt3 };

enum class OtherSetting { Up, Down, Left, Right };

constexpr auto Page1() noexcept {
  // a page is more or less a named_tuple of items. The <<= operator 'binds' the name(created with
  // the NAME macro) on the left hand side to the item on the right hand side.
  return sgl::Page(
      NAME("bool item 1") <<= sgl::Boolean(true),              // boolean item
      NAME("setting item 1") <<= sgl::make_enum(Setting::opt1, // enumerated item
                                                "Option 1",
                                                Setting::opt2,
                                                "Option 2",
                                                Setting::opt3,
                                                "Option 3"),
      NAME("double item 1") <<= sgl::make_numeric(1.0_double, 1.0), // item holding a double
      NAME("float item 1") <<= sgl::make_numeric(1.0_float, 1.0f),  // item holding a float
      NAME("int item 1") <<= sgl::make_numeric<12, char>(1, 2),     // item holding an int
      NAME("link to page 2") <<= sgl::pagelink(NAME("page2"), "return to page 2") // a page link
  );
}

constexpr auto Page2() noexcept {
  return sgl::Page(NAME("bool item 2") <<= sgl::Boolean(true),
                   NAME("OtherSetting item 1") <<= sgl::make_enum(OtherSetting::Up,
                                                                  "Up",
                                                                  OtherSetting::Down,
                                                                  "Down",
                                                                  OtherSetting::Left,
                                                                  "Left",
                                                                  OtherSetting::Right,
                                                                  "Right"),
                   NAME("double item 2") <<= sgl::make_numeric(2.0_double, 2.0),
                   NAME("float item 2") <<= sgl::make_numeric(2.0_float, 2.0f),
                   NAME("int item 2") <<= sgl::make_numeric<12, char>(2, 2),
                   NAME("link to page 1") <<= sgl::pagelink(NAME("page1"), "return to page 1"));
}

constexpr auto make_menu() {
  // a menu is more or less a named_tuple of pages, so the creation of one is similar to that of a
  // page.
  return sgl::Menu(NAME("page1") <<= Page1(), NAME("page2") <<= Page2());
}

TEST_CASE("sgl::Menu") {
  auto menu = sgl::Menu(NAME("page1") <<= Page1(), NAME("page2") <<= Page2());

  REQUIRE(menu.size() == 2);
  
  SECTION("item_name() and page_name()"){
  REQUIRE(menu.page_name() == "page1"_sv);
  REQUIRE(menu.item_name(0) == "bool item 1"_sv);
  REQUIRE(menu.item_name(1) == "setting item 1"_sv);
  REQUIRE(menu.item_name(2) == "double item 1"_sv);
  REQUIRE(menu.item_name(3) == "float item 1"_sv);
  REQUIRE(menu.item_name(4) == "int item 1"_sv);
  REQUIRE(menu.item_name(5) == "link to page 2"_sv);

  auto error = menu.set_current_page(1);
  REQUIRE(menu.page_name() == "page2"_sv);
  REQUIRE(error == sgl::error::no_error);

  REQUIRE(menu.item_name(0) == "bool item 2"_sv);
  REQUIRE(menu.item_name(1) == "OtherSetting item 1"_sv);
  REQUIRE(menu.item_name(2) == "double item 2"_sv);
  REQUIRE(menu.item_name(3) == "float item 2"_sv);
  REQUIRE(menu.item_name(4) == "int item 2"_sv);
  REQUIRE(menu.item_name(5) == "link to page 1"_sv);
  }

  SECTION("switching pages") {
    REQUIRE(menu.set_current_page(0) == sgl::error::no_error);
    REQUIRE(menu.handle_input(sgl::Input::up) == sgl::error::no_error);
    REQUIRE(menu.handle_input(sgl::Input::enter) == sgl::error::no_error);
    REQUIRE(menu.current_page_index() == 1);
  }
}
