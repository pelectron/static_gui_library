/**
 * \file sgl/page.hpp
 *
 * \author Pelé Constam (pelectron1602@gmail.com)
 * \brief This file contains the sgl::Page class template and the page iteration functions
 * sgl::for_each(), sgl::for_each_with_name() and sgl::for_current().
 *
 * \version 0.1
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
#include "sgl/input.hpp"
#include "sgl/item_concepts.hpp"
#include "sgl/named_tuple.hpp"
#include "sgl/smallest_type.hpp"
#include "sgl/static_string.hpp"

namespace sgl {
  /// \cond
  // this is an intentional forward declaration. Dont remove.
  template <typename ItemNameList, typename ItemTypeList>
  class Page;

  /// \endcond

  /**
   * \headerfile page.hpp "sgl/page.hpp"
   * \brief This class represents a page of a menu. It is a non recursive data structure, i.e. a
   * page cannot contain a subpage. A page is responsible for navigating through the items and
   * delegating user input the the current item.
   *
   * \details A page consists of the following data:
   *  - a \ref sgl::NamedTuple "named tuple" of items.
   *  - an [input handler](markdown/concepts.md#input-handler). It manages how the page gets
   * navigated and how user input is passed on to its items.
   *  - an enter and exit action/handler. These will be called by the menu when a page is entered,
   *  i.e. becomes the current page, or exited, i.e. get's switched from. By default these do
   *  nothing. More on this [here](markdown/concepts.md#page-action).
   *  - a boolean to indicate edit or navigation mode.
   *  - a start and stop edit input value. Receiving an input equal to this value set's the page
   * into edit or navigation mode respectively.
   *  - an index to keep track of the current page.
   *
   *
   * \tparam Names names of the items
   * \tparam Items item types
   */
  template <typename... Names, typename... Items>
  class Page<sgl::type_list<Names...>, sgl::type_list<Items...>> {
    template <typename F>
    static constexpr bool nothrow_applicable = noexcept(
        sgl::for_each(std::declval<sgl::NamedTuple<type_list<Names...>, type_list<Items...>>>(),
                      std::declval<F>()));
    template <typename F>
    static constexpr bool const_nothrow_applicable = noexcept(sgl::for_each(
        std::declval<const sgl::NamedTuple<type_list<Names...>, type_list<Items...>>>(),
        std::declval<F>()));
    template <typename F>
    static constexpr bool nothrow_applicable_with_name = noexcept(sgl::for_each_with_name(
        std::declval<sgl::NamedTuple<type_list<Names...>, type_list<Items...>>>(),
        std::declval<F>()));
    template <typename F>
    static constexpr bool const_nothrow_applicable_with_name = noexcept(sgl::for_each_with_name(
        std::declval<const sgl::NamedTuple<type_list<Names...>, type_list<Items...>>>(),
        std::declval<F>()));
    static constexpr bool nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<
        sgl::NamedTuple<type_list<Names...>, type_list<Items...>>>;
    static constexpr bool nothrow_move_constructible = std::is_nothrow_move_constructible_v<
        sgl::NamedTuple<type_list<Names...>, type_list<Items...>>>;

  public:
    static_assert((sgl::is_name_type_v<Names> && ...),
                  "One of the types begin used as a name is not instance of sgl::Name<...>.");
    static_assert(sgl::all_unique_v<Names...>,
                  "sgl::Page can't have duplicate names for its items! Make sure to give every "
                  "item in the page a unique name.");

    /// type list of item types
    using item_list = sgl::type_list<Items...>;

    /// type list of the item names
    using name_list = sgl::type_list<Names...>;

    /// \brief Concrete input handler type. A page input handler is a callable with a call signature
    /// equal to sgl::error(Page&, sgl::Input).[See here](markdown/concepts.md#input-handler) for
    /// more info.
    using InputHandler_t = Callable<error(Page&, sgl::Input)>;

    /// Concrete page action type. Page actions are callables which can be invoked with a reference
    /// to a page and return an sgl::error value. [See here](markdown/concepts.md#page-action) for
    /// more info.
    using PageAction_t = sgl::Callable<sgl::error(Page&)>;

    /// construct Page from named items
    /// \param items items of the page
    constexpr explicit Page(const sgl::NamedValue<Names, Items>&... items) noexcept(
        nothrow_copy_constructible);

    /// construct Page from named items
    /// \param items items to be moved into the page
    constexpr explicit Page(sgl::NamedValue<Names, Items>&&... items) noexcept(
        nothrow_move_constructible);

    /// get number of items in page
    /// \return size_t
    [[nodiscard]] constexpr size_t size() const noexcept;

    /// get the index of the current item in the page. Zero based index.
    /// \return size_t
    [[nodiscard]] constexpr size_t current_item_index() const noexcept;

    /// set the current item index. Zero based index.
    /// \param i page index
    /// \return Page&
    constexpr Page& set_current_item(size_t i) noexcept;

    /// set current item by name.
    /// \tparam Name characters of name
    /// \param name item name
    /// \return Page&
    template <char... Cs>
    constexpr Page& set_current_item(sgl::Name<Cs...> name) noexcept;

    /// handle input. Delegates to input handler.
    /// \param i input to handle
    /// \return sgl::error
    [[nodiscard]] constexpr sgl::error handle_input(sgl::Input i) noexcept;

    /// execute enter action
    /// \return sgl::error
    [[nodiscard]] constexpr sgl::error on_enter() noexcept;

    /// execute exit action
    /// \return sgl::error
    [[nodiscard]] constexpr sgl::error on_exit() noexcept;

    /// query if page is in edit mode.
    /// \return bool
    [[nodiscard]] constexpr bool is_in_edit_mode() const noexcept;

    /// set the page in edit mode.
    constexpr void set_edit_mode() noexcept;

    /// set the page into navigation mode.
    constexpr void set_navigation_mode() noexcept;

    /// get the start edit input value.
    /// \return sgl::Input
    [[nodiscard]] constexpr sgl::Input get_start_edit() const noexcept;

    /// get the stop edit input.
    /// \return sgl::Input
    [[nodiscard]] constexpr sgl::Input get_stop_edit() const noexcept;

    /// set the start edit input value
    /// \param start_edit value to start edit mode on
    /// \return Page&
    constexpr Page& set_start_edit(sgl::Input start_edit) noexcept;

    /// set the sop edit input value
    /// \param stop_edit value to stop edit mode on
    /// \return Page&
    constexpr Page& set_stop_edit(sgl::Input stop_edit) noexcept;

    /// \brief Set the menu for items which need it, for example sgl::PageLink.
    /// \tparam Menu menu type
    /// \param menu pointer to menu instance
    ////
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /// \brief Set enter action.
    /// \tparam PageAction action type. See PageAction_t for more info.
    /// \param action action instance
    /// \return Page&
    template <typename Action>
    constexpr Page& set_on_enter(Action&& action) noexcept;

    /// \brief Set exit action.
    /// \tparam PageAction action type. See PageAction_t for more info.
    /// \param action action instance
    /// \return Page&
    template <typename Action>
    constexpr Page& set_on_exit(Action&& action) noexcept;

    /// \brief apply f on all items in the page with the items' **declared
    /// type**.
    ///
    /// \note f must be a generic lambda like in the example below or some kind
    /// of template function taking one parameter by (const) reference.
    ///
    /// \code
    /// auto page_var = make_page(...);
    /// page_var.for_current_item([](auto& page){...});
    /// \endcode
    ///
    /// \tparam F invocable type
    /// \param f callable
    /// \{
    template <typename F>
    constexpr void for_each_item(F&& f) noexcept(nothrow_applicable<F>);

    template <typename F>
    constexpr void for_each_item(F&& f) const noexcept(const_nothrow_applicable<F>);
    /// \}

    /// \brief apply f on each item in page with the items name. See NamedTuple::for_each_with_name
    /// for details on the signature of f.
    /// \tparam F functor type
    /// \param f functor instance
    /// \{
    template <typename F>
    constexpr void for_each_item_with_name(F&& f) noexcept(nothrow_applicable_with_name<F>);

    template <typename F>
    constexpr void for_each_item_with_name(F&& f) const
        noexcept(const_nothrow_applicable_with_name<F>);
    /// \}

    /// \brief apply f on the current item. An example is below.
    ///
    /// \note f must be a generic lambda like in the example below or some kind
    /// of template taking one parameter by (const) reference.
    ///
    /// \code
    /// auto page_var = make_page(...);
    /// page_var.for_current_item([](auto& page){...});
    /// \endcode
    ///
    /// \tparam F invocable type
    /// \param f callable
    /// \return f(current_item), if f returns a non void value.
    /// \{
    template <typename F>
    constexpr decltype(auto) for_current_item(F&& f) noexcept(nothrow_applicable<F>);

    template <typename F>
    constexpr decltype(auto) for_current_item(F&& f) const noexcept(const_nothrow_applicable<F>);
    /// \}

    /// \brief get item by name
    /// \tparam Name characters of name
    /// \param name name instance
    /// \return reference to item
    ////
    template <char... Cs>
    constexpr auto& operator[](sgl::Name<Cs...> name) noexcept;

    /// \brief get item by name
    /// \tparam Cs characters of name
    /// \param name name instance
    /// \return const reference to item
    ////
    template <char... Cs>
    constexpr const auto& operator[](sgl::Name<Cs...> name) const noexcept;

    /// invoke the tick handler of every item contained
    constexpr void tick() noexcept;

  private:
    /// default input handler
    [[nodiscard]] static constexpr sgl::error default_handle_input(Page&      page,
                                                                   sgl::Input i) noexcept;

    /// default page action, does nothing
    [[nodiscard]] constexpr static sgl::error default_page_action(Page& page) noexcept;

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_item_impl(F&& f) noexcept(nothrow_applicable<F>);

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_item_impl(F&& f) const
        noexcept(const_nothrow_applicable<F>);

    sgl::NamedTuple<sgl::type_list<Names...>, sgl::type_list<Items...>>
                   items_;                                ///< storage for the items
    InputHandler_t input_handler_{&default_handle_input}; ///< page input handler
    PageAction_t   on_enter_{&default_page_action}; ///< action to execute when page is entered.
    PageAction_t   on_exit_{&default_page_action};  ///< action to execute when page is exited
    sgl::Input     start_edit_{sgl::Input::enter};  ///< page will switch to edit mode when an input
                                                    ///< equal to start_edit_ is given.
    sgl::Input stop_edit_{sgl::Input::enter}; ///< page will switch to navigation mode when an input
                                              ///< equal to stop_edit_ is given.
    bool elem_in_edit_{false};                ///< indicates navigation(false)/edit(true) mode.
    sgl::smallest_type_t<sizeof...(Items)> index_{0}; ///< index of the current item.
  };

  template <typename... Names, typename... Ts>
  Page(NamedValue<Names, Ts>&&... elems) -> Page<sgl::type_list<Names...>, sgl::type_list<Ts...>>;

  template <typename... Names, typename... Ts>
  Page(const NamedValue<Names, Ts>&... elems)
      -> Page<sgl::type_list<Names...>, sgl::type_list<Ts...>>;

  /// \brief apply f on each item in a page. This will call f with a reference to the item for each
  /// item.
  /// \tparam NameList name list of page
  /// \tparam ItemList type list of page
  /// \tparam F functor type
  /// \param page page instance
  /// \param f functor instance
  template <typename NameList, typename ItemList, typename F>
  void for_each(sgl::Page<NameList, ItemList>& page,
                F&& f) noexcept(noexcept(page.template for_each_item(std::forward<F>(f))));

  /// \brief apply f on each item in a page. This will call f with a const reference to the item for
  /// each item.
  /// \tparam NameList name list of page
  /// \tparam ItemList type list of page
  /// \tparam F functor type
  /// \param page page instance
  /// \param f functor instance
  template <typename NameList, typename ItemList, typename F>
  void for_each(const sgl::Page<NameList, ItemList>& page,
                F&& f) noexcept(noexcept(page.template for_each_item(std::forward<F>(f))));

  /// \brief apply f on each item in a page. f will be called with the name of the item as an
  /// sgl::string_view<char> as the first argument and a reference to the corresponding item as
  /// the second argument.
  /// \tparam NameList name list of page
  /// \tparam ItemList type list of page
  /// \tparam F functor type
  /// \param page page instance
  /// \param f functor instance
  template <typename NameList, typename ItemList, typename F>
  void for_each_with_name(sgl::Page<NameList, ItemList>& page, F&& f) noexcept(
      noexcept(page.template for_each_item_with_name(std::forward<F>(f))));

  /// \brief apply f on each item in a page. f will be called with the name of the item as an
  /// sgl::string_view<char> as the first argument and a const reference to the corresponding item
  /// as the second argument. \tparam NameList name list of page \tparam ItemList type list of page
  /// \tparam F functor type
  /// \param page page instance
  /// \param f functor instance
  template <typename NameList, typename ItemList, typename F>
  void for_each_with_name(const sgl::Page<NameList, ItemList>& page, F&& f) noexcept(
      noexcept(page.template for_each_item_with_name(std::forward<F>(f))));

  /// \brief apply functor f on the current item of a page. This will call f with a reference to the
  /// current item of the page.
  /// \tparam NameList name list of page
  /// \tparam ItemList type list of page
  /// \tparam F functor type
  /// \param page page instance
  /// \param f functor instance
  /// \return returns the return value of f.
  template <typename NameList, typename ItemList, typename F>
  decltype(auto) for_current(sgl::Page<NameList, ItemList>& page, F&& f) noexcept(
      noexcept(std::declval<Page<NameList, ItemList>>().for_current_item(std::forward<F>(f))));

  /// \brief apply functor f on the current item of a page. This will call f with a const reference
  /// to the current item of the page.
  /// \tparam NameList name list of page
  /// \tparam ItemList type list of page
  /// \tparam F functor type
  /// \param page page instance
  /// \param f functor instance
  /// \return returns the return value of f.
  template <typename NameList, typename ItemList, typename F>
  decltype(auto) for_current(const sgl::Page<NameList, ItemList>& page, F&& f) noexcept(noexcept(
      std::declval<const Page<NameList, ItemList>>().for_current_item(std::forward<F>(f))));

} // namespace sgl

#include "sgl/impl/page_impl.hpp"
#endif /* SGL_PAGE_HPP */
