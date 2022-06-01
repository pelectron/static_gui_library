#define SGL_INSTANTIATE 1
#define SGL_CHAR_TYPE char
#define SGL_LINE_WIDTH 25

#include "sgl.hpp"
#include <iostream>
using namespace sgl;

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

/// button click handler
auto button_click_handler = [](Button& b) {
  std::cout << b.get_name().data() << " pressed\n";
  return sgl::error::no_error;
};

// custom button input handler, copied default implementation.
auto button_input_handler = [](Item& item, sgl::Input input) {
  auto& button = static_cast<Button&>(item); // get correct type
  std::cout << button.get_name().data() << " input handler called\n";
  auto ec = button.click(); // execute buttons click handler
  switch (ec) {
    case sgl::error::no_error:
      return sgl::error::edit_finished;
    default:
      return ec;
  }
};
/// test enum for Enum_t
enum class test_enum { _0 = 0, _1, _2, _3, _4 };
/// enum input handler, just copied default implementation
auto enum_input_handler = [](Enum<test_enum, 5>& item, sgl::Input input) {
  switch (input) {
    case sgl::Input::right:
      [[fallthrough]];
    case sgl::Input::up:
      item.set_index((item.index() + 1) % item.num_values());
      break;
    case sgl::Input::left:
      [[fallthrough]];
    case sgl::Input::down:
      item.set_index(item.index() == 0 ? item.num_values() - 1
                                       : item.index() - 1);
      break;
    default:
      break;
  }
  item.set_text(item.current_string());
  return sgl::error::no_error;
};
/// Integer input handler, copied default implementation
auto numeric_input_handler = [](auto& item, sgl::Input input) {
  if (!is_keyboard_input(input)) {
    switch (input) {
      case sgl::Input::up:
        [[fallthrough]];
      case sgl::Input::right:
        item.set_value(item.get_value() + item.get_delta());
        break;
      case sgl::Input::down:
        [[fallthrough]];
      case sgl::Input::left:
        item.set_value(item.get_value() - item.get_delta());
        break;
      default:
        break;
    }
  }
  return sgl::error::no_error;
};

/// text validator
auto text_validator = [](StringView sv) { return sgl::error::no_error; };
/// text input handler, copied default implementation
auto text_input_handler = [](auto& element, sgl::Input input) {
  auto& text_elem = static_cast<Text&>(element);
  auto  buffer = text_elem.get_text(); // copy of item text!

  if (sgl::is_keyboard_input(input)) {
    char c = sgl::get_char<char>(input);
    if (sgl::is_visible_char(c))
      buffer.append(c);
  } else {
    switch (input) {
      case sgl::Input::up:
        if (sgl::is_visible_char(buffer[text_elem.cursor()] + 1)) {
          ++buffer[text_elem.cursor()];
        } else {
          buffer[text_elem.cursor()] = '~';
        }
        break;
      case sgl::Input::down:
        if (sgl::is_visible_char(buffer[text_elem.cursor()] - 1)) {
          --buffer[text_elem.cursor()];
        } else {
          buffer[text_elem.cursor()] = ' ';
        }
        break;
      case sgl::Input::left:
        text_elem.decrement_cursor();
        break;
      case sgl::Input::right:
        text_elem.increment_cursor();
        break;
      default:
        break;
    }
  }
  sgl::error ec = text_elem.validate(buffer);
  if (ec == sgl::error::no_error) {
    text_elem.set_text(buffer);
    return sgl::error::no_error;
  }
  return ec;
};

