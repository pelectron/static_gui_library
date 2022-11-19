//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#include "sgl/menu_tester.hpp"


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
              num1 <<= numeric<13, char>(1, 2),
              float1 <<= numeric<24, char>(5.0f, 0.1f),
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

constexpr auto MainMenu() {
  // this menu as three pages, a home page, settings page and 'other' settings page
  return sgl::Menu(home_page <<= HomePage(),
                   settings_page <<= SettingsPage(),
                   other_settings_page <<= OtherSettingsPage());
}

int main() {
  // making a menu tester with the menu and supplying a mapping of sgl::input values to
  // sgl::string_view. The tester will check if a command line input matches any of the strings in
  // the input map and forward the corresponding sgl::input value. If the command line input is not
  // in the map, the tester will call handle_input for every character in the input string.
  auto tester = MenuTester(MainMenu(),
                           {{sgl::input::up, "up"_sv},
                            {sgl::input::down, "down"_sv},
                            {sgl::input::left, "left"_sv},
                            {sgl::input::right, "right"_sv},
                            {sgl::input::enter, "enter"_sv}});
  // print just shows the current page with all items
  tester.print();

  std::string s;
  // main event loop
  while (std::getline(std::cin, s)) {
    if (s == "quit") {
      break;
    }
    tester.handle_input(sgl::string_view<char>(s));
    tester.print();
  }
  return 0;
}
