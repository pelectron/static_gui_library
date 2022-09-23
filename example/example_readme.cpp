#include "sgl/sgl.hpp"

#include <iostream>

using namespace sgl;
using namespace sgl::string_view_literals;
using namespace sgl::cx_arg_literals;
enum class Setting { opt1, opt2, opt3 };
enum class OtherSetting { Up, Down, Left, Right };
enum class myEnum {
  val1,
  val2,
  // ...
};

constexpr auto Page1() noexcept {
  return NAME("page1") <<=
         Page(NAME("boolean") <<= Boolean(true),
              NAME("settings enum") <<= make_enum(Setting::opt1,
                                                  "Option 1",
                                                  Setting::opt2,
                                                  "Option 2",
                                                  Setting::opt3,
                                                  "Option 3"),
              NAME("double item") <<= make_numeric(1.0_double, 1.0),
              NAME("float item") <<= make_numeric(1.0_float, 1.0f),
              NAME("int item") <<= make_numeric<12, char>(1, 2),
              NAME("p2l") <<= make_pagelink(NAME("page2"), "return to page 2"),
              NAME("spl") <<= make_pagelink(NAME("settings page"), "settings page"));
}

constexpr auto Page2() noexcept {
  return NAME("page2") <<= Page(NAME("bool 2") <<= Boolean(true),
                                NAME("Other Setting Option") <<= make_enum(OtherSetting::Up,
                                                                           "Up",
                                                                           OtherSetting::Down,
                                                                           "Down",
                                                                           OtherSetting::Left,
                                                                           "Left",
                                                                           OtherSetting::Right,
                                                                           "Right"),
                                NAME("double 2") <<= make_numeric(2.0_double, 2.0),
                                NAME("float2") <<= make_numeric(2.0_float, 2.0f),
                                NAME("int2") <<= make_numeric<12, char>(2, 2),
                                NAME("p1l") <<= make_pagelink(NAME("page1"), "return to page 1"));
}

constexpr auto SettingsPage() {
  return NAME("settings page") <<=
         Page(NAME("settings item") <<= make_enum(Setting::opt1,
                                                  "Option 1",
                                                  Setting::opt2,
                                                  "Option 2",
                                                  Setting::opt3,
                                                  "Option 3"),
              NAME("Other Setting Option") <<= make_enum(OtherSetting::Up,
                                                         "Up",
                                                         OtherSetting::Down,
                                                         "Down",
                                                         OtherSetting::Left,
                                                         "Left",
                                                         OtherSetting::Right,
                                                         "Right"),
              NAME("p1l") <<= make_pagelink(NAME("page1"), "return to page 1"));
}
int main() { auto menu = Menu(Page1(), Page2(), SettingsPage()); }