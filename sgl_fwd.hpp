#ifndef SGL_FWD_HPP
#define SGL_FWD_HPP
#include "sgl_input.hpp"
#include "sgl_string_view.hpp"
namespace sgl {
  /// @cond
  template <size_t, typename>
  class Item_t;
  template <size_t, typename>
  class Boolean_t;
  template <size_t, typename>
  class Button_t;
  template <typename, size_t, size_t, typename>
  class Enum_t;
  template <typename, size_t, typename>
  class Integer_t;
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
  /// @endcond
} // namespace sgl
#endif