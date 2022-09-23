#ifndef MENU_HPP
#define MENU_HPP
#include "sgl/callable.hpp"
#include "sgl/error.hpp"
#include "sgl/input.hpp"
#include "sgl/named_tuple.hpp"
#include "sgl/smallest_type.hpp"

namespace sgl {
  /// \cond
  template <typename NameList, typename PageTypeList>
  class Menu;
  /// \endcond

  /// \brief The menu class brings everything together and holds all the data of the menu. It is
  /// essentially a named tuple of pages.
  /// \tparam Names names of the pages
  /// \tparam Pages types of the pages
  template <typename... Names, typename... Pages>
  class Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>> {
  public:
    /// \brief is true if F is nothrow invocable with a reference to each page in page_list
    /// \tparam F functor type
    template <typename F>
    static constexpr bool nothrow_applicable = (std::is_nothrow_invocable_v<F, Pages&> && ...);

    /// \brief is true if F is nothrow invocable with a const reference to each page in page_list
    /// \tparam F functor type
    template <typename F>
    static constexpr bool
        const_nothrow_applicable = (std::is_nothrow_invocable_v<F, const Pages&> && ...);

    /// \brief is true if F is nothrow invocable with a string_view<char> as it's first argument and
    /// a reference to each page in page_list as it's second.
    /// \tparam F functor type
    template <typename F>
    static constexpr bool nothrow_applicable_with_name =
        (std::is_nothrow_invocable_v<F, sgl::string_view<char>, Pages&> && ...);

    /// \brief is true if F is nothrow invocable with a string_view<char> as it's first argument and
    /// a const reference to each page in page_list as it's second.
    /// \tparam F functor type
    template <typename F>
    static constexpr bool const_nothrow_applicable_with_name =
        (std::is_nothrow_invocable_v<F, sgl::string_view<char>, const Pages&> && ...);

    /// page names of the menu
    using name_list = sgl::type_list<Names...>;
    /// page types of the menu
    using page_list = sgl::type_list<Pages...>;
    /// concrete input handler type
    using InputHandler_t =
        sgl::Callable<sgl::error(sgl::Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>&,
                                 sgl::Input)>;

    /// \brief true if menu is nothrow copy constructible
    static constexpr bool nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<
        sgl::NamedTuple<type_list<Names...>, type_list<Pages...>>>;

    /// \brief true if menu is nothrow move constructible
    static constexpr bool nothrow_move_constructible = std::is_nothrow_move_constructible_v<
        sgl::NamedTuple<type_list<Names...>, type_list<Pages...>>>;

    /// \brief copy ctor
    /// \param other menu to copy
    constexpr Menu(const Menu& other) noexcept(nothrow_copy_constructible);

    /// \brief move ctor
    /// \param other menu to move from
    constexpr Menu(Menu&& other) noexcept(nothrow_move_constructible);

    /// \brief construct menu from named pages
    /// \param pages pages as named arguments.
    constexpr explicit Menu(const sgl::NamedValue<Names, Pages>&... pages) noexcept(
        nothrow_copy_constructible);

    /// \brief construct menu from named pages
    /// \param pages pages as named arguments.
    constexpr explicit Menu(sgl::NamedValue<Names, Pages>&&... pages) noexcept(
        nothrow_move_constructible);

    /// invoke the menu's input handler
    /// \param i input to handle
    /// \return sgl::error
    [[nodiscard]] sgl::error handle_input(sgl::Input i) noexcept;

    /// invoke tick() method for each item in the menu.
    /// \note Keep in mind that this function call can take a non negligible time to complete if you
    /// have a lot of tick handlers doing (maybe expensive) work, so don't call it in an IRQ!
    /// \see item_tick_handling
    constexpr void tick() noexcept;

    /// get the index of the currently active page
    /// \return size_t
    [[nodiscard]] constexpr size_t current_page_index() const noexcept;

    /// get number of pages stored in the menu
    /// \return size_t
    [[nodiscard]] constexpr size_t size() const noexcept;

    /// set current page by index
    /// \param page_index zero based index of the page
    /// \return sgl::error
    [[nodiscard]] constexpr sgl::error set_current_page(size_t page_index) noexcept;

    /// set current page by name
    /// \param name name of the page
    /// \return sgl::error
    template <typename Name>
    [[nodiscard]] constexpr sgl::error set_current_page(Name name) noexcept;

    /// \brief get page by name
    /// \tparam Name name type
    /// \param name name instance
    /// \return reference to page
    template <typename Name>
    [[nodiscard]] auto& operator[](Name name) noexcept;

    /// \brief get page by name
    /// \tparam Name name type
    /// \param name name instance
    /// \return const reference to page
    template <typename Name>
    [[nodiscard]] const auto& operator[](Name name) const noexcept;

    /// \brief get reference to page at index I
    /// \tparam I page index
    /// \return reference to page
    template <size_t I>
    [[nodiscard]] constexpr auto& get_page() noexcept;

    /// \brief get const reference to page at index I
    /// \tparam I page index
    /// \return const reference to page
    template <size_t I>
    [[nodiscard]] constexpr const auto& get_page() const noexcept;

