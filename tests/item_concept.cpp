#include "sgl/item_concepts.hpp"

#include <catch2/catch.hpp>

struct Item {
    using char_type = char;
    static constexpr size_t text_size = 25;

  sgl::error handle_input(sgl::Input) noexcept { return sgl::error::edit_finished; }

  void tick() noexcept {}

  void set_menu(void*) noexcept {}

  sgl::static_string<char_type, text_size>&       text() noexcept;
  const sgl::static_string<char_type, text_size>& text() const noexcept;
};

struct Item2 {
    using char_type = char;
    static constexpr size_t text_size = 322;

  sgl::error handle_input(sgl::Input) noexcept { return sgl::error::edit_finished; }

  void tick() noexcept {}

  template <typename Menu>
  void set_menu(Menu*) noexcept {}

  sgl::static_string<char_type, text_size>& text() noexcept;

  const sgl::static_string<char_type, text_size>& text() const noexcept;
};


TEST_CASE("sgl::is_item_v") {
  STATIC_REQUIRE(sgl::is_item_v<Item>);
  STATIC_REQUIRE(sgl::is_item_v<Item2>);
}