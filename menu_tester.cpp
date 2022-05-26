#include "sgl_macro.hpp"
#include "sgl_menu_tester.hpp"

#include <iostream>
#include <string>

DEFINE_SGL_TYPES(40, char);
enum class test_enum { _0 = 0, _1, _2, _3, _4 };
using namespace sgl::string_view_literals;
auto button_cb = [](Element& b) -> sgl::error {
  std::cout << b.get_name().data() << " pressed\n";
  return sgl::error::no_error;
};

int main() {
  auto menu_tester = sgl::MenuTester(
      sgl::make_menu<40, char>(
          "menu",
          make_page("home",
                    "PAGE1",
                    sgl::Input::enter,
                    sgl::Input::enter,
                    Boolean("boolean 1",
                            "true",
                            true,
                            [](auto& b) {
                              if (b.get_value()) {
                                b.set_text("false");
                                b.set_value(false);
                              } else {
                                b.set_text("true");
                                b.set_value(true);
                              }
                              return sgl::error::no_error;
                            }),
                    Boolean("boolean 2", false),
                    Number("number1"_sv, 42, 1),
                    Text("t1", "this a text"),
                    PageLink("page2link", "page 2 link", "page2"),
                    Element("elem1", "Element 1"),
                    Button("b1", "button1", button_cb),
                    Button("b3", "button3", button_cb),
                    ConstText("ct1"),
                    Enum<test_enum, 5>("enum 1",
                                       {{test_enum::_0, {"zero"}},
                                        {test_enum::_1, "one"},
                                        {test_enum::_2, "two"},
                                        {test_enum::_3, "three"},
                                        {test_enum::_4, "four"}})),
          make_page("page2",
                    "PAGE2",
                    sgl::Input::enter,
                    sgl::Input::enter,
                    Element("elem2", "elem2"),
                    Button("b4", "button4", button_cb),
                    Text("t2", "Text 2"),
                    ConstText("ct2", "ConstText 2"),
                    Enum<test_enum, 5>("enum 2",
                                       {{test_enum::_0, {"zero"}},
                                        {test_enum::_1, "one"},
                                        {test_enum::_2, "two"},
                                        {test_enum::_3, "three"},
                                        {test_enum::_4, "four"}}),
                    PageLink("home page link", "home page link", "home"))),

      sgl::InputMap<char, 5>{{{sgl::Input::up, "up"},
                              {sgl::Input::down, "down"},
                              {sgl::Input::left, "left"},
                              {sgl::Input::right, "right"},
                              {sgl::Input::enter, "enter"}}});

  std::string s;
  while (1) {
    menu_tester.print();

    std::getline(std::cin, s);
    if (s == "quit")
      break;
    menu_tester.handle_input(sgl::string_view<char>(s));
  }
  return 0;
}