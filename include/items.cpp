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
error button_cb(Button<40, char>& b) {
  a = 1;
  return error::no_error;
}
struct M {
  error set_active_page(string_view<char>) { return error::no_error; }
};
constexpr int func() { return 2; }
static_assert(Callable<int()>(+[]() { return 2; })() == 2, "");
enum class test_enum { _0 = 0, _1, _2, _3, _4 };

int main() {
  int  k = 1;
  auto lambda1 = [k_ = k](Button<40, char>& b) noexcept {
    a = k_;
    return error::no_error;
  };
  auto                      lambda2{lambda1};
  M                         menu;
  PageLink<40, char>        link("hi", "world", "link");
  Numeric<int, 40, char>    integer("int", 1, 2);
  Numeric<float, 40, char>  floating("float", 1.0f, 2.0f);
  Numeric<double, 40, char> double_f("double", 1.0, 2.0);
  Enum<test_enum, 5, 40, char>("enum 1",
                               {{test_enum::_0, "zero"},
                                {test_enum::_1, "one"},
                                {test_enum::_2, "two"},
                                {test_enum::_3, "three"},
                                {test_enum::_4, "four"}});
  static_assert(is_item_v<decay_t<decltype(link)>>, "");
  link.set_menu(&menu);
  link.clear_text();
  Button<40, char> button("b1", "Button 1");
  Button<40, char> button2("b2", "Button 2", [k_ = k](Button<40, char>& b) noexcept {
    a = k_;
    return error::no_error;
  });
  button2.click();
  Boolean<40, char> bool1 = Boolean<40, char>("bool1", true);

  auto page = Page("page1"_sv, "Page 1"_sv, Boolean<40, char>("bool1", true));
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
  return a;
}