    /// \brief apply f on each page in menu.
    /// \details
    /// \code
    /// // defined somewhere in a header
    /// template<typename Page>
    /// void global_func(Page& page){...};
    ///
    /// // some where a menu instance called menu used in a function
    /// menu.for_each_page(global_func);
    /// // or with a generic lambda
    /// menu.for_each_page([](auto& page){...});
    ///
    /// \endcode
    ///
    /// \tparam F functor type
    /// \param f functor
    /// \{
    template <typename F>
    constexpr void for_each_page(F&& f) noexcept(nothrow_applicable<F>);

    template <typename F>
    constexpr void for_each_page(F&& f) const noexcept(const_nothrow_applicable<F>);
    /// \}

    /// \brief apply f on each page in menu.
    /// \details
    /// \code
    /// // defined somewhere in a header
    /// template<typename Page>
    /// void global_func(sgl::string_view<char> name, Page& page){...};
    ///
    /// // some where a menu instance called menu used in a function
    /// menu.for_each_page(global_func);
    /// // or with a generic lambda
    /// menu.for_each_page([](sgl::string_view<char> name, auto& page){...});
    ///
    /// \endcode
    ///
    /// \tparam F functor type
    /// \param f functor
    /// \{
    template <typename F>
    constexpr void for_each_page_with_name(F&& f) noexcept(nothrow_applicable<F>) {
      sgl::for_each_with_name(pages_, std::forward<F>(f));
    }

    template <typename F>
    constexpr void for_each_page_with_name(F&& f) const noexcept(const_nothrow_applicable<F>) {
      sgl::for_each_with_name(pages_, std::forward<F>(f));
    }
    /// \}

    /// \brief apply f on the current page.
    /// \tparam F functor type
    /// \param f functor instance
    /// \return returns f(current_item), if f returns a non-void value. Else the return type is
    /// void.
    /// \{
    template <typename F>
    constexpr decltype(auto) for_current_page(F&& f) noexcept(nothrow_applicable<F>);

    template <typename F>
    constexpr decltype(auto) for_current_page(F&& f) const noexcept(const_nothrow_applicable<F>);
    /// \}

  private:
    [[nodiscard]] constexpr static sgl::error default_handle_input(Menu& menu,
                                                                   Input input) noexcept;

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_page_impl(F&& f) noexcept(nothrow_applicable<F>);

    template <size_t I, typename F>
    constexpr decltype(auto) for_current_page_impl(F&& f) const
        noexcept(const_nothrow_applicable<F>);

    sgl::NamedTuple<name_list, page_list> pages_;        ///< pages owned by this menu
    InputHandler_t input_handler_{&default_handle_input}; ///< menu input handler
    sgl::smallest_type_t<sizeof...(Pages)> index_{0};     ///< index of current page
  };

  /// \brief template argument deduction guide for Menu
  /// \tparam Names names of pages
  /// \tparam Ts page types
  template <typename... Names, typename... Ts>
  Menu(NamedValue<Names, Ts>&&...) -> Menu<sgl::type_list<Names...>, sgl::type_list<Ts...>>;

  /// \brief template argument deduction guide for Menu
  /// \tparam Names names of pages
  /// \tparam Ts page types
  template <typename... Names, typename... Ts>
  Menu(const NamedValue<Names, Ts>&...) -> Menu<sgl::type_list<Names...>, sgl::type_list<Ts...>>;

  /// \brief apply f on each page in menu. Same as sgl::for_each(NamedTuple&,F&&).
  /// \tparam NameList list of page names
  /// \tparam PageList list of page types
  /// \tparam F functor type
  /// \param menu menu instance
  /// \param f functor instance
  /// \{
  template <typename NameList, typename PageList, typename F>
  void for_each(Menu<NameList, PageList>& menu, F&& f) noexcept(
      noexcept(std::declval<Menu<NameList, PageList>>().for_each_page(std::declval<F>())));

  template <typename NameList, typename PageList, typename F>
  void for_each(const Menu<NameList, PageList>& menu, F&& f) noexcept(
      noexcept(std::declval<Menu<NameList, PageList>>().for_each_page(std::declval<F>())));
  /// \}

  /// \brief apply f on each page of the menu, with the name of the page. See Menu<NameList,
  /// PageList>::for_each_page_with_name for more details.
  /// \tparam NameList list of page names
  /// \tparam PageList list of page types
  /// \tparam F functor type
  /// \param menu menu instance
  /// \param f functor instance
  /// \{
  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(Menu<NameList, PageList>& menu, F&& f) noexcept(noexcept(
      std::declval<Menu<NameList, PageList>>().for_each_page_with_name(std::declval<F>())));

  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(const Menu<NameList, PageList>& menu, F&& f) noexcept(noexcept(
      std::declval<const Menu<NameList, PageList>>().for_each_page_with_name(std::declval<F>())));
  /// \}
} // namespace sgl
#include "sgl/impl/menu_impl.hpp"
#endif