#include "sgl/item_concepts.hpp"
#include <catch2/catch.hpp>
struct Item{
  sgl::error handle_input(sgl::Input)noexcept{ return sgl::error::edit_finished;}
  void tick()noexcept{}
  void set_menu(void*)noexcept{}
};
struct Item2{
  sgl::error handle_input(sgl::Input)noexcept{ return sgl::error::edit_finished;}
  void tick()noexcept{}
  template<typename Menu>
  void set_menu(Menu*)noexcept{}
};


TEST_CASE("sgl::is_item_v"){
  STATIC_REQUIRE(sgl::is_item_v<Item>);
  STATIC_REQUIRE(sgl::is_item_v<Item2>);

}