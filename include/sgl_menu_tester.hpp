#ifndef SGL_MENU_TESTER_HPP
#define SGL_MENU_TESTER_HPP
#include "sgl_menu.hpp"

#include <iostream>
namespace sgl2 {

  /**
   * \brief pair of sgl::Input and sgl::string_view<CharT>
   * \tparam CharT character type
   */
  template <typename CharT>
  struct InputPair {
    sgl::Input              input;
    sgl::string_view<CharT> string;
  };

  /**
   * \brief This class is used to map sgl::Input values to string_views. It is
   * used by the MenuTester class.
   * \tparam CharT character type
   * \tparam N number of mapped values
   */
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

  /**
   * \brief This class provides an easy ways to test menus.
   * \see menu_tester.cpp for a complete example on how to use this class.
   * \tparam Menu menu type
   * \tparam N number of inputs mapped
   */
  template <size_t N, size_t LineWidth, typename CharT, typename... Pages>
  class MenuTester {
  public:
    using Menu = sgl::Menu<CharT, Pages...>;

    using char_type = typename Menu::char_type;
    /**
     * \brief Construct a new Menu Tester.
     * \details Example of how to use:
     *
     * \code
     * auto tester = MenuTester(sgl::make_menu(...), // the menu to test
     *                          // input map mapping sgl::Input to strings
     *                          {{sgl::Input::up, "up"},
     *                           {sgl::Input::down, "down"},
     *                           {sgl::Input::left, "left"},
     *                           {sgl::Input::right, "right"},
     *                           {sgl::Input::enter, "enter"}});
     * \endcode
     *
     * \param menu menu to test
     * \param input_map map of sgl::Input to sgl::string_view<CharT>
     * \{
     */
    MenuTester(const Menu& menu, const InputPair<CharT> (&input_map)[N])
        : menu_(menu), map(input_map) {}

    MenuTester(Menu&& menu, const InputPair<CharT> (&input_map)[N])
        : menu_(std::move(menu)), map(input_map) {}
    /// \}

    /**
     * \brief handles input
     * \param input user input as string
     * \return sgl::error
     */
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

    /**
     * \brief prints current page and its content to std::out
     */
    void print() const {
      std::cout << "\n---------------\n";
      menu_.for_current_page([](const auto& page) {
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

  private:
    Menu                   menu_;
    InputMap<char_type, N> map;
  };

} // namespace sgl2
#endif