#ifndef SGL_MENU_HPP
#define SGL_MENU_HPP
#include "item_traits.hpp"
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_smallest_type.hpp"
#include "sgl_string_view.hpp"
#include "sgl_traits.hpp"
#include "sgl_tuple.hpp"

namespace sgl {

  template <typename... Pages>
  class Menu {
  public:
    static_assert((sgl::is_page_v<Pages> && ...), "");
    using InputHandler_t = Callable<error(Menu<Pages...>&, Input)>;

    /// returns type at index I of Pages...
    template <size_t I>
    using page_at_t = type_at_t<I, type_list<Pages...>>;
    /// get item index I from page with index P
    template <size_t P, size_t I>
    using item_at_t = typename page_at_t<P>::template item_at_t<I>;

    template <typename... P>
    static constexpr bool
        nothrow_constructible_v = (sgl::is_nothrow_constructible_v<sgl::decay_t<P>, P> && ...);
    // constructors
    /**
     * @brief Construct a Menu with pages. The active page will be the first parameter passed to the constructor.
     * @param pages page of the menu
     * @{
     */
    constexpr Menu(Pages&&... pages) noexcept((is_nothrow_move_constructible_v<Pages> && ...));

    constexpr Menu(const Pages&... pages) noexcept((is_nothrow_copy_constructible_v<Pages> && ...));
    /// @}

    /**
     * @brief Construct a Menu with pages and custom input handler. The active page will be the first parameter passed to the constructor.
     * @tparam InputHandler input handler type
     * @param handler input handler instance.
     * @param pages pages of the menu
     * @{
     */
    template <typename InputHandler,
              sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>> = true>
    constexpr Menu(InputHandler&& handler,
                   Pages&&... pages) noexcept((is_nothrow_move_constructible_v<Pages> && ...));

    template <typename InputHandler,
              sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>> = true>
    constexpr Menu(InputHandler&& handler,
                   const Pages&... pages) noexcept((is_nothrow_move_constructible_v<Pages> && ...));
    /// @}

    /// copy ctor
    constexpr Menu(const Menu& other) noexcept((is_nothrow_copy_constructible_v<Pages> && ...));

    /// move ctor
    constexpr Menu(Menu&& other) noexcept((is_nothrow_move_constructible_v<Pages> && ...));

    /**
     * @brief process input
     * @param input user input
     * @return sgl::error
     */
    constexpr sgl::error handle_input(Input input) noexcept;

    /**
     * @brief Set the active page by name
     * @param page_name name of the page
     * @return sgl::error
     */
    template <typename CharT>
    constexpr sgl::error set_active_page(sgl::string_view<CharT> page_name) noexcept;

    /**
     * @brief Set the active page by index
     * @param page_index page index
     * @return sgl::error::invalid_page_index
     */
    constexpr sgl::error set_active_page(size_t page_index) noexcept;

    /**
     * @brief get reference to page at index I
     * @tparam I page index
     */
    template <size_t I>
    constexpr page_at_t<I>& get_page() noexcept;

    /**
     * @brief get const reference to page at index I
     * @tparam I page index
     */
    template <size_t I>
    constexpr const page_at_t<I>& get_page() const noexcept;

    /// get the index of the currently active page
    constexpr size_t index() const noexcept;

    /// get number of pages stored in the menu
    constexpr size_t size() const noexcept;

    /**
     * @brief get reference to item at ItemIndex from page at PageIndex.
     * @details Works like multi array indexing.
     * @code
     * // gets the third element in the first page.
     * auto& page = menu.get_page<0,2>();
     * @endcode
     *
     * @tparam PageIndex page index in menu
     * @tparam ItemIndex item index in page
     * @return reference to item at ItemIndex from page at PageIndex.
     */
    template <size_t PageIndex, size_t ItemIndex>
    constexpr item_at_t<PageIndex, ItemIndex>& get_item() noexcept;

    /**
     * @brief get const reference to item at ItemIndex from page at PageIndex.
     * @details Works like multi array indexing.
     * @code
     * // gets the third element in the first page.
     * const auto& page = menu.get_page<0,2>();
     * @endcode
     *
     * @tparam PageIndex page index in menu
     * @tparam ItemIndex item index in page
     * @return const item_at_t<PageIndex, ItemIndex>&
     */
    template <size_t PageIndex, size_t ItemIndex>
    constexpr const item_at_t<PageIndex, ItemIndex>& get_item() const noexcept;

    /**
     * @brief apply f on each page in menu.
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
     * @param f function template
     */
    template <typename F>
    constexpr void for_each_page(F&& f) noexcept(noexcept(f));

    /**
     * @brief apply f on each page in menu.
     * @details
     * @code
     * // defined somewhere in a header
     * template<typename Page>
     * void global_func(const Page& page){...};
     *
     * // some where a menu instance called menu used in a function
     * menu.for_each_page(global_func);
     * // or with a generic lambda
     * menu.for_each_page([](const auto& page){...});
     *
     * @endcode
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F>
    constexpr void for_each_page(F&& f) const noexcept(noexcept(f));

    /**
     * @brief apply f on the current page.
     * @tparam F functor type
     * @param f functor instance
     * @{
     */
    template <typename F>
    constexpr void for_current_page(F&& f) noexcept(noexcept(f));

