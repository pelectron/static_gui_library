#include "sgl/menu_tester.hpp"

#include "sgl/boolean.hpp"
#include "sgl/enum.hpp"
#include "sgl/menu.hpp"
#include "sgl/numeric.hpp"
#include "sgl/page.hpp"
#include "sgl/page_link.hpp"

#include <iostream>
#include <string>

using namespace sgl;
using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;
constexpr auto home_page = NAME("home page");
constexpr auto home_link = NAME("back to home");

constexpr auto settings = NAME("settings");
constexpr auto settings_page = NAME("settings page");
constexpr auto settings_link = NAME("settings link");

constexpr auto other_settings = NAME("other settings");
constexpr auto other_settings_page = NAME("other settings page");
constexpr auto other_settings_link = NAME("other settings link");

constexpr auto page2 = NAME("page 2");
constexpr auto page2link = NAME("page 2 link");

constexpr auto bool_item = NAME("bool 1");
constexpr auto num1 = NAME("number 1");
constexpr auto num2 = NAME("number 2");
constexpr auto float1 = NAME("float 1");

enum class Setting { opt1, opt2, opt3 };
enum class OtherSetting { Up, Down, Left, Right };

constexpr auto HomePage() {
  return Page(bool_item <<= Boolean(true),
              num1 <<= make_numeric<13, char>(1, 2),
              float1 <<= make_numeric<24, char>(5.0f, 0.1f),
              settings_link <<= PageLink(settings_page),
              other_settings_link <<= PageLink(other_settings_page));
}

constexpr auto SettingsPage() {
  return Page(
      settings <<=
      make_enum(Setting::opt1, "Option 1", Setting::opt2, "Option 2", Setting::opt3, "Option 3"),
      other_settings_link <<= PageLink(other_settings_page),
      home_link <<= PageLink(home_page));
}

constexpr auto OtherSettingsPage() {
  return Page(other_settings <<= make_enum(OtherSetting::Up,
                                           "Up",
                                           OtherSetting::Down,
                                           "Down",
                                           OtherSetting::Left,
                                           "Left",
                                           OtherSetting::Right,
                                           "Right"),
              settings_link <<= PageLink(settings_page),
              home_link <<= PageLink(home_page));
}

int main() {
  auto menu = MenuTester(Menu(home_page <<= HomePage(),
                              settings_page <<= SettingsPage(),
                              other_settings_page <<= OtherSettingsPage()),
                         {{sgl::Input::up, "up"_sv},
                          {sgl::Input::down, "down"_sv},
                          {sgl::Input::left, "left"_sv},
                          {sgl::Input::right, "right"_sv},
                          {sgl::Input::enter, "enter"_sv}});
  menu.print();
  std::string s;
  while (std::getline(std::cin, s)) {
    if (s == "quit") {
      break;
    } else {
      menu.handle_input(string_view<char>(s));
      menu.print();
    }
  }
  return 0;
}