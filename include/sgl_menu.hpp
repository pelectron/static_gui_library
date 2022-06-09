#ifndef SGL_MENU_HPP
#define SGL_MENU_HPP
#include "item_traits.hpp"
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_smallest_type.hpp"
#include "sgl_string_view.hpp"
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

    // constructors
    /**
     * @brief Construct a new Menu object
     *
     * @param menu_name
     * @param pages
     */
    Menu(StringView menu_name, Pages&&... pages)
        : pages_(forward<Pages>(pages)...), name_(menu_name) {
      pages_.for_each([this](auto& page) { page.set_menu(this); });
    }

    template <size_t N>
    Menu(const CharT (&name)[N], Pages&&... pages)
        : Menu(StringView(name), forward<Pages>(pages)...) {}

    Menu(const Menu& other)
        : pages_(other.pages_), input_handler_(other.input_handler_), name_(other.name_),
          index_(other.index_) {
      pages_.for_each([this](auto& page) { page.set_menu(this); });
    }

    Menu(Menu&& other)
        : pages_(move(other.pages_)), input_handler_(move(other.input_handler_)),
          name_(move(other.name_)), index_(move(other.index_)) {
      pages_.for_each([this](auto& page) { page.set_menu(this); });
    }

    /**
     * @brief process input
     * @param input user input
     * @return sgl::error
     */
    sgl::error handle_input(Input input) { return input_handler_(*this, input); }

    /**
     * @brief Set the active page by name
     * @param page_name name of the page
     * @return sgl::error
     */
    sgl::error set_active_page(StringView page_name) { return set_active_page_impl<0>(page_name); }

    /**
     * @brief Set the active page by index
     * @param page_index page index
     * @return sgl::error::invalid_page_index
     */
    sgl::error set_active_page(size_t page_index) {
      if (page_index < num_pages) {
        for_current_page([](auto& page) { page.on_exit(); });
        index_ = page_index;
        for_current_page([](auto& page) { page.on_enter(); });
        return sgl::error::no_error;
      } else
        return sgl::error::invalid_page_index;
    }

    /**
     * @brief get reference to page at index I
     * @tparam I page index
     */
    template <size_t I>
    page_at_t<I>& get_page() noexcept {
      return pages_.template get<I>();
    }

    /**
     * @brief get const reference to page at index I
     * @tparam I page index
     */
    template <size_t I>
    const page_at_t<I>& get_page() const noexcept {
      return pages_.template get<I>();
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
    item_at_t<PageIndex, ItemIndex>& get_item() noexcept {
      return pages_.template get<PageIndex>().template get_item<ItemIndex>();
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
    const item_at_t<PageIndex, ItemIndex>& get_item() const noexcept {
      return pages_.template get<PageIndex>().template get_item<ItemIndex>();
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
    void for_each_page(F&& f) noexcept(noexcept(f)) {
      pages_.for_each(forward<F>(f));
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
    void for_each_page(F&& f) const noexcept(noexcept(f)) {
      pages_.for_each(forward<F>(f));
    }

    /**
     * @brief apply f on the current page.
     * @tparam F functor type
     * @param f functor instance
     * @{
     */
    template <typename F>
    void for_current_page(F&& f) noexcept(noexcept(f)) {
      for_current_page_impl<0>(forward<F>(f));
    }

    template <typename F>
    void for_current_page(F&& f) const noexcept(noexcept(f)) {
      for_current_page_impl<0>(forward<F>(f));
    }
    /// @}

  private:
    static sgl::error default_handle_input(Menu<CharT, Pages...>& menu, Input input) noexcept {
      sgl::error ret;
      menu.for_current_page([&ret, i = input](auto& page) { ret = page.handle_input(i); });
      return ret;
    }

    template <size_t I>
    sgl::error set_active_page_impl(StringView page_name) noexcept {
      if constexpr (I == num_pages) {
        return sgl::error::page_not_found;
      } else {
        if (page_name == StringView(pages_.template get<I>().name())) {
          for_current_page([](auto& page) { page.on_exit(); });
          index_ = I;
          for_current_page([](auto& page) { page.on_enter(); });
          return error::no_error;
        } else {
          return set_active_page_impl<I + 1>(page_name);
        }
      }
    }

    void set_start_edit_for_pages(sgl::Input start_edit) {
      set_start_edit_for_pages(start_edit, sgl::index_sequence_for<Pages...>{});
    }

    template <size_t... I>
    void set_start_edit_for_pages(sgl::Input start_edit, sgl::index_seq_t<I...>) {
      (get_page<I>().set_start_edit(start_edit), ...);
    }

    void set_stop_edit_for_pages(sgl::Input stop_edit) {
      set_stop_edit_for_pages(stop_edit, sgl::index_sequence_for<Pages...>{});
    }

    template <size_t... I>
    void set_stop_edit_for_pages(sgl::Input stop_edit, sgl::index_seq_t<I...>) {
      (get_page<I>().set_stop_edit(stop_edit), ...);
    }

    template <size_t I, typename F>
    void for_current_page_impl(F&& f) noexcept(noexcept(f)) {
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
    void for_current_page_impl(F&& f) const noexcept(noexcept(f)) {
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

} // namespace sgl
#endif