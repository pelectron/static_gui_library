#ifndef SGL_MENU_TESTER_HPP
#define SGL_MENU_TESTER_HPP
#include "sgl.hpp"

#include <iostream>
namespace sgl {
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

  template <typename CharT>
  struct InputPair {
    sgl::Input                    input;
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
} // namespace sgl
#endif