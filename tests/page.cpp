#include "sgl.hpp"

#include <catch2/catch.hpp>

using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;
constexpr auto i1 = NAME("bool item 1");
constexpr auto i2 = NAME("double item 2");

TEST_CASE("Page") {
  auto page = sgl::Page(i1 <<= sgl::Boolean(true), i2 <<= sgl::numeric(1.0_double, 1.0));

  bool handled_input1 = false;
  bool handled_input2 = false;

  page[i1].set_input_handler([&handled_input1](auto&, sgl::input) noexcept {
    handled_input1 = true;
    return sgl::error::edit_finished;
  });
  page[i2].set_input_handler([&handled_input2](auto&, sgl::input) noexcept {
    handled_input2 = true;
    return sgl::error::no_error;
  });

  REQUIRE(page.size() == 2);
  REQUIRE(page.current_item_index() == 0);
  REQUIRE_FALSE(page.is_in_edit_mode());

  SECTION("set_current_item") {
    page.set_current_item(1);
    REQUIRE(page.current_item_index() == 1);

    // setting the current item when in edit mode switches the page back into navigation mode.
    page.set_edit_mode();
    REQUIRE(page.is_in_edit_mode());
    page.set_current_item(0);
    REQUIRE_FALSE(page.is_in_edit_mode());
  }
  SECTION("handle_input()") {
    // a page starts out in navigation mode
    REQUIRE(page.current_item_index() == 0);
    REQUIRE_FALSE(page.is_in_edit_mode());

    (void)page.handle_input(sgl::input::down);
    REQUIRE(page.current_item_index() == 1);

    // when the start_edit input is given, the input starts being passed on to the item, incuding
    // the first input it received.
    (void)page.handle_input(sgl::input::enter);
    REQUIRE(handled_input2);
    REQUIRE_FALSE(handled_input1);
    handled_input2 = false;

    (void)page.set_current_item(0);
    // when an item input handler returns sgl::edit_finished, then the page switches back into
    // navigation mode. The error from the item is turned into no_error.
    REQUIRE(sgl::error::no_error == page.handle_input(sgl::input::enter));
    REQUIRE(handled_input1);
    REQUIRE_FALSE(handled_input2);
    REQUIRE_FALSE(page.is_in_edit_mode());
  }
  SECTION("on_enter") {
    bool entered = false;
    page.set_on_enter([&entered](auto&) noexcept {
      entered = true;
      return sgl::error::edit_finished;
    });
    REQUIRE(sgl::error::edit_finished == page.on_enter());
    REQUIRE(entered);
  }
  SECTION("on_exit") {
    bool exited = false;
    page.set_on_exit([&exited](auto&) noexcept {
      exited = true;
      return sgl::error::edit_finished;
    });
    REQUIRE(sgl::error::edit_finished == page.on_exit());
    REQUIRE(exited);
  }
  SECTION("item_name()") {
    REQUIRE(page.item_name(0) == "bool item 1"_sv);
    REQUIRE(page.item_name(1) == "double item 2"_sv);
  }
  SECTION("tick()"){
    bool ticked1 = false;
    bool ticked2 = false;
    page[i1].set_tick_handler([&ticked1](auto&)noexcept{ ticked1 = true;});
    page[i2].set_tick_handler([&ticked2](auto&)noexcept{ ticked2 = true;});
    page.tick();
    REQUIRE(ticked1);
    REQUIRE(ticked2);
  }
}
