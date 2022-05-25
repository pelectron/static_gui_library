#include "sgl.hpp"
using namespace sgl;
#include <iostream>
static constexpr size_t L = 40;
enum class test_enum { _0 = 0, _1, _2, _3, _4 };
using Pair_t = Pair<test_enum, char>;
using sv = string_view<char>;

using Element = Item_t<L, char>;
using Button = Button_t<L, char>;
using Text = Text_t<L, char>;
using ConstText = ConstText_t<L, char>;
using Number = Integer_t<int, L, char>;
using Enum = Enum_t<test_enum, 5, L, char>;
using PageLink = PageLink_t<L, char>;

auto sv_i = "hello"_sv;
template <typename Page>
void print_page(const Page& page) {
  std::cout << "Title : " << page.get_title().data() << std::endl;
  for (size_t i = 0; i < page.size(); ++i) {
    if (i == page.index()) {
      std::cout << "--> " << i << ". " << page[i].get_text().data()
                << std::endl;
    } else
      std::cout << "    " << i << ". " << page[i].get_text().data()
                << std::endl;
  }
}

template <typename Menu, size_t I>
void print_menu_impl(const Menu& menu) {
  std::cout << menu.current_item().get_name().data() << std::endl;
  if constexpr ((Menu::num_pages - 1) > I) {
    if (I == menu.index())
      print_page(menu.template get_page<I>());
    else
      print_menu_impl<Menu, I + 1>(menu);
  } else {
    if (I == menu.index()) {
      print_page(menu.template get_page<I>());
    }
  }
}
template <typename Menu>
void print_menu(const Menu& m) {
  std::cout << "------------\n";
  print_menu_impl<Menu, 0>(m);
  std::cout << "------------\n";
}
auto button_cb = [](Element& b) -> sgl::error {
  std::cout << b.get_name().data() << " pressed\n";
  return sgl::error::no_error;
};
int main() {
  auto page =
      make_page<40, char>("home",
                          "page1",
                          Text("t1", "this a text"),
                          PageLink("home 2 link", "page 2 link", "page2"),
                          Element("elem1", "Element 1"),
                          Button("b1", "button1", button_cb),
                          Button("b3", "button3", button_cb));
  auto menu = make_menu<40, char>(
      "menu",
      make_page<40, char>(
          "home",
          "PAGE1",
          Input::enter,
          Input::enter,
          Text("t1", "this a text"),
          PageLink("home 2 link", "page 2 link", "page2"),
          Element("elem1", "Element 1"),
          Button("b1", "button1", button_cb),
          Button("b3", "button3", button_cb),

          ConstText("ct1"),
          Enum_t<test_enum, 5, L, char>("enum 1",
                                        {{test_enum::_0, {"zero"}},
                                         {test_enum::_1, "one"},
                                         {test_enum::_2, "two"},
                                         {test_enum::_3, "three"},
                                         {test_enum::_4, "four"}})),
      make_page<40, char>(
          "page2",
          "PAGE2",
          Input::enter,
          Input::enter,
          Element("elem2", "elem2"),
          Button("b2", "button2", button_cb),
          Text("t2", "Text 2"),
          ConstText("ct2", "ConstText 2"),
          Enum_t<test_enum, 5, L, char>("enum 2",
                                        {{test_enum::_0, {"zero"}},
                                         {test_enum::_1, "one"},
                                         {test_enum::_2, "two"},
                                         {test_enum::_3, "three"},
                                         {test_enum::_4, "four"}})));

  menu.current_item();
  menu.current_page_name();
  menu.get_page<0>();
  menu.set_active_page("home");
  char c;
  print_menu(menu);
  bool keep_looping = true;
  while (keep_looping) {
    c = std::cin.get();
    switch (c) {
      case 'a':
        menu.handle_input(Input::left);
        print_menu(menu);

        break;
      case 's':
        menu.handle_input(Input::down);
        print_menu(menu);

        break;
      case 'd':
        menu.handle_input(Input::right);
        print_menu(menu);

        break;
      case 'w':
        menu.handle_input(Input::up);
        print_menu(menu);

        break;
      case 'e':
        menu.handle_input(Input::enter);
        print_menu(menu);

        break;
      case 'c':
        keep_looping = false;
        break;
      default:
        break;
    }
  }
  auto menu2 = make_menu<40, char>(
      "menu",
      Input::enter,
      Input::enter,
      make_page<40, char>(
          "home",
          "PAGE1",
          Text("t1", "this a text"),
          PageLink("home 2 link", "page 2 link", "page2"),
          Element("elem1", "Element 1"),
          Button("b1", "button1", button_cb),
          Button("b3", "button3", button_cb),
          ConstText("ct1"),
          Enum_t<test_enum, 5, L, char>("enum 1",
                                        {{test_enum::_0, {"zero"}},
                                         {test_enum::_1, "one"},
                                         {test_enum::_2, "two"},
                                         {test_enum::_3, "three"},
                                         {test_enum::_4, "four"}})),
      make_page<40, char>(
          "page2",
          "PAGE2",
          Element("elem2", "elem2"),
          Button("b2", "button2", button_cb),
          Text("t2", "Text 2"),
          ConstText("ct2", "ConstText 2"),
          Enum_t<test_enum, 5, L, char>("enum 2",
                                        {{test_enum::_0, {"zero"}},
                                         {test_enum::_1, "one"},
                                         {test_enum::_2, "two"},
                                         {test_enum::_3, "three"},
                                         {test_enum::_4, "four"}})));
  std::cout << menu2.size();
  menu2.get_page<0>();
  menu2.get_page<1>();

  menu2.index();
  return 0;
}