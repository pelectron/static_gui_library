#define SGL_INSTANTIATE 1
#define SGL_CHAR_TYPE   char
#define SGL_LINE_WIDTH  25
#define SGL_USE_RYU     1
#include "sgl.hpp"

#include <iostream>

using namespace sgl::string_view_literals;
using namespace sgl;

template <typename Menu>
void print(const Menu& menu) {
  std::cout << "\n---------------\n";
  menu.for_current_page([](const auto& page) {
    std::cout << page.title().data() << std::endl;
    size_t i = 0;
    page.for_each_item([&i, &page](const auto& item) {
      if (i == page.index()) {
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

int a{0};

constexpr int func() { return 2; }
static_assert(Callable<int()>([]() noexcept { return 2; })() == 2, "");
enum class test_enum { _0 = 0, _1, _2, _3, _4 };
using namespace sgl::cx_arg_literals;
template <typename Float>
constexpr bool epsilon_equal(Float f1, Float f2, Float epsilon = 0.000001) {
  Float diff = f1 - f2;
  diff = diff < 0 ? -diff : diff;
  return diff < epsilon;
}
constexpr auto float_ = 0.2_float;
constexpr auto float_2 = 0.2e2_float;
constexpr auto float_3 = +0.3e11_double;
constexpr auto float_4 = 1.e-3_float;
static_assert(epsilon_equal(float_.value, 0.2f, 0.2f), "");
static_assert(float_2.value == 0.2e2f, "");
// static_assert(float_3.value == 0.3f, "");
// static_assert(float_4.value == 1.e-3f, "");

int main() {
  auto double_1 = 0.3e-11_double;
  auto double_2 = .3e-11_double;
  auto double_3 = -0.3e-11_double;
  auto double_4 = -.3e+11_double;

  std::cout << (char*)float_.string << std::endl;
  std::cout << float_.value << std::endl;
  std::cout << (char*)double_3.string << std::endl;
  std::cout << double_3.value << std::endl;
  std::cout << (char*)double_4.string << std::endl;
  std::cout << double_4.value << std::endl;
  std::cout << (1.0e-3_double).value << std::endl;
  static_assert(Menu(make_page("page1"_sv,
                               "Page 1"_sv,
                               Boolean("bool1", true),
                               Enum<test_enum, 5, 40, char>("enum 1",
                                                            {{test_enum::_0, "zero"},
                                                             {test_enum::_1, "one"},
                                                             {test_enum::_2, "two"},
                                                             {test_enum::_3, "three"},
                                                             {test_enum::_4, "four"}}),
                               PageLink<40, char>("p2l", "page 2 link", "page2"),
                               make_numeric<30>("num item", 65.43_float, 1.0f)))
                        .handle_input(sgl::Input::enter) == sgl::error::no_error,
                "");
  auto mn = Menu(make_page("page1"_sv,
                           "Page 1"_sv,
                           Boolean("bool1", true),
                           Enum<test_enum, 5, 40, char>("enum 1",
                                                        {{test_enum::_0, "zero"},
                                                         {test_enum::_1, "one"},
                                                         {test_enum::_2, "two"},
                                                         {test_enum::_3, "three"},
                                                         {test_enum::_4, "four"}}),
                           make_numeric<25>("double"_sv, 1.0, 1.0),
                           make_numeric<15>("float"_sv, 1.0f, 1.0f),
                           make_numeric<13>("int"_sv, 1, 2),
                           PageLink<40, char>("p2l", "page 2 link", "page2")),
                 make_page("page2"_sv,
                           "Page 2"_sv,
                           Boolean("bool1", true),
                           Enum<test_enum, 5, 40, char>("enum 1",
                                                        {{test_enum::_0, "zero"},
                                                         {test_enum::_1, "one"},
                                                         {test_enum::_2, "two"},
                                                         {test_enum::_3, "three"},
                                                         {test_enum::_4, "four"}}),
                           make_numeric<20>("double2", 2.0, 2.0),
                           make_numeric<45>("float2", 2.0f, 2.0f),
                           make_numeric<10>("int2", 2, 2),
                           PageLink<40, char>("p1l", "page 1 link", "page1")));
  static_assert(is_menu_v<decltype(mn)>, "");
  // print(mn);
  mn.handle_input(Input::up);
  // print(mn);

  // mn.handle_input(Input::up);

  mn.handle_input(Input::enter);

  // print(mn);
  mn.handle_input(Input::down);
  mn.handle_input(Input::down);
  mn.handle_input(Input::enter);
  mn.handle_input(Input::up);
  mn.handle_input(Input::up);
  mn.handle_input(Input::enter);

  mn.handle_input(Input::down);
  mn.handle_input(Input::enter);
  mn.handle_input(Input::down);
  mn.handle_input(Input::down);
  mn.handle_input(Input::enter);
  mn.handle_input(Input::down);
  mn.handle_input(Input::enter);
  mn.handle_input(Input::down);
  mn.handle_input(Input::down);
  mn.handle_input(Input::enter);
  // print(mn);

  // Numeric<int,40,char> numeric("numeric",3,1);
  return 0;
}