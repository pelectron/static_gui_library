#include "sgl.hpp"
#include "sgl/page.hpp"

#include <catch2/catch.hpp>

using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;

enum class Setting { opt1, opt2, opt3 };

enum class OtherSetting { Up, Down, Left, Right };

constexpr auto Page1() noexcept {
  return sgl::Page(NAME("bool item 1") <<= sgl::Boolean(true),              // boolean item
                   NAME("setting item 1") <<= sgl::make_enum(Setting::opt1, // enumerated item
                                                             "Option 1",
                                                             Setting::opt2,
                                                             "Option 2",
                                                             Setting::opt3,
                                                             "Option 3"),
                   NAME("double item 1") <<= sgl::numeric(1.0_double, 1.0), // item holding a double
                   NAME("float item 1") <<= sgl::numeric(1.0_float, 1.0f),  // item holding a float
                   NAME("int item 1") <<= sgl::numeric<12, char>(1, 2),     // item holding an int
                   NAME("link to page 2") <<=
                   sgl::pagelink(NAME("page2"), "return to page 2") // a page link
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
                   NAME("double item 2") <<= sgl::numeric(2.0_double, 2.0),
                   NAME("float item 2") <<= sgl::numeric(2.0_float, 2.0f),
                   NAME("int item 2") <<= sgl::numeric<12, char>(2, 2),
                   NAME("link to page 1") <<= sgl::pagelink(NAME("page1"), "return to page 1"));
}

constexpr auto make_menu() {
  // a menu is more or less a named_tuple of pages, so the creation of one is similar to that of a
  // page.
  return sgl::Menu(NAME("page1") <<= Page1(), NAME("page2") <<= Page2());
}

constexpr auto page1 = NAME("page1");
constexpr auto page2 = NAME("page2");

