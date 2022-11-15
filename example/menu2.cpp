#include "sgl/boolean.hpp"
#include "sgl/enum.hpp"
#include "sgl/menu.hpp"
#include "sgl/numeric.hpp"
#include "sgl/page.hpp"
#include "sgl/page_link.hpp"

#include <iostream>

using namespace sgl;
using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;

// names of items and pages of a menu
constexpr auto home_page = NAME("home page");
constexpr auto home_link = NAME("back to home");

constexpr auto page2 = NAME("page 2");
constexpr auto page2link = NAME("page 2 link");

constexpr auto bool_item = NAME("bool 1");
constexpr auto settings = NAME("settings page");
constexpr auto other_settings = NAME("other settings page");
constexpr auto num1 = NAME("number 1");
constexpr auto num2 = NAME("number 2");
constexpr auto float1 = NAME("float 1");

// some example enums
enum class Setting { opt1, opt2, opt3 };
enum class OtherSetting { Up, Down, Left, Right };

// returns the home page
constexpr auto home() {
  return Page(
      bool_item <<= Boolean(true),
      settings <<=
      make_enum(Setting::opt1, "Option 1", Setting::opt2, "Option 2", Setting::opt3, "Option 3"),
      num1 <<= make_numeric<13, char>(1, 2),
      float1 <<= make_numeric<24, char>(5.0f, 0.1f),
      page2link <<= PageLink(page2));
}

// returns page 2
constexpr auto page_2() {
  return Page(other_settings <<= make_enum(OtherSetting::Up,
                                           "Up",
                                           OtherSetting::Down,
                                           "Down",
                                           OtherSetting::Left,
                                           "Left",
                                           OtherSetting::Right,
                                           "Right"),
              num2 <<= make_numeric(25.234_double, 0.125),
              home_link <<= PageLink(home_page));
}

template <typename Menu>
void print(const Menu& menu) {
  std::cout << "\n---------------\n";
  menu.for_current_page([](const auto& page) {
    size_t i = 0;
    page.for_each_item([&i, &page](const auto& item) {
      if (i == page.current_item_index()) {
        // current item of page
        std::cout << "--> ";
      } else {
        std::cout << "    ";
      }
      std::cout << item.text().data() << std::endl;
      ++i;
    });
  });
  std::cout << "---------------\n";
}

int main() {
  auto menu = Menu(home_page <<= home(), page2 <<= page_2());
  print(menu);
  auto ec = menu.handle_input(sgl::input::enter);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::up);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::enter);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::enter);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::up);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::up);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::enter);
  std::cout << static_cast<int>(ec);
  print(menu);
  ec = menu.handle_input(sgl::input::up);
  std::cout << static_cast<int>(ec);
  print(menu);
  std::cout << static_cast<int>(ec);
  ec = menu.handle_input(sgl::input::enter);
  print(menu);
  std::cout << static_cast<int>(ec);
  return static_cast<int>(ec);
};
