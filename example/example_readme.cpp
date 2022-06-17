#include "../include/sgl.hpp"

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
  return make_page("page1"_sv,
                   "Page 1"_sv,
                   Boolean("bool 1", true),
                   make_enum<20>("Setting Option"_sv,
                                 SGL_ENUM_MAP({Setting::opt1, "Option 1"},
                                              {Setting::opt2, "Option 2"},
                                              {Setting::opt3, "Option 3"})),
                   make_numeric<25>("double item", 1.0_double, 1.0),
                   make_numeric<15>("float item", 1.0_float, 1.0f),
                   make_numeric<13>("int item", 1, 2),
                   PageLink<40, char>("p2l", "page 2 link", "page2"));
}
constexpr auto Page2() noexcept {
  return make_page("page2"_sv,
                   "Page 2"_sv,
                   Boolean("bool 2", true),
                   make_enum<5>("Other Setting Option"_sv,
                                SGL_ENUM_MAP({OtherSetting::Up, "Up"},
                                             {OtherSetting::Down, "Down"},
                                             {OtherSetting::Left, "Left"},
                                             {OtherSetting::Right, "Right"})),
                   make_numeric<20>("double2", 2.0_double, 2.0),
                   make_numeric<45>("float2", 2.0_float, 2.0f),
                   make_numeric<10>("int2", 2, 2),
                   PageLink<40, char>("p1l", "page 1 link", "page1"));
}

auto assign = [](auto&& f)->Callable<int(void)> {
  Callable<int(void)> c;
  c = Callable<int(void)>(f);
  return c;
};
auto l3 = []() noexcept { return 3; };
int            main() {
  static_assert((Callable<int(void)>(assign(l3)))() == l3());

  auto menu = Menu(Page1(), Page2());
}