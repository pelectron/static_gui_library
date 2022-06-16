#include "../include/enum_map.hpp"
//#include "../include/sgl.hpp"
#include <iostream>
using namespace sgl;
using namespace sgl::string_view_literals;
enum class Setting { opt1, opt2, opt3 };
enum class OtherSetting { Up, Down, Left, Right };
#define Mac(var, ...) var << ##__VA_ARGS__
int main() {

  auto map = SGL_MAP({OtherSetting::Up, "Up"_sv},
                     {OtherSetting::Down, "Down"_sv},
                     {OtherSetting::Left, "Left"_sv},
                     {OtherSetting::Right, "Right"_sv});

  // auto m = MAP({OtherSetting::Up, "Up"_sv},
  //              {OtherSetting::Down, "Down"_sv},
  //              {OtherSetting::Left, "Left"_sv},
  //              {OtherSetting::Right, "Right"_sv});
  // auto menu = Menu(make_page("page1",
  //                            "Page 1",
  //                            Boolean("bool 1", true),
  //                            make_enum<Setting,20>("enum 1"_sv,
  //                                          {{Setting::opt1, "Option 1"},
  //                                           {Setting::opt2, "Option 2"},
  //                                           {Setting::opt3, "Option 3"}}),
  //                            make_numeric<25>("double item", 1.0, 1.0),
  //                            make_numeric<15>("float item", 1.0f, 1.0f),
  //                            make_numeric<13>("int item", 1, 2),
  //                            PageLink<40, char>("p2l", "page 2 link", "page2")),
  //                  make_page("page2",
  //                            "Page 2",
  //                            Boolean("bool 2", true),
  //                            make_enum<OtherSetting,5>("enum 1"_sv,
  //                                         {{OtherSetting::Up, "Up"},
  //                                          {OtherSetting::Down, "Down"},
  //                                          {OtherSetting::Left, "Left"},
  //                                          {OtherSetting::Right, "Right"}}),
  //                            make_numeric<20>("double2", 2.0, 2.0),
  //                            make_numeric<45>("float2", 2.0f, 2.0f),
  //                            make_numeric<10>("int2", 2, 2),
  //                            PageLink<40, char>("p1l", "page 1 link", "page1")));
}