TEST_CASE("sgl::Menu") {
  auto menu = sgl::Menu(NAME("page1") <<= Page1(), NAME("page2") <<= Page2());

  bool page1_entered = false;
  bool page1_exited = false;
  bool page2_entered = false;
  bool page2_exited = false;
  bool bool1_ticked = false;

  menu[page1].set_on_enter([&page1_entered](auto&) noexcept -> sgl::error {
    page1_entered = true;
    return sgl::error::no_error;
  });

  menu[page1].set_on_exit([&page1_exited](auto&) noexcept -> sgl::error {
    page1_exited = true;
    return sgl::error::no_error;
  });

  menu[page1][NAME("bool item 1")].set_tick_handler(
      [&bool1_ticked](auto&) noexcept { bool1_ticked = true; });

  menu[page2].set_on_enter([&page2_entered](auto&) noexcept -> sgl::error {
    page2_entered = true;
    return sgl::error::no_error;
  });

  menu[page2].set_on_exit([&page2_exited](auto&) noexcept -> sgl::error {
    page2_exited = true;
    return sgl::error::no_error;
  });

  REQUIRE(menu.size() == 2);

  SECTION("current_page_index") { REQUIRE(menu.current_page_index() == 0); }

  SECTION("set_current_page(size_t)") {
    REQUIRE(menu.current_page_index() == 0);

    auto error = menu.set_current_page(1);

    REQUIRE(error == sgl::error::no_error);
    REQUIRE(menu.current_page_index() == 1);
    REQUIRE(page1_exited);
    REQUIRE(page2_entered);

    error = menu.set_current_page(2);
    REQUIRE(error == sgl::error::out_of_range);

    page1_entered = false;
    page2_exited = false;
    error = menu.set_current_page(0);
    REQUIRE(page1_entered);
    REQUIRE(page2_exited);
  }
  SECTION("set_current_page(sgl::Name)") {

    REQUIRE(menu.current_page_index() == 0);

    auto error = menu.set_current_page(page2);

    REQUIRE(error == sgl::error::no_error);
    REQUIRE(menu.current_page_index() == 1);
    REQUIRE(page1_exited);
    REQUIRE(page2_entered);

    page1_entered = false;
    page2_exited = false;
    error = menu.set_current_page(page1);
    REQUIRE(error == sgl::error::no_error);
    REQUIRE(page1_entered);
    REQUIRE(page2_exited);
  }

  SECTION("item_name() and page_name()") {
    REQUIRE(menu.current_page_index() == 0);
    REQUIRE(menu.page_name() == "page1"_sv);
    REQUIRE(menu.item_name(0) == "bool item 1"_sv);
    REQUIRE(menu.item_name(1) == "setting item 1"_sv);
    REQUIRE(menu.item_name(2) == "double item 1"_sv);
    REQUIRE(menu.item_name(3) == "float item 1"_sv);
    REQUIRE(menu.item_name(4) == "int item 1"_sv);
    REQUIRE(menu.item_name(5) == "link to page 2"_sv);
    REQUIRE(menu.item_name(6) == sgl::string_view<char>{});

    auto error = menu.set_current_page(1);
    REQUIRE(menu.page_name() == "page2"_sv);
    REQUIRE(error == sgl::error::no_error);

    REQUIRE(menu.item_name(0) == "bool item 2"_sv);
    REQUIRE(menu.item_name(1) == "OtherSetting item 1"_sv);
    REQUIRE(menu.item_name(2) == "double item 2"_sv);
    REQUIRE(menu.item_name(3) == "float item 2"_sv);
    REQUIRE(menu.item_name(4) == "int item 2"_sv);
    REQUIRE(menu.item_name(5) == "link to page 1"_sv);
    REQUIRE(menu.item_name(6) == sgl::string_view<char>{});
  }

  SECTION("switching pages with sgl::PageLink") {
    REQUIRE(menu.set_current_page(0) == sgl::error::no_error);
    REQUIRE(menu.handle_input(sgl::input::up) == sgl::error::no_error);
    REQUIRE(menu.handle_input(sgl::input::enter) == sgl::error::no_error);
    REQUIRE(menu.current_page_index() == 1);
    REQUIRE_FALSE(menu[page1].is_in_edit_mode());
  }

  SECTION("item_text()") {
    REQUIRE(menu.current_page_index() == 0);
    REQUIRE(sgl::string_view<char>(menu[page1][NAME("bool item 1")].text()) == menu.item_text(0));
    REQUIRE(sgl::string_view<char>(menu[page1][NAME("int item 1")].text()) == menu.item_text(4));
  }
  SECTION("tick()") {
    REQUIRE_FALSE(bool1_ticked);
    menu.tick();
    REQUIRE(bool1_ticked);
  }
  SECTION("handle_input") {
    bool input_handled1 = false;
    menu[page1].set_input_handler([&input_handled1](auto&, sgl::input i) noexcept {
      REQUIRE(i == sgl::input::enter);
      input_handled1 = true;
      return sgl::error::edit_finished;
    });
    bool input_handled2 = false;
    menu[page2].set_input_handler([&input_handled2](auto&, sgl::input i) noexcept {
      REQUIRE(i == sgl::input::enter);
      input_handled2 = true;
      return sgl::error::edit_finished;
    });
    REQUIRE(menu.handle_input(sgl::input::enter) == sgl::error::edit_finished);
    REQUIRE(input_handled1);
    REQUIRE_FALSE(input_handled2);

    input_handled1 = false;
    (void)menu.set_current_page(page2);
    REQUIRE(menu.handle_input(sgl::input::enter) == sgl::error::edit_finished);
    REQUIRE(input_handled2);
    REQUIRE_FALSE(input_handled1);
  }
  SECTION("get_page() and operator[]") {
    REQUIRE(&menu[page1] == &menu.get_page<0>());
    REQUIRE(&menu[page2] == &menu.get_page<1>());
  }

  SECTION("for_current_page") {
    bool p1_visited = false;
    bool p2_visited = false;
    REQUIRE(menu.current_page_index() == 0);
    menu.for_current_page([&p1_visited](auto&) noexcept { p1_visited = true; });
    REQUIRE(p1_visited);
    REQUIRE_FALSE(p2_visited);

    p1_visited = false;
    auto error = menu.set_current_page(1);
    sgl::for_current(menu, [&p2_visited](auto&) noexcept { p2_visited = true; });
    REQUIRE(p2_visited);
    REQUIRE_FALSE(p1_visited);
    REQUIRE(error == sgl::error::no_error);
  }
  SECTION("for_current_page()const") {
    bool p1_visited = false;
    bool p2_visited = false;
    REQUIRE(menu.current_page_index() == 0);
    menu.for_current_page([&p1_visited](const auto&) noexcept { p1_visited = true; });
    REQUIRE(p1_visited);
    REQUIRE_FALSE(p2_visited);

    p1_visited = false;
    auto error = menu.set_current_page(1);
    sgl::for_current(menu, [&p2_visited](const auto&) noexcept { p2_visited = true; });
    REQUIRE(p2_visited);
    REQUIRE_FALSE(p1_visited);
    REQUIRE(error == sgl::error::no_error);
  }
  SECTION("for_each_page") {
    bool p1_visited = false;
    bool p2_visited = false;
    menu[page1].set_input_handler([&p1_visited](auto&, sgl::input i) noexcept {
      REQUIRE(i == sgl::input::enter);
      p1_visited = true;
      return sgl::error::edit_finished;
    });
    menu[page2].set_input_handler([&p2_visited](auto&, sgl::input i) noexcept {
      REQUIRE(i == sgl::input::enter);
      p2_visited = true;
      return sgl::error::edit_finished;
    });

    menu.for_each_page([](auto& page) { (void)page.handle_input(sgl::input::enter); });
    REQUIRE(p1_visited);
    REQUIRE(p2_visited);

    p1_visited = false;
    p2_visited = false;

    sgl::for_each(menu, [](auto& p) { (void)p.handle_input(sgl::input::enter); });
    REQUIRE(p1_visited);
    REQUIRE(p2_visited);
  }
}