int main() {
  auto menu = make_menu(
      "MainMenu",
      sgl::Input::enter,
      sgl::Input::enter,
      make_page(
          "home",
          "Page 1",
          /// item ctors
          Item("Item1"),
          Item("Item2", "item 2"),
          Item(
              "Item3",
              [](Item& item, sgl::Input) { return sgl::error::edit_finished; }),
          Item(
              "Item4",
              "item 4",
              [](Item& item, sgl::Input) { return sgl::error::edit_finished; }),
          /// boolean ctors
          Boolean("Boolean1", true),
          Boolean("Boolean2",false,"true","false"),
          /// button ctors
          Button("Button1"),
          Button("Button2", "button 2"),
          Button("Button3", "button 3", button_click_handler),
          Button("Button4",
                 "button 4",
                 button_click_handler,
                 button_input_handler),
          /// enum ctors
          make_enum<test_enum>("Enum1",
                               {{test_enum::_0, {"zero"}},
                                {test_enum::_1, "one"},
                                {test_enum::_2, "two"},
                                {test_enum::_3, "three"},
                                {test_enum::_4, "four"}},
                               2),
          Enum<test_enum, 5>("Enum2",
                             {{test_enum::_0, {"zero"}},
                              {test_enum::_1, "one"},
                              {test_enum::_2, "two"},
                              {test_enum::_3, "three"},
                              {test_enum::_4, "four"}},
                             enum_input_handler,
                             1),
          Integer("Integer1", 1, 1),
          Integer("Integer2", 2, 2, &sgl::default_format<char, 25, int>),
          Integer("Integer3",
                  3,
                  3,
                  &sgl::default_format<char, 25, int>,
                  numeric_input_handler),
          Floating("Integer1", 1.0, 1.0),
          Floating("Integer2", 2.0, 2.0, &sgl::default_format<char, 25, float>),
          Floating("Integer3",
                   3.0,
                   3.0,
                   &sgl::default_format<char, 25, float>,
                   numeric_input_handler),
          PageLink("PageLink1", "page 1 link", "page1"),
          Text("Text1", "text 1"),
          Text("Text2", "text 2", text_validator),
          Text("Text3", "text 3", text_validator, text_input_handler),
          ConstText("ConstText1"),
          ConstText("ConstText2", "const text 2")),
      make_page(
          "page1",
          "Page 1",
          /// item ctors
          Item("Item1"),
          Item("Item2", "item 2"),
          Item(
              "Item3",
              [](Item& item, sgl::Input) { return sgl::error::edit_finished; }),
          Item(
              "Item4",
              "item 4",
              [](Item& item, sgl::Input) { return sgl::error::edit_finished; }),
          /// boolean ctors
          Boolean("Boolean1", true),
          Boolean("Boolean2", false, "true","false"),
          /// button ctors
          Button("Button1"),
          Button("Button2", "button 2"),
          Button("Button3", "button 3", button_click_handler),
          Button("Button4",
                 "button 4",
                 button_click_handler,
                 button_input_handler),
          /// enum ctors
          make_enum<test_enum>("Enum1",
                               {{test_enum::_0, {"zero"}},
                                {test_enum::_1, "one"},
                                {test_enum::_2, "two"},
                                {test_enum::_3, "three"},
                                {test_enum::_4, "four"}},
                               2),
          Enum<test_enum, 5>("Enum2",
                             {{test_enum::_0, {"zero"}},
                              {test_enum::_1, "one"},
                              {test_enum::_2, "two"},
                              {test_enum::_3, "three"},
                              {test_enum::_4, "four"}},
                             enum_input_handler,
                             1),
          Integer("Integer1", 1, 1),
          Integer("Integer2", 2, 2, &sgl::default_format<char, 25, int>),
          Integer("Integer3",
                  3,
                  3,
                  &sgl::default_format<char, 25, int>,
                  numeric_input_handler),
          Floating("Integer1", 1.0, 1.0),
          Floating("Integer2", 2.0, 2.0, &sgl::default_format<char, 25, float>),
          Floating("Integer3",
                   3.0,
                   3.0,
                   &sgl::default_format<char, 25, float>,
                   numeric_input_handler),
          PageLink("PageLink1", "home link", "home"),
          Text("Text1", "text 1"),
          Text("Text2", "text 2", text_validator),
          Text("Text3", "text 3", text_validator, text_input_handler),
          ConstText("ConstText1"),
          ConstText("ConstText2", "const text 2")));
  char c;
  bool keep_looping = true;
  print_menu(menu);
  while (keep_looping) {
    c = std::cin.get();
    switch (c) {
      case 'a':
        menu.handle_input(sgl::Input::left);
        break;
      case 's':
        menu.handle_input(sgl::Input::down);
        break;
      case 'd':
        menu.handle_input(sgl::Input::right);
        break;
      case 'w':
        menu.handle_input(sgl::Input::up);
        break;
      case '\0':
        [[fallthrough]];
      case 'e':
        menu.handle_input(sgl::Input::enter);
        break;
      case 'c':
        keep_looping = false;
        break;
      default:
        break;
    }
    print_menu(menu);
  }
  return 0;
}