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

  template <typename CharT, typename... Pages>
  class Menu {
  public:
    static_assert((sgl::is_page_v<Pages> && ...), "");
    using char_type = CharT;
    static constexpr size_t num_pages = sizeof...(Pages);
    using InputHandler_t = Callable<error(Menu<CharT, Pages...>&, Input)>;

    /// returns type at index I of Pages...
    template <size_t I>
    using page_at_t = type_at_t<I, type_list<Pages...>>;
    /// get item index I from page with index P
    template <size_t P, size_t I>
    using item_at_t = typename page_at_t<P>::template item_at_t<I>;

    using StringView = sgl::string_view<CharT>;
    template <typename... P>
    static constexpr bool
        nothrow_constructible_v = (sgl::is_nothrow_constructible_v<sgl::decay_t<P>, P> && ...);
    // constructors
    /**
     * @brief Construct a new Menu object
     *
     * @param menu_name
     * @param pages
     */
    constexpr Menu(StringView menu_name,
                   Pages&&... pages) noexcept(nothrow_constructible_v<Pages...>)
        : pages_(forward<Pages>(pages)...), name_(menu_name) {
      sgl::for_each(pages_, [this](auto& page) { page.set_menu(this); });
    }

    template <size_t N>
    constexpr Menu(const CharT (&name)[N],
                   Pages&&... pages) noexcept(nothrow_constructible_v<Pages...>)
        : Menu(StringView(name), forward<Pages>(pages)...) {}

    constexpr Menu(const Menu& other) noexcept(nothrow_constructible_v<Pages...>)
        : pages_(other.pages_), input_handler_(other.input_handler_), name_(other.name_),
          index_(other.index_) {
      sgl::for_each(pages_, [this](auto& page) { page.set_menu(this); });
    }

    constexpr Menu(Menu&& other) noexcept(nothrow_constructible_v<Pages...>)
        : pages_(move(other.pages_)), input_handler_(move(other.input_handler_)),
          name_(move(other.name_)), index_(move(other.index_)) {
      sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
    }

    /**
     * @brief process input
     * @param input user input
     * @return sgl::error
     */
    constexpr sgl::error handle_input(Input input) noexcept { return input_handler_(*this, input); }

    /**
     * @brief Set the active page by name
     * @param page_name name of the page
     * @return sgl::error
     */
    constexpr sgl::error set_active_page(StringView page_name) noexcept {
      return set_active_page_impl<0>(page_name);
    }

    /**
     * @brief Set the active page by index
     * @param page_index page index
     * @return sgl::error::invalid_page_index
     */
    constexpr sgl::error set_active_page(size_t page_index) noexcept {
      if (page_index < num_pages) {
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

    /**
     * @brief get reference to page at index I
     * @tparam I page index
     */
    template <size_t I>
    constexpr page_at_t<I>& get_page() noexcept {
      return sgl::get<I>(pages_);
    }

    /**
     * @brief get const reference to page at index I
     * @tparam I page index
     */
    template <size_t I>
    constexpr const page_at_t<I>& get_page() const noexcept {
      return sgl::get<I>(pages_);
    }

    /// get the index of the currently active page
    constexpr size_t index() const noexcept { return index_; }

    /// get number of pages stored in the menu
    constexpr size_t size() const noexcept { return sizeof...(Pages); }

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
    constexpr item_at_t<PageIndex, ItemIndex>& get_item() noexcept {
      return sgl::get<ItemIndex>(sgl::get<PageIndex>(pages_));
    }

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
    constexpr const item_at_t<PageIndex, ItemIndex>& get_item() const noexcept {
      return sgl::get<ItemIndex>(sgl::get<PageIndex>(pages_));
    }

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
    constexpr void for_each_page(F&& f) noexcept(noexcept(f)) {
      sgl::for_each(pages_, forward<F>(f));
    }

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
    constexpr void for_each_page(F&& f) const noexcept(noexcept(f)) {
      sgl::for_each(pages_, forward<F>(f));
    }

    /**
     * @brief apply f on the current page.
     * @tparam F functor type
     * @param f functor instance
     * @{
     */
    template <typename F>
    constexpr void for_current_page(F&& f) noexcept(noexcept(f)) {
      for_current_page_impl<0>(forward<F>(f));
    }

    template <typename F>
    constexpr void for_current_page(F&& f) const noexcept(noexcept(f)) {
      for_current_page_impl<0>(forward<F>(f));
    }
    /// @}

  private:
    constexpr static sgl::error default_handle_input(Menu<CharT, Pages...>& menu,
                                                     Input                  input) noexcept {
      sgl::error ret{sgl::error::invalid_page_index};
      menu.for_current_page([&ret, i = input](auto& page) { ret = page.handle_input(i); });
      return ret;
    }

    template <size_t I>
    constexpr sgl::error set_active_page_impl(StringView page_name) noexcept {
      if constexpr (I == num_pages) {
        return sgl::error::page_not_found;
      } else {
        if (page_name == StringView(pages_.template get<I>().name())) {
          for_current_page([](auto& page) noexcept { page.on_exit(); });
          index_ = I;
          for_current_page([](auto& page) noexcept { page.on_enter(); });
          return error::no_error;
        } else {
          return set_active_page_impl<I + 1>(page_name);
        }
      }
    }

    constexpr void set_start_edit_for_pages(sgl::Input start_edit) noexcept {
      sgl::for_each(pages_, [start_edit](auto& page) noexcept { page.set_start_edit(start_edit); });
    }

    constexpr void set_stop_edit_for_pages(sgl::Input stop_edit) noexcept {
      sgl::for_each(pages_, [stop_edit](auto& page) noexcept { page.set_stop_edit(stop_edit); });
    }

    template <size_t I, typename F>
    constexpr void for_current_page_impl(F&& f) noexcept(noexcept(f)) {
      if constexpr (I == num_pages) {
        return;
      } else {
        if (I == index_) {
          f(this->get_page<I>());
        } else {
          for_current_page_impl<I + 1>(forward<F>(f));
        }
      }
    }

    template <size_t I, typename F>
    constexpr void for_current_page_impl(F&& f) const noexcept(noexcept(f)) {
      if constexpr (I == num_pages) {
        return;
      } else {
        if (I == index_) {
          f(this->get_page<I>());
        } else {
          for_current_page_impl<I + 1>(forward<F>(f));
        }
      }
    }

    sgl::tuple<Pages...>                   pages_;
    InputHandler_t                         input_handler_{&default_handle_input};
    StringView                             name_;
    sgl::smallest_type_t<sizeof...(Pages)> index_{0};
  };

  template <size_t I, typename CharT, typename... Pages>
  constexpr auto& get(Menu<CharT, Pages...>& menu) noexcept {
    return menu.template get_page<I>();
  }

  template <size_t I, typename CharT, typename... Pages>
  constexpr const auto& get(const Menu<CharT, Pages...>& menu) noexcept {
    return menu.template get_page<I>();
  }

  template <typename F, typename CharT, typename... Pages>
  constexpr void for_each(Menu<CharT, Pages...>& menu, F&& f) noexcept(noexcept(f)) {
    menu.template for_each_page(std::forward<F>(f));
  }

  template <typename F, typename CharT, typename... Pages>
  constexpr void for_each(const Menu<CharT, Pages...>& menu, F&& f) noexcept(noexcept(f)) {
    menu.template for_each_page(std::forward<F>(f));
  }
} // namespace sgl
#endif