#include "sgl/sgl.hpp"

using namespace sgl::cx_arg_literals;
enum class Setting { opt1, opt2, opt3 };
enum class OtherSetting { Up, Down, Left, Right };


constexpr auto Page1() noexcept {
  return NAME("page1") <<=
         sgl::Page(NAME("bool item 1") <<=  sgl::Boolean(true),
                   NAME("setting item 1") <<= sgl::make_enum(Setting::opt1,
                                                            "Option 1",
                                                            Setting::opt2,
                                                            "Option 2",
                                                            Setting::opt3,
                                                            "Option 3"),
                   NAME("double item 1") <<= sgl::make_numeric(1.0_double, 1.0),
                   NAME("float item 1") <<= sgl::make_numeric(1.0_float, 1.0f),
                   NAME("int item 1") <<= sgl::make_numeric<12, char>(1, 2),
                   NAME("link to page 2") <<= sgl::make_pagelink(NAME("page2"), "return to page 2"),
                   NAME("link to settings page") <<= sgl::make_pagelink(NAME("settings page"), "settings page"));
}

constexpr auto Page2() noexcept {
  return NAME("page2") <<=
         sgl::Page(NAME("bool item 2") <<=  sgl::Boolean(true),
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
                   NAME("link to page 1") <<= sgl::make_pagelink(NAME("page1"), "return to page 1"));
}

constexpr auto SettingsPage() {
  return NAME("settings page") <<=
         sgl::Page(NAME("settings item 2") <<= sgl::make_enum(Setting::opt1,
                                                            "Option 1",
                                                            Setting::opt2,
                                                            "Option 2",
                                                            Setting::opt3,
                                                            "Option 3"),
                   NAME("OtherSetting item 2") <<= sgl::make_enum(OtherSetting::Up,
                                                                   "Up",
                                                                   OtherSetting::Down,
                                                                   "Down",
                                                                   OtherSetting::Left,
                                                                   "Left",
                                                                   OtherSetting::Right,
                                                                   "Right"),
                   NAME("link to page 1") <<= sgl::make_pagelink(NAME("page1"), "return to page 1"));
}

auto make_menu() {
  return sgl::Menu(Page1(), Page2(), SettingsPage());
}
int main() { auto menu = make_menu(); }