    template <typename F>
    constexpr void for_current_page(F&& f) const noexcept(noexcept(f));
    /// @}

  private:
    constexpr static sgl::error default_handle_input(Menu<Pages...>& menu, Input input) noexcept;

    template <size_t I, typename CharT>
    constexpr sgl::error set_active_page_impl(sgl::string_view<CharT> page_name) noexcept;

    constexpr void set_start_edit_for_pages(sgl::Input start_edit) noexcept;
    constexpr void set_stop_edit_for_pages(sgl::Input stop_edit) noexcept;

    template <size_t I, typename F>
    constexpr void for_current_page_impl(F&& f) noexcept(noexcept(f));

    template <size_t I, typename F>
    constexpr void for_current_page_impl(F&& f) const noexcept(noexcept(f));

    sgl::tuple<Pages...>                   pages_;
    InputHandler_t                         input_handler_{&default_handle_input};
    sgl::smallest_type_t<sizeof...(Pages)> index_{0};
  };

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(Pages&&... pages) noexcept(
      (is_nothrow_move_constructible_v<Pages> && ...))
      : pages_(move(pages)...) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(const Pages&... pages) noexcept(
      (is_nothrow_copy_constructible_v<Pages> && ...))
      : pages_(pages...) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  template <typename... Pages>
  template <typename InputHandler, sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>>>
  constexpr Menu<Pages...>::Menu(InputHandler&& handler, Pages&&... pages) noexcept(
      (is_nothrow_move_constructible_v<Pages> && ...))
      : Menu<Pages...>(move(pages)...), input_handler_(forward<InputHandler>(handler)) {}

  template <typename... Pages>
  template <typename InputHandler, sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>>>
  constexpr Menu<Pages...>::Menu(InputHandler&& handler, const Pages&... pages) noexcept(
      (is_nothrow_move_constructible_v<Pages> && ...))
      : Menu<Pages...>(pages...), input_handler_(forward<InputHandler>(handler)) {}

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(const Menu& other) noexcept(
      (is_nothrow_copy_constructible_v<Pages> && ...))
      : pages_(other.pages_), input_handler_(other.input_handler_), index_(other.index_) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(Menu&& other) noexcept((is_nothrow_move_constructible_v<Pages> &&
                                                         ...))
      : pages_(move(other.pages_)), input_handler_(move(other.input_handler_)),
        index_(move(other.index_)) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  template <typename... Pages>
  constexpr sgl::error Menu<Pages...>::handle_input(Input input) noexcept {
    return input_handler_(*this, input);
  }

  template <typename... Pages>
  template <typename CharT>
  constexpr sgl::error Menu<Pages...>::set_active_page(sgl::string_view<CharT> page_name) noexcept {
    return set_active_page_impl<0>(page_name);
  }

  template <typename... Pages>
  constexpr sgl::error Menu<Pages...>::set_active_page(size_t page_index) noexcept {
    if (page_index < sizeof...(Pages)) {
      sgl::error ec{sgl::error::no_error};
      for_current_page([&ec](auto& page) noexcept { ec = page.on_exit(); });
      if (ec != sgl::error::no_error)
        return ec;
      index_ = page_index;
      for_current_page([&ec](auto& page) noexcept { ec = page.on_enter(); });
      return ec;
    } else
      return sgl::error::invalid_page_index;
  }

  template <typename... Pages>
  template <size_t I>
  constexpr typename Menu<Pages...>::template page_at_t<I>& Menu<Pages...>::get_page() noexcept {
    return sgl::get<I>(pages_);
  }

  template <typename... Pages>
  template <size_t I>
  constexpr const typename Menu<Pages...>::template page_at_t<I>&
      Menu<Pages...>::get_page() const noexcept {
    return sgl::get<I>(pages_);
  }

  template <typename... Pages>
  constexpr size_t Menu<Pages...>::index() const noexcept {
    return index_;
  }

  template <typename... Pages>
  constexpr size_t Menu<Pages...>::size() const noexcept {
    return sizeof...(Pages);
  }

  template <typename... Pages>
  template <size_t PageIndex, size_t ItemIndex>
  constexpr typename Menu<Pages...>::template item_at_t<PageIndex, ItemIndex>&
      Menu<Pages...>::get_item() noexcept {
    return sgl::get<ItemIndex>(sgl::get<PageIndex>(pages_));
  }

