#ifndef SGL_FWD_HPP
#define SGL_FWD_HPP
#include "sgl_input.hpp"
#include "sgl_string_view.hpp"
namespace sgl {
  /// @defgroup item_types Item Types
  /// @{
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
  template <size_t, typename>
  class PageLink_t;
  template <size_t, typename>
  class Text_t;
  template <size_t, typename>
  class ConstText_t;
  /// @}

  /// @defgroup menu_type Menu Type
  /// @{
  template <size_t, typename, typename...>
  class Menu_t;
  /// @}

  /// @defgroup page_type Page Type
  template <size_t, typename, typename...>
  class Page_t;
  /// @}

  /// @defgroup custom_string_types Custom String Types
  /// @{
  template <typename>
  class string_view;
  template <typename, size_t>
  class StaticString;
  /// @}

  /// @defgroup menu_factories Menu Factory Functions
  /// menu factory functions needed for (partial) type deduction
  /// @{
  /**
   * @brief create a menu where all pages have the same start- and stop-edit
   * value. The start and stop edit inputs do not have to be given to the pages
   * when constructed, as this function will take care of that.
   *
   * @tparam LineWidth display width
   * @tparam CharT character type
   * @tparam Pages page types
   * @param menu_name name of the menu
   * @param start_edit input to start edit mode on
   * @param stop_edit input to stop edit mode on
   * @param pages Page_t instances
   * @return Menu_t<LineWidth, CharT, Pages...>
   */
  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> menu_name,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               Pages&&... pages);
  /**
   * @brief create menu from name and pages.
   * @tparam LineWidth display width
   * @tparam CharT   character type
   * @tparam Pages  page types
   * @param menu_name name of menu
   * @param pages Page_t instances
   * @return Menu_t<LineWidth, CharT, Pages...>
   */
  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> menu_name,
                                               Pages&&... pages);
  /// @}

  /// @defgroup page_factories Page Factory Functions
  /// page factory functions needed for (partial) type deduction
  /// @{

  /**
   * @brief create page with custom input handler.
   *
   * @tparam LineWidth display width
   * @tparam CharT character type
   * @tparam InputHandler input handler type
   * @tparam Items  item types
   * @param page_name name of page
   * @param page_title  title of page
   * @param start_edit input to start edit mode on
   * @param stop_edit input to stop edit mode on
   * @param input_handler page input handler
   * @param items items
   * @return Page_t<LineWidth, CharT, Items...>
   */
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

  /**
   * @brief create page from name, title, items and start- and stop-edit values
   * with default input handler.
   * @tparam LineWidth display width
   * @tparam CharT character type
   * @tparam Items  item types
   * @param page_name name of page
   * @param page_title  title of page
   * @param start_edit input to start edit mode on
   * @param stop_edit input to stop edit mode on
   * @param items items
   * @return Page_t<LineWidth, CharT, Items...>
   */
  template <size_t LineWidth, typename CharT, typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> page_name,
                                               string_view<CharT> page_title,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               Items&&... items);

  /**
   * @brief create page from name, title, items and with default input handler..
   * Use this if you want the default start and stop edit values, i.e.
   * sgl::Input::enter, or if you use make_menu(menu_name, start_edit,
   * stop_edit, pages...) factory function.
   * @tparam LineWidth display width
   * @tparam CharT character type
   * @tparam Items  item types
   * @param page_name name of page
   * @param page_title  title of page
   * @param items items
   * @return Page_t<LineWidth, CharT, Items...>
   */
  template <size_t LineWidth, typename CharT, typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> page_name,
                                               string_view<CharT> page_title,
                                               Items&&... items);
  /// @}

} // namespace sgl
#endif