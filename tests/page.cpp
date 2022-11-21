#include "sgl.hpp"
#include "sgl/input.hpp"
#include "sgl/menu.hpp"

#include <catch2/catch.hpp>

/// TODO:
/// for_each_item_with_name
/// operator[](sgl::Name<Cs...> name)

using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;
constexpr auto i1 = NAME("bool item 1");
constexpr auto i2 = NAME("double item 2");

struct TestItem {
  using char_type = char;
  static constexpr size_t text_size = 25;

  constexpr sgl::error handle_input(sgl::input) noexcept { return sgl::error::edit_finished; }

  constexpr void tick() noexcept {}

  template <typename Menu>
  constexpr void set_menu(Menu* m) noexcept {
    this->m = m;
  }

  constexpr sgl::static_string<char_type, text_size>& text() noexcept { return str; }

  constexpr const sgl::static_string<char_type, text_size>& text() const noexcept { return str; }

  void*                                    m{nullptr};
  sgl::static_string<char_type, text_size> str;
};

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

  SECTION("set_current_item(size_t)") {
    page.set_current_item(1);
    REQUIRE(page.current_item_index() == 1);

    // setting the current item when in edit mode switches the page back into navigation mode.
    page.set_edit_mode();
    REQUIRE(page.is_in_edit_mode());
    page.set_current_item(0);
    REQUIRE_FALSE(page.is_in_edit_mode());
  }
  SECTION("set_current_item(sgl::Name)") {
    page.set_current_item(i2);
    REQUIRE(page.current_item_index() == 1);

    // setting the current item when in edit mode switches the page back into navigation mode.
    page.set_edit_mode();
    REQUIRE(page.is_in_edit_mode());
    page.set_current_item(i1);
    REQUIRE_FALSE(page.is_in_edit_mode());
  }
  SECTION("edit mode") {
    REQUIRE_FALSE(page.is_in_edit_mode());
    page.set_edit_mode();
    REQUIRE(page.is_in_edit_mode());
    page.set_navigation_mode();
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
  SECTION("set_input_handler()") {
    bool input_handled = false;
    page.set_input_handler([&input_handled](auto&, sgl::input) noexcept {
      input_handled = true;
      return sgl::error::edit_finished;
    });
    REQUIRE(page.handle_input(sgl::input::none) == sgl::error::edit_finished);
    REQUIRE(input_handled);
  }
  SECTION("(set_)on_enter") {
    bool entered = false;
    page.set_on_enter([&entered](auto&) noexcept {
      entered = true;
      return sgl::error::edit_finished;
    });
    REQUIRE(sgl::error::edit_finished == page.on_enter());
    REQUIRE(entered);
  }
  SECTION("(set_)on_exit") {
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
  SECTION("item_text()") {
    REQUIRE(sgl::string_view(page.item_text(0)) == "TRUE"_sv);
    REQUIRE(sgl::string_view(page.item_text(1)) == "1.0"_sv);
  }
  SECTION("tick()") {
    bool ticked1 = false;
    bool ticked2 = false;
    page[i1].set_tick_handler([&ticked1](auto&) noexcept { ticked1 = true; });
    page[i2].set_tick_handler([&ticked2](auto&) noexcept { ticked2 = true; });
    page.tick();
    REQUIRE(ticked1);
    REQUIRE(ticked2);
  }
  SECTION("set_start_edit() and get_start_edit()") {
    page.set_start_edit(sgl::input::down);
    REQUIRE(page.get_start_edit() == sgl::input::down);
  }
  SECTION("set_stop_edit() and get_stop_edit()") {
    page.set_stop_edit(sgl::input::down);
    REQUIRE(page.get_stop_edit() == sgl::input::down);
  }
  SECTION("for_current_item()") {
    auto lambda = [](auto& item) { return item.handle_input(sgl::input::none); };

    auto error = page.for_current_item(lambda);
    REQUIRE(error == sgl::error::edit_finished);
    REQUIRE(handled_input1);
    REQUIRE_FALSE(handled_input2);

    handled_input1 = false;
    handled_input2 = false;
    page.set_current_item(i2);
    sgl::for_current(page, lambda);
    REQUIRE(handled_input2);
    REQUIRE_FALSE(handled_input1);
  }
  SECTION("for_each_item()") {
    auto lambda = [](auto& item) { item.handle_input(sgl::input::none); };
    page.for_each_item(lambda);
    REQUIRE(handled_input1);
    REQUIRE(handled_input2);

    handled_input1 = false;
    handled_input2 = false;
    sgl::for_each(page, lambda);
    REQUIRE(handled_input1);
    REQUIRE(handled_input2);
  }
  SECTION("set_menu") {
    auto page2 = sgl::Page(i1 <<= TestItem{}, i2 <<= TestItem{});
    page2.set_menu(reinterpret_cast<void*>(42));
    REQUIRE(page2[i1].m == reinterpret_cast<void*>(42));
    REQUIRE(page2[i2].m == reinterpret_cast<void*>(42));
  }
}
