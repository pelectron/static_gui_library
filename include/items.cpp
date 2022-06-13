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

int   a{0};

constexpr int func() { return 2; }
static_assert(Callable<int()>([]() noexcept { return 2; })() == 2, "");
enum class test_enum { _0 = 0, _1, _2, _3, _4 };

int main() {
  static_assert(Menu("menu",
                     Page("page1"_sv,
                          "Page 1"_sv,
                          Boolean("bool1", true),
                          Enum<test_enum, 5, 40, char>("enum 1",
                                                       {{test_enum::_0, "zero"},
                                                        {test_enum::_1, "one"},
                                                        {test_enum::_2, "two"},
                                                        {test_enum::_3, "three"},
                                                        {test_enum::_4, "four"}}),
                          PageLink<40, char>("p2l", "page 2 link", "page2")))
                        .handle_input(sgl::Input::enter) == sgl::error::no_error,
                "");
  auto mn = Menu("menu",
                 Page("page1"_sv,
                      "Page 1"_sv,
                      Boolean("bool1", true),
                      Enum<test_enum, 5, 40, char>("enum 1",
                                                   {{test_enum::_0, "zero"},
                                                    {test_enum::_1, "one"},
                                                    {test_enum::_2, "two"},
                                                    {test_enum::_3, "three"},
                                                    {test_enum::_4, "four"}}),
                      Numeric<double, 40, char>("double", 1.0, 2.0),
                      Numeric<float, 40, char>("float", 1.0f, 2.0f),
                      Numeric<int, 40, char>("int", 1, 2),
                      PageLink<40, char>("p2l", "page 2 link", "page2")),
                 Page("page2"_sv,
                      "Page 2"_sv,
                      Boolean("bool1", true),
                      Enum<test_enum, 5, 40, char>("enum 1",
                                                   {{test_enum::_0, "zero"},
                                                    {test_enum::_1, "one"},
                                                    {test_enum::_2, "two"},
                                                    {test_enum::_3, "three"},
                                                    {test_enum::_4, "four"}}),
                      Numeric<double, 40, char>("double", 1.0, 2.0),
                      Numeric<float, 40, char>("float", 1.0f, 2.0f),
                      Numeric<unsigned, 40, char>("int", 1, 1),
                      PageLink<40, char>("p1l", "page 1 link", "page1")));
  static_assert(is_menu_v<decltype(mn)>, "");
  auto p = Page("page2"_sv,
                "Page 2"_sv,
                Boolean("bool1", true),
                Enum<test_enum, 5, 40, char>("enum 1",
                                             {{test_enum::_0, "zero"},
                                              {test_enum::_1, "one"},
                                              {test_enum::_2, "two"},
                                              {test_enum::_3, "three"},
                                              {test_enum::_4, "four"}}),
                Numeric<double, 40, char>("double", 1.0, 2.0),
                Numeric<float, 40, char>("float", 1.0f, 2.0f),
                Numeric<unsigned, 40, char>("int", 1, 1),
                PageLink<40, char>("p1l", "page 1 link", "page1"));
  static_assert(is_page_v<decltype(p)>, "");
  print(mn);
  mn.handle_input(Input::up);
  print(mn);

  // mn.handle_input(Input::up);

  mn.handle_input(Input::enter);

  print(mn);
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
  print(mn);

  // Numeric<int,40,char> numeric("numeric",3,1);
  return 0;
}