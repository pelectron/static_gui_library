/**
 * @file sgl/page.hpp
 *
 * @author Pelé Constam (pelectron1602@gmail.com)
 * @brief This file contains the sgl::Page class template and the page iteration functions
 * sgl::for_each(), sgl::for_each_with_name() and sgl::for_current().
 *
 * @version 0.1
 * @date 2022-09-23
 *
 *          Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef SGL_PAGE_HPP
#define SGL_PAGE_HPP
#include "sgl/callable.hpp"
#include "sgl/error.hpp"
#include "sgl/fwd.hpp"
#include "sgl/input.hpp"
#include "sgl/item_concepts.hpp"
#include "sgl/named_tuple.hpp"
#include "sgl/smallest_type.hpp"
#include "sgl/static_string.hpp"

namespace sgl {
  /// @cond
  namespace detail {
    template <typename... Ts>
    constexpr bool all_same_char_type(sgl::type_list<Ts...>) {
      return sgl::all_same_v<typename Ts::char_type...>;
    }

    template <typename... Ts>
    constexpr bool all_are_name_types(sgl::type_list<Ts...>) {
      return (sgl::is_name_type_v<Ts> && ...);
    }
  } // namespace detail

  /// @endcond

  /**
   * @headerfile page.hpp "sgl/page.hpp"
   * @brief This class represents a page of a menu. A page is a flat container of
   * [items](markdown/concepts.md#item). It is a non recursive data structure, i.e. a page cannot
   * contain subpages as items, see sgl::PageLink for that. A page is responsible for navigating
   * through the items and delegating user input the the current item.
   *
   * A page consists of the following data:
   *  - a @ref NamedTuple "named tuple" of [items](markdown/concepts.md#item).
   *  - an [input handler](markdown/concepts.md#input-handler). It manages how the page gets
   * navigated and how user input is passed on to its items.
   *  - an enter and exit action/handler. These will be called by the menu when a page is entered,
   *  i.e. becomes the current page, or exited, i.e. another page becomes current. By default these
   * do nothing. More on this [here](markdown/concepts.md#page-action).
   *  - a boolean to indicate edit or navigation mode.
   *  - a start and stop edit input value. Receiving an input equal to this value set's the page
   * into edit or navigation mode respectively.
   *  - an index to keep track of the current item.
   *
   * Constructing a page is done the same way as constructing an sgl::NamedTuple, that
   * is by providing the arguments as NamedValues of items. In the example below `name1` and `name2`
   * are of type sgl::Name, and `item1` and `item2` fullfill the item concept.
   *
   * ```cpp
   *  #include "sgl/page.hpp"
   *
   *  auto page = sgl::Page(name1 <<= item1, name2 <<= item2);
   * ```
   *
   * A static assert will be triggered if two items in a page have the same name, i.e. the follwoing
   * would fail to compile:
   *
   * ```cpp
   *  #include "sgl/page.hpp"
   *
   *  auto page = sgl::Page(name1 <<= item1, name1 <<= item2, ...); // error, item1 and item2 have
   *                                                                // the same name
   * ```
   *
   * Like the sgl::NamedTuple, the page also offers possibilities to apply a fucntor on its items
   * with overloads for sgl::for_each(), and Page::for_each_item(). Additionally, any functor thath
   * can be applied with the aforementioned methods can also be called just on the current item with
   * the Page::for_current_item() method.
   *
   * @tparam NameList sgl::type_list of sgl::Name
   * @tparam ItemList sgl::type_list of item types
   */
  template <typename NameList, typename ItemList>
  class Page {
    static_assert(sgl::is_type_list_v<ItemList>,
                  "ItemList must ba an instance of sgl::type_list<Items...>");

    static_assert(sgl::is_type_list_v<NameList>,
                  "NameList must ba an instance of sgl::type_list<Names...>");

    static_assert(sgl::detail::all_are_name_types(NameList{}),
                  "All names in NameList must be an instance of sgl::Name<Chars...>");

    static_assert(sgl::all_unique_v<NameList>,
                  "sgl::Page can't have duplicate names for its items! Make sure to give every "
                  "item in the page a unique name.");

    static_assert(sgl::detail::all_same_char_type(ItemList{}),
                  "All items in a page must have the same char_type! Make sure ech item has the "
                  "expected character type.");

    using ItemTuple = sgl::NamedTuple<NameList, ItemList>;

  public:
    // the character type of the items
    using char_type = typename sgl::first_t<ItemList>::char_type;

    /**
     * @brief Concrete input handler type. A page input handler is a callable with a call signature
     * equal to sgl::error(Page&, sgl::input).[See here](markdown/concepts.md#input-handler) for
     * more info.
     */
    using InputHandler_t = Callable<error(Page&, sgl::input)>;

    /**
     * Concrete page action type. Page actions are callables which can be invoked with a reference
     * to a page and return an sgl::error value. [See here](markdown/concepts.md#page-action) for
     * more info.
     */
    using PageAction_t = sgl::Callable<sgl::error(Page&)>;

    /**
     * construct Page from named items
     * @param named_items items of the page
     */
    template <typename... Names, typename... Items>
    constexpr explicit Page(const sgl::NamedValue<Names, Items>&... named_items) noexcept(
        std::is_nothrow_copy_constructible_v<ItemTuple>);

    /**
     * construct Page from named items
     * @param named_items items of the page
     */
    template <typename... Names, typename... Items>
    constexpr explicit Page(sgl::NamedValue<Names, Items>&&... named_items) noexcept(
        std::is_nothrow_move_constructible_v<ItemTuple>);

    /**
     * get number of items in page
     * @return size_t
     */
    [[nodiscard]] constexpr size_t size() const noexcept;

    /**
     * get the index of the current item in the page. Zero based index.
     * @return size_t
     */
    [[nodiscard]] constexpr size_t current_item_index() const noexcept;

    /**
     * set the current item index. Zero based index.
     * @param i page index
     * @return Page&
     */
    constexpr Page& set_current_item(size_t i) noexcept;

    /**
     * set current item by name.
     * @tparam Cs characters of name
     * @param name item name
     * @return Page&
     */
    template <char... Cs>
    constexpr Page& set_current_item(sgl::Name<Cs...> name) noexcept;

    /**
     * handle input. Delegates to input handler.
     * @param i input to handle
     * @return sgl::error
     */
    [[nodiscard]] constexpr sgl::error handle_input(sgl::input i) noexcept;

    /**
     * execute enter action
     * @return sgl::error
     */
    [[nodiscard]] constexpr sgl::error on_enter() noexcept;

    /**
     * execute exit action
     * @return sgl::error
     */
    [[nodiscard]] constexpr sgl::error on_exit() noexcept;

    /**
     * query if page is in edit mode.
     * @return bool
     */
    [[nodiscard]] constexpr bool is_in_edit_mode() const noexcept;

    /**
     * set the page in edit mode.
     */
    constexpr void set_edit_mode() noexcept;

    /**
     * set the page into navigation mode.
     */
    constexpr void set_navigation_mode() noexcept;

    /**
     * get the start edit input value.
     * @return sgl::input
     */
    [[nodiscard]] constexpr sgl::input get_start_edit() const noexcept;

    /**
     * get the stop edit input.
     * @return sgl::input
     */
    [[nodiscard]] constexpr sgl::input get_stop_edit() const noexcept;

    /**
     * set the start edit input value
     * @param start_edit value to start edit mode on
     * @return Page&
     */
    constexpr Page& set_start_edit(sgl::input start_edit) noexcept;

    /**
     * set the sop edit input value
     * @param stop_edit value to stop edit mode on
     * @return Page&
     */
    constexpr Page& set_stop_edit(sgl::input stop_edit) noexcept;

    /**
     * @brief Set the menu for items which need it, for example sgl::PageLink.
     * @tparam Menu menu type
     * @param menu pointer to menu instance
     */
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /**
     * @brief Set enter action.
     * @tparam PageAction action type. See PageAction_t for more info.
     * @param action action instance. Must be invocable with signature sgl::error(Page&)noexcept.
     * @return Page&
     */
    template <typename Action>
    constexpr Page& set_on_enter(Action&& action) noexcept;

    /**
     * @brief Set exit action.
     * @tparam PageAction action type. See PageAction_t for more info.
     * @param action action instance. Must be invocable with signature sgl::error(Page&)noexcept.
     * @return Page&
     */
    template <typename Action>
    constexpr Page& set_on_exit(Action&& action) noexcept;

    /**
     * @brief apply f on all items in the page with the items' **declared
     * type**.
     *
     * @note f must be a generic lambda like in the example below or some kind
     * of template function taking one parameter by (const) reference.
     *
     * @code
     * auto page_var = make_page(...);
     * page_var.for_current_item([](auto& page){...});
     * @endcode
     *
     * @tparam F invocable type
     * @param f callable
     * @{
     */
    template <typename F>
    constexpr void for_each_item(F&& f);

    template <typename F>
    constexpr void for_each_item(F&& f) const;
    //

    /**
     * @brief apply f on each item in page with the items name. See NamedTuple::for_each_with_name
     * for details on the signature of f.
     * @tparam F functor type
     * @param f functor instance
     * @{
     */
    template <typename F>
    constexpr void for_each_item_with_name(F&& f);

    template <typename F>
    constexpr void for_each_item_with_name(F&& f) const;
    /// @}

    /**
     * @brief apply f on the current item. An example is below.
     *
     * @note f must be a generic lambda like in the example below or some kind
     * of template taking one parameter by (const) reference.
     *
     * @code
     * auto page_var = make_page(...);
     * page_var.for_current_item([](auto& page){...});
     * @endcode
     *
     * @tparam F invocable type
     * @param f callable
     * @return f(current_item), if f returns a non void value.
     * @{
     */
    template <typename F>
    constexpr decltype(auto) for_current_item(F&& f);

    template <typename F>
    constexpr decltype(auto) for_current_item(F&& f) const;
    /// @}

    /**
     * @brief get item by name
     * @tparam Cs characters of name
     * @param name name instance
     * @return reference to item
     */
    template <char... Cs>
    constexpr auto& operator[](sgl::Name<Cs...> name) noexcept;

    /**
     * @brief get item by name
     * @tparam Cs characters of name
     * @param name name instance
     * @return const reference to item
     */
    template <char... Cs>
    constexpr const auto& operator[](sgl::Name<Cs...> name) const noexcept;

    /// invoke the tick handler of every item contained
    constexpr void tick() noexcept;

    /**
     * @brief get the name of the i-th item as a string_view
     * @param i item index
     * @return sgl::string_view<char>
     */
    constexpr sgl::string_view<char> item_name(size_t i) const noexcept;

    /**
     * @brief get the text of the i-th item as a string_view
     * @param i item index
     * @return sgl::string_view<char_type>
     */
    constexpr sgl::string_view<char_type> item_text(size_t i) const noexcept;

  private:
    // default input handler
    [[nodiscard]] static constexpr sgl::error default_handle_input(Page&      page,
                                                                   sgl::input i) noexcept;

    // default page action, does nothing
    [[nodiscard]] constexpr static sgl::error default_page_action(Page& page) noexcept;

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_item_impl(F&& f);

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_item_impl(F&& f) const;

    template <size_t I>
    constexpr sgl::string_view<char> item_name_impl(size_t i) const noexcept;

    template <size_t I>
    constexpr sgl::string_view<char_type> item_text_impl(size_t i) const noexcept;

    ItemTuple      items_;                                ///< storage for the items
    InputHandler_t input_handler_{&default_handle_input}; ///< page input handler
    PageAction_t   on_enter_{&default_page_action}; ///< action to execute when page is entered
    PageAction_t   on_exit_{&default_page_action};
    ///< action to execute when page is exited
    sgl::input start_edit_{sgl::input::enter}; ///< page will switch to edit mode when an input
                                               ///< equal to start_edit_ is given
    sgl::input stop_edit_{sgl::input::enter};  ///< page will switch to navigation
    bool       elem_in_edit_{false};           ///< indicates navigation(false ) /edit(true) mode.
    sgl::smallest_type_t<sgl::list_size_v<ItemList>> index_{0}; ///< index of the current ite
  };

  /// @cond
  // deduction guides for Page
  template <typename... Names, typename... Ts>
  Page(NamedValue<Names, Ts>&&... elems) -> Page<sgl::type_list<Names...>, sgl::type_list<Ts...>>;
  // deduction guides for Page
  template <typename... Names, typename... Ts>
  Page(const NamedValue<Names, Ts>&... elems)
      -> Page<sgl::type_list<Names...>, sgl::type_list<Ts...>>;
  /// @endcond

  /**
   * @brief apply f on each item in a page. This will call f with a reference to the item for each
   * item.
   * @tparam NameList name list of page
   * @tparam ItemList type list of page
   * @tparam F functor type
   * @param page page instance
   * @param f functor instance
   */
  template <typename NameList, typename ItemList, typename F>
  void for_each(sgl::Page<NameList, ItemList>& page, F&& f);

  /**
   * @brief apply f on each item in a page. This will call f with a const reference to the item for
   * each item.
   * @tparam NameList name list of page
   * @tparam ItemList type list of page
   * @tparam F functor type
   * @param page page instance
   * @param f functor instance
   */
  template <typename NameList, typename ItemList, typename F>
  void for_each(const sgl::Page<NameList, ItemList>& page, F&& f);

  /**
   * @brief apply f on each item in a page. f will be called with the name of the item as an
   * sgl::string_view<char> as the first argument and a reference to the corresponding item as
   * the second argument.
   * @tparam NameList name list of page
   * @tparam ItemList type list of page
   * @tparam F functor type
   * @param page page instance
   * @param f functor instance
   */
  template <typename NameList, typename ItemList, typename F>
  void for_each_with_name(sgl::Page<NameList, ItemList>& page, F&& f);

  /**
   * @brief apply f on each item in a page. f will be called with the name of the item as an
   * sgl::string_view<char> as the first argument and a const reference to the corresponding item
   * as the second argument. @tparam NameList name list of page @tparam ItemList type list of page
   * @tparam F functor type
   * @param page page instance
   * @param f functor instance
   */
  template <typename NameList, typename ItemList, typename F>
  void for_each_with_name(const sgl::Page<NameList, ItemList>& page, F&& f);

  /**
   * @brief apply functor f on the current item of a page. This will call f with a reference to the
   * current item of the page.
   * @tparam NameList name list of page
   * @tparam ItemList type list of page
   * @tparam F functor type
   * @param page page instance
   * @param f functor instance
   * @return returns the return value of f.
   */
  template <typename NameList, typename ItemList, typename F>
  decltype(auto) for_current(sgl::Page<NameList, ItemList>& page, F&& f);

  /**
   * @brief apply functor f on the current item of a page. This will call f with a const reference
   * to the current item of the page.
   * @tparam NameList name list of page
   * @tparam ItemList type list of page
   * @tparam F functor type
   * @param page page instance
   * @param f functor instance
   * @return returns the return value of f.
   */
  template <typename NameList, typename ItemList, typename F>
  decltype(auto) for_current(const sgl::Page<NameList, ItemList>& page, F&& f);

} // namespace sgl

#include "sgl/impl/page_impl.hpp"
#endif /* SGL_PAGE_HPP */
