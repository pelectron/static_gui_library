#ifndef SGL_MENU_TESTER_HPP
#define SGL_MENU_TESTER_HPP
#include "sgl/menu.hpp"

#include <iostream>
namespace sgl {

  /// \brief pair of sgl::Input and sgl::string_view<char>
  /// \tparam char character type
  template <typename CharT>
  struct InputPair {
    sgl::Input              input;
    sgl::string_view<CharT> string;
  };

  /// \brief This class is used to map sgl::Input values to string_views. It is
  /// used by the MenuTester class.
  /// \tparam char character type
  /// \tparam N number of mapped values
  template <typename CharT, size_t N>
  struct InputMap {
    InputMap(const InputPair<CharT> (&inp_map)[N]) {
      for (size_t i = 0; i < N; ++i)
        map[i] = inp_map[i];
    }
    InputPair<CharT> map[N];
    sgl::Input       get(sgl::string_view<CharT> string) {
            for (const auto& e : map) {
              if (string == e.string)
          return e.input;
      }
            return sgl::Input::none;
    }
  };

  /// \brief This class provides an easy ways to test menus.
  /// \see menu_tester.cpp for a complete example on how to use this class.
  /// \tparam Menu menu type
  /// \tparam N number of inputs mapped
  template <typename Menu, size_t N>
  class MenuTester {
  public:
    /// \brief Construct a new Menu Tester.
    /// \details Example of how to use:
    ///
    /// \code
    /// auto tester = MenuTester(sgl::make_menu(...), // the menu to test
    ///                          // input map mapping sgl::Input to strings
    ///                          {{sgl::Input::up, "up"},
    ///                           {sgl::Input::down, "down"},
    ///                           {sgl::Input::left, "left"},
    ///                           {sgl::Input::right, "right"},
    ///                           {sgl::Input::enter, "enter"}});
    /// \endcode
    ///
    /// \param menu menu to test
    /// \param input_map map of sgl::Input to sgl::string_view<char>
    /// \{
    MenuTester(const Menu& menu, const InputPair<char> (&input_map)[N])
        : menu_(menu), map(input_map) {}

    MenuTester(Menu&& menu, const InputPair<char> (&input_map)[N])
        : menu_(std::move(menu)), map(input_map) {}
    /// \}

    /// \brief handles input
    /// \param input user input as string
    /// \return sgl::error
    sgl::error handle_input(sgl::string_view<char> input) {
      sgl::Input i = map.get(input);
      if (i != sgl::Input::none) {
        return menu_.handle_input(i);
      }
      if (input.size() == 0)
        return menu_.handle_input(sgl::Input::enter);
      else {
        sgl::error ec;
        for (size_t i = 0; i < input.size() and ec == sgl::error::no_error; ++i) {
          ec = menu_.handle_input(sgl::to_input(input[i]));
        }
        return ec;
      }
    }

    /// \brief prints current page and its content to std::out
    void print() const {
      std::cout << "\n---------------\n";
      menu_.for_current_page([](const auto& page) {
        size_t i = 0;
        for_each_with_name(page, [&i, &page](string_view<char> name, const auto& item) {
          if (i == page.current_item_index()) {
            // current item of page
            std::cout << "--> ";
          } else {
            std::cout << "    ";
          }
          std::cout << name.data() << ": " << item.text().data() << std::endl;
          ++i;
        });
      });
      std::cout << "---------------\n";
    }

  private:
    Menu              menu_;
    InputMap<char, N> map;
  };
} // namespace sgl
#endif /* SGL_MENU_TESTER_HPP */