  template <typename... Pages>
  template <size_t PageIndex, size_t ItemIndex>
  constexpr const typename Menu<Pages...>::template item_at_t<PageIndex, ItemIndex>&
      Menu<Pages...>::get_item() const noexcept {
    return sgl::get<ItemIndex>(sgl::get<PageIndex>(pages_));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_each_page(F&& f) noexcept(noexcept(f)) {
    sgl::for_each(pages_, forward<F>(f));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_each_page(F&& f) const noexcept(noexcept(f)) {
    sgl::for_each(pages_, forward<F>(f));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_current_page(F&& f) noexcept(noexcept(f)) {
    for_current_page_impl<0>(forward<F>(f));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_current_page(F&& f) const noexcept(noexcept(f)) {
    for_current_page_impl<0>(forward<F>(f));
  }

  template <typename... Pages>
  constexpr sgl::error Menu<Pages...>::default_handle_input(Menu<Pages...>& menu,
                                                            Input           input) noexcept {
    sgl::error ret{sgl::error::invalid_page_index};
    menu.for_current_page([&ret, i = input](auto& page) { ret = page.handle_input(i); });
    return ret;
  }

  template <typename... Pages>
  template <size_t I, typename CharT>
  constexpr sgl::error
      Menu<Pages...>::set_active_page_impl(sgl::string_view<CharT> page_name) noexcept {
    if constexpr (I == sizeof...(Pages)) {
      return sgl::error::page_not_found;
    } else if constexpr (!sgl::is_convertible_v<typename decay_t<decltype(sgl::get<I>(pages_))>::StringView,
                                                sgl::string_view<CharT>>) {
      static_assert(sgl::is_convertible_v<typename decltype(sgl::get<I>(pages_))::StringView,
                                          sgl::string_view<CharT>>,
                    "page_name's type is not compatible with the item's string_view type");
    } else {
      if (page_name == sgl::string_view<CharT>(sgl::get<I>(pages_).name())) {
        sgl::error ec{sgl::error::page_not_found};
        for_current_page([&ec](auto& page) noexcept { ec = page.on_exit(); });
        if (ec != sgl::error::no_error)
          return ec;
        index_ = I;
        return sgl::get<I>(pages_).on_enter();
      } else {
        return set_active_page_impl<I + 1>(page_name);
      }
    }
  }

  template <typename... Pages>
  constexpr void Menu<Pages...>::set_start_edit_for_pages(sgl::Input start_edit) noexcept {
    sgl::for_each(pages_, [start_edit](auto& page) noexcept { page.set_start_edit(start_edit); });
  }

  template <typename... Pages>
  constexpr void Menu<Pages...>::set_stop_edit_for_pages(sgl::Input stop_edit) noexcept {
    sgl::for_each(pages_, [stop_edit](auto& page) noexcept { page.set_stop_edit(stop_edit); });
  }

  template <typename... Pages>
  template <size_t I, typename F>
  constexpr void Menu<Pages...>::for_current_page_impl(F&& f) noexcept(noexcept(f)) {
    if constexpr (I == sizeof...(Pages)) {
      return;
    } else {
      if (I == index_) {
        f(this->get_page<I>());
      } else {
        for_current_page_impl<I + 1>(forward<F>(f));
      }
    }
  }

  template <typename... Pages>
  template <size_t I, typename F>
  constexpr void Menu<Pages...>::for_current_page_impl(F&& f) const noexcept(noexcept(f)) {
    if constexpr (I == sizeof...(Pages)) {
      return;
    } else {
      if (I == index_) {
        f(this->get_page<I>());
      } else {
        for_current_page_impl<I + 1>(forward<F>(f));
      }
    }
  }

  template <size_t I, typename CharT, typename... Pages>
  constexpr auto& get(Menu<Pages...>& menu) noexcept {
    return menu.template get_page<I>();
  }

  template <size_t I, typename CharT, typename... Pages>
  constexpr const auto& get(const Menu<Pages...>& menu) noexcept {
    return menu.template get_page<I>();
  }

  template <typename F, typename CharT, typename... Pages>
  constexpr void for_each(Menu<Pages...>& menu, F&& f) noexcept(noexcept(f)) {
    menu.template for_each_page(forward<F>(f));
  }

  template <typename F, typename CharT, typename... Pages>
  constexpr void for_each(const Menu<Pages...>& menu, F&& f) noexcept(noexcept(f)) {
    menu.template for_each_page(forward<F>(f));
  }

  template <typename... Pages, sgl::enable_if_t<(sgl::is_page_v<Pages> && ...), bool> = true>
  constexpr Menu<decay_t<Pages>...> make_menu(Pages&&... pages) {
    return Menu<decay_t<Pages>...>(forward<Pages>(pages)...);
  }

  template <typename InputHandler,
            typename... Pages,
            sgl::enable_if_t<
                sgl::is_input_handler_for_v<InputHandler, Menu<decay_t<Pages>...>>> = true,
            sgl::enable_if_t<(sgl::is_page_v<Pages> && ...), bool> = true>
  constexpr Menu<decay_t<Pages>...> make_menu(InputHandler&& handler, Pages&&... pages) {
    return Menu<decay_t<Pages>...>(forward<InputHandler>(handler), forward<Pages>(pages)...);
  }
} // namespace sgl
#endif