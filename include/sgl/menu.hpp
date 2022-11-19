/**
 * @file menu.hpp
 * @author Pelé Constam (pelectron1602@gmail.com)
 * This file contains the sgl::Menu class and the free iteration functions sgl::for_each,
 * sgl::for_each_with_name and sgl::for_current.
 *
 * @version 0.1
 * @date 2022-09-24
 *
 *          Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef SGL_MENU_HPP
#define SGL_MENU_HPP
#include "sgl/callable.hpp"
#include "sgl/error.hpp"
#include "sgl/input.hpp"
#include "sgl/named_tuple.hpp"
#include "sgl/page.hpp"
#include "sgl/smallest_type.hpp"

namespace sgl {
  /// @cond

  template <typename NameList, typename PageTypeList>
  class Menu;

  /// @endcond

  /**
   * @headerfile menu.hpp "sgl/menu.hpp"
   * The menu class brings everything together and holds all the pages of the
   * menu.
   *
   * @details It consists of a NameTuple of pages, an index to keep track of the current page, and
   * an input handler. The default input handler just delegates the input to the current page and
   * returns its result.
   * Constructing a menu is the same as constructing a [NamedTuple](#NamedTuple) of pages.
   *
   * @tparam NameList name of the pages. The type of NameList is sgl::type_list<sgl::Name<...>...>.
   * @tparam PageList page types. The type of PageList is sgl::type_list<sgl::Page<...>...>.
   */
  template <typename NameList, typename PageList>
  class Menu {
  public:
    static_assert(sgl::is_type_list_v<PageList>,
                  "ItemList must ba an instance of sgl::type_list<Items...>");

    static_assert(sgl::is_type_list_v<NameList>,
                  "NameList must ba an instance of sgl::type_list<Names...>");

    static_assert(sgl::detail::all_are_name_types(NameList{}),
                  "All names in NameList must be an instance of sgl::Name<Chars...>");

    static_assert(sgl::all_unique_v<NameList>,
                  "sgl::Page can't have duplicate names for its items! Make sure to give every "
                  "item in the page a unique name.");

    static_assert(sgl::detail::all_same_char_type(PageList{}),
                  "All items in a page must have the same char_type! Make sure ech item has the "
                  "expected character type.");

    using PageTuple = sgl::NamedTuple<NameList, PageList>;

    static constexpr bool nothrow_copy_constructible =
        std::is_nothrow_copy_constructible_v<PageTuple>;

    static constexpr bool nothrow_move_constructible =
        std::is_nothrow_move_constructible_v<PageTuple>;

    /// type list of page names
    using name_list = NameList;

    /// type list of page types
    using page_list = PageList;

    /// character type of the menu
    using char_type = typename sgl::first_t<PageList>::char_type;

    /// string view type of the menu
    using StringView = sgl::string_view<char_type>;

    /// concrete input handler type
    using InputHandler_t = sgl::Callable<sgl::error(sgl::Menu<NameList, PageList>&, sgl::input)>;

    /**
     * copy ctor
     * @param other menu to copy
     */
    constexpr Menu(const Menu& other) noexcept(std::is_nothrow_copy_constructible_v<PageTuple>);

    /**
     * move ctor
     * @param other menu to move from
     */
    constexpr Menu(Menu&& other) noexcept(std::is_nothrow_move_constructible_v<PageTuple>);

    /**
     * construct menu from named pages
     * @param pages pages as named arguments.
     */
    template <typename... Names, typename... Pages>
    constexpr explicit Menu(const sgl::NamedValue<Names, Pages>&... pages) noexcept(
        std::is_nothrow_copy_constructible_v<PageTuple>);

    /**
     * construct menu from named pages
     * @param pages pages as named arguments.
     */
    template <typename... Names, typename... Pages>
    constexpr explicit Menu(sgl::NamedValue<Names, Pages>&&... pages) noexcept(
        std::is_nothrow_move_constructible_v<PageTuple>);

    /**
     * invoke the menu's input handler
     * @param i input to handle
     * @return sgl::error
     */
    [[nodiscard]] constexpr sgl::error handle_input(sgl::input i) noexcept;

    /**
     * invoke tick() method for each item in the menu.
     * @note Keep in mind that this function call can take a non negligible time to complete if you
     * have a lot of tick handlers doing (maybe expensive) work, so don't call it in an IRQ!
     * @see item_tick_handling
     */
    constexpr void tick() noexcept;

    /**
     * get the index of the currently active page
     * @return size_t
     */
    [[nodiscard]] constexpr size_t current_page_index() const noexcept;

    /**
     * get number of pages stored in the menu
     * @return size_t
     */
    [[nodiscard]] constexpr size_t size() const noexcept;

    /**
     * set current page by index
     * @param page_index zero based index of the page
     * @return sgl::error
     */
    [[nodiscard]] constexpr sgl::error set_current_page(size_t page_index) noexcept;

    /**
     * set current page by name
     * @param name name of the page
     * @return sgl::error
     */
    template <char... Cs>
    [[nodiscard]] constexpr sgl::error set_current_page(sgl::Name<Cs...> name) noexcept;

    /**
     * get page by name
     * @tparam Name name type
     * @param name name instance
     * @return reference to page
     */
    template <char... Cs>
    [[nodiscard]] constexpr auto& operator[](sgl::Name<Cs...> name) noexcept;

    /**
     * get page by name
     * @tparam Name name type
     * @param name name instance
     * @return const reference to page
     */
    template <char... Cs>
    [[nodiscard]] constexpr const auto& operator[](sgl::Name<Cs...> name) const noexcept;

    /**
     * get reference to page at index I
     * @tparam I page index
     * @return reference to I-th page
     */
    template <size_t I>
    [[nodiscard]] constexpr auto& get_page() noexcept;

    /**
     * get const reference to page at index I
     * @tparam I page index
     * @return const reference to I-th page
     */
    template <size_t I>
    [[nodiscard]] constexpr const auto& get_page() const noexcept;

    /**
     * get the name of the current page as a string_view.
     * @return sgl::string_view<char>
     */
    [[nodiscard]] constexpr sgl::string_view<char> page_name() const noexcept;

    /**
     * returns the name of the i-th item of the current page as a string_view.
     * @param i item index
     * @return sgl::string_view<char>
     */
    [[nodiscard]] constexpr sgl::string_view<char> item_name(size_t i) const noexcept;

    /**
     * returns the text from the i-th item of the current page as a string_view.
     * @param i item index
     * @return sgl::string_view<char_type>
     */
    [[nodiscard]] constexpr sgl::string_view<char_type> item_text(size_t i) const noexcept;

    /**
     * apply f on each page in menu.
     * @details
     * @code
     * // defined somewhere in a header
     * template<typename Page>
     * void global_func(Page& page){...};
     *
     * // some where a menu instance called menu used in a function
     * menu.for_each_page(global_func);
     * // or with a generic lambda
     * menu.for_each_page([](auto& page){...});
     *
     * @endcode
     *
     * @tparam F functor type
     * @param f functor
     * @{
     */
    template <typename F>
    constexpr void for_each_page(F&& f);

    template <typename F>
    constexpr void for_each_page(F&& f) const;

    /** @} */

    /**
     * apply f on each page in menu.
     * @details
     * @code
     *
     * // defined somewhere in a header
     * template<typename Page>
     * void global_func(sgl::string_view<char> name, Page& page){...};
     *
     * // some where a menu instance called menu used in a function
     * menu.for_each_page(global_func);
     * // or with a generic lambda
     * menu.for_each_page([](sgl::string_view<char> name, auto& page){...});
     *
     * @endcode
     *
     * @tparam F functor type
     * @param f functor
     * @{
     */
    template <typename F>
    constexpr void for_each_page_with_name(F&& f) {
      sgl::for_each_with_name(pages_, std::forward<F>(f));
    }

    template <typename F>
    constexpr void for_each_page_with_name(F&& f) const {
      sgl::for_each_with_name(pages_, std::forward<F>(f));
    }

    /** @} */

    /**
     * apply f on the current page.
     * @tparam F functor type
     * @param f functor instance
     * @return returns f(current_item), if f returns a non-void value. Else the return type is
     * void.
     * @{
     */
    template <typename F>
    constexpr decltype(auto) for_current_page(F&& f);

    template <typename F>
    constexpr decltype(auto) for_current_page(F&& f) const;
    /** @} */

  private:
    [[nodiscard]] constexpr static sgl::error default_handle_input(Menu& menu,
                                                                   input input) noexcept;

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_page_impl(F&& f);

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_page_impl(F&& f) const;

    template <size_t I>
    [[nodiscard]] constexpr sgl::string_view<char> page_name_impl() const noexcept;

    [[nodiscard]] constexpr sgl::string_view<char> item_name_impl(size_t i) const noexcept;

    [[nodiscard]] constexpr sgl::string_view<char_type> item_text_impl(size_t i) const noexcept;

    PageTuple      pages_;                                      /// < pages owned by this menu
    InputHandler_t input_handler_{&default_handle_input};       /// < menu input handler
    sgl::smallest_type_t<sgl::list_size_v<PageList>> index_{0}; /// < index of current page
  };

  /**
   * @cond
   */
  template <typename... Names, typename... Ts>
  Menu(NamedValue<Names, Ts>&&...) -> Menu<sgl::type_list<Names...>, sgl::type_list<Ts...>>;

  template <typename... Names, typename... Ts>
  Menu(const NamedValue<Names, Ts>&...) -> Menu<sgl::type_list<Names...>, sgl::type_list<Ts...>>;
  /// @endcond

  /**
   * apply f on each page in menu.
   * @tparam NameList list of page names
   * @tparam PageList list of page types
   * @tparam F functor type
   * @param menu menu instance
   * @param f functor instance
   * @{
   */
  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_each(Menu<NameList, PageList>& menu, F&& f);

  template <typename NameList, typename PageList, typename F>
  void for_each(const Menu<NameList, PageList>& menu, F&& f);
  /** @} */

  /**
   * apply f on each page of the menu, with the name of the page.
   * See Menu::for_each_page_with_name for more details.
   * @tparam NameList list of page names
   * @tparam PageList list of page types
   * @tparam F functor type
   * @param menu menu instance
   * @param f functor instance
   * @{
   */
  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(Menu<NameList, PageList>& menu, F&& f);

  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(const Menu<NameList, PageList>& menu, F&& f);
  /** @} */

  /**
   * apply f on the current page of the menu.
   * @tparam NameList list of page names
   * @tparam PageList list of page types
   * @tparam F functor type
   * @param menu menu instance
   * @param f functor instance
   * @{
   */
  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_current(Menu<NameList, PageList>& menu, F&& f);

  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_current(const Menu<NameList, PageList>& menu, F&& f);
  /** @} */

} // namespace sgl

#include "sgl/impl/menu_impl.hpp"
#endif /* SGL_MENU_HPP */
