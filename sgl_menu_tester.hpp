#ifndef SGL_MENU_TESTER_HPP
#define SGL_MENU_TESTER_HPP
#define SGL_INSTANTIATE 1
#define SGL_CHAR_TYPE char
#define SGL_LINE_WIDTH 40
#include "sgl.hpp"

#include <iostream>
namespace sgl {
  template <typename Menu>
  void print_menu(const Menu& menu) {
    std::cout << "\n---------------\n";
    menu.for_current_page([](const auto& page) {
      std::cout << page.get_title().data() << std::endl;
      for (size_t i = 0; i < page.size(); ++i) {
        if (i == page.index()) {
          // current item of page
          std::cout << "--> ";
        } else {
          std::cout << "    ";
        }
        std::cout << page[i].get_text().data() << std::endl;
      }
    });
    std::cout << "---------------\n";
  }

  template <typename CharT>
  struct InputPair {
    sgl::Input              input;
    sgl::string_view<CharT> string;
  };

  template <typename CharT, size_t N>
  struct InputMap {
    InputPair<CharT> map[N];
    sgl::Input       get(sgl::string_view<CharT> string) {
      for (const auto& e : map) {
        if (string == e.string)
          return e.input;
      }
      return sgl::Input::none;
    }
  };

  template <typename Menu, size_t N>
  class MenuTester {
  public:
    using char_type = typename Menu::char_type;
    MenuTester(const Menu& menu, const InputMap<char_type, N>& input_map)
        : menu_(menu), map(input_map) {}

    sgl::error handle_input(sgl::string_view<char_type> input) {
      sgl::Input i = map.get(input);
      if (i != sgl::Input::none) {
        menu_.handle_input(i);
        return sgl::error::no_error;
      }
      if (input.size() == 0)
        menu_.handle_input(sgl::Input::enter);
      else
        for (size_t i = 0; i < input.size(); ++i) {
          menu_.handle_input(sgl::to_input(input[i]));
        }
      return sgl::error::no_error;
    }
    void print() { print_menu(menu_); }

  private:
    Menu                   menu_;
    InputMap<char_type, N> map;
  };
  /// @endcond
} // namespace sgl
#endif