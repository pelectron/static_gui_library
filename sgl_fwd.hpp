#ifndef SGL_FWD_HPP
#define SGL_FWD_HPP
#include "sgl_input.hpp"
#include "sgl_string_view.hpp"
namespace sgl {
  template <size_t, typename>
  class Item_t;
  template <size_t, typename>
  class Boolean_t;
  template <size_t, typename>
  class Button_t;
  template <typename, size_t, size_t, typename>
  class Enum_t;
  template <typename, size_t, typename>
  class Numeric_t;
  template <size_t, typename, typename...>
  class Menu_t;
  template <size_t, typename, typename...>
  class Page_t;
  template <size_t, typename>
  class PageLink_t;
  template <size_t, typename>
  class Text_t;
  template <size_t, typename>
  class ConstText_t;
  template <typename>
  class string_view;
  /// type trait to get the number of pages in a menu.
  template <typename Menu>
  struct number_of_pages;

  template <size_t LineWidth, typename CharT, typename... Pages>
  struct number_of_pages<Menu_t<LineWidth, CharT, Pages...>> {
    static constexpr size_t value = sizeof...(Pages);
  };
  /// type trait to determine the number of items stored in a page.
  template <typename Page>
  struct number_of_items;

  template <size_t LineWidth, typename CharT, typename... Items>
  struct number_of_items<Page_t<LineWidth, CharT, Items...>> {
    static constexpr size_t value = sizeof...(Items);
  };

  template <typename Menu>
  static constexpr size_t number_of_pages_v = number_of_pages<Menu>::value;

  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> menu_name,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               Pages&&... pages);
  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> menu_name,
                                               Pages&&... pages);

  template <size_t LineWidth,
            typename CharT,
            typename InputHandler,
            typename... Items,
            typename>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> page_name,
                                               string_view<CharT> page_title,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               InputHandler&&     input_handler,
                                               Items&&... items);

  template <size_t LineWidth, typename CharT, typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> page_name,
                                               string_view<CharT> page_title,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               Items&&... items);

  template <size_t LineWidth, typename CharT, typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> page_name,
                                               string_view<CharT> page_title,
                                               Items&&... items);
} // namespace sgl
#endif