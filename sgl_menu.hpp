#ifndef SGL_MENU_HPP
#define SGL_MENU_HPP
#include "sgl_fwd.hpp"
#include "sgl_page.hpp"

namespace sgl {
  /**
   * @brief This class encapsulates a menu. It's comprised of several pages.
   * Only one page is active at any time. Input handling can be customized by
   * providing a input handler. The input handler must have a calling signature
   * of 'error(Menu_t<LineWidth, CharT, Pages...>&, Input)'. By default, the
   * inputs are delegated to the current page.
   *
   * @tparam LineWidth line width of the menu
   * @tparam CharT character type of the menu
   * @tparam Pages page types of the menu
   */
  template <size_t LineWidth, typename CharT, typename... Pages>
  class Menu_t {
  public:
    using char_type = CharT;
    static constexpr size_t num_pages = sizeof...(Pages);
    using ItemBase = sgl::Item_t<LineWidth, CharT>;
    using InputHandler_t =
        Callable<error(Menu_t<LineWidth, CharT, Pages...>&, Input)>;
    template <typename T>
    using input_handler_check = std::enable_if_t<
        std::is_invocable_r_v<error,
                              T,
                              Menu_t<LineWidth, CharT, Pages...>&,
                              Input>>;

    /// returns type at index I of Pages...
    template <size_t I>
    using page_at_t = type_at_t<I, type_list<Pages...>>;
    /// get item index I from page with index P
    template <size_t P, size_t I>
    using item_at_t = typename page_at_t<P>::template item_at_t<I>;

    using StringView = typename sgl::string_view<CharT>;

    // constructors
    /**
     * @brief Construct a new Menu_t object
     *
     * @param menu_name
     * @param pages
     */
    Menu_t(StringView menu_name, Pages&&... pages)
        : pages_(Pages(this, std::forward<Pages>(pages))...), name_(menu_name) {
    }

    Menu_t(const Menu_t& other)
        : pages_(other.pages_), input_handler_(other.input_handler_),
          name_(other.name_), index_(other.index_) {
      pages_.for_each([this](auto& page) {
        page.for_each_item([this](auto& item) { item.set_menu(this); });
      });
    }

    Menu_t(Menu_t&& other)
        : pages_(std::move(other.pages_)),
          input_handler_(std::move(other.input_handler_)),
          name_(std::move(other.name_)), index_(other.index_) {
      pages_.for_each([this](auto& page) {
        page.for_each_item([this](auto& item) { item.set_menu(this); });
      });
    }

    /**
     * @brief
     *
     * @param input
     * @return error
     */
    sgl::error handle_input(Input input) {
      return input_handler_(*this, input);
    }

    /**
     * @brief Set the active page by name
     *
     * @param page_name name of the page
     * @return sgl::error
     */
    sgl::error set_active_page(StringView page_name) {
      return set_active_page_impl<0>(page_name);
    }

    /**
     * @brief Set the active page by index
     *
     * @param page_index
     * @return sgl::error
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
     * @brief get current item
     * @return ItemBase&
     */
    ItemBase& current_item() noexcept {
      ItemBase* ret;
      for_current_page([&ret](auto& page) { ret = &page.current_item(); });
      return *ret;
    }

    /**
     * @brief get current item
     *
     * @return const ItemBase&
     */
    const ItemBase& current_item() const noexcept {
      const ItemBase* ret{nullptr};
      for_current_page(
          [&ret](const auto& page) { ret = &page.current_item(); });
      return *ret;
    }

    /**
     * @brief get menu_name of the current page
     * @return StringView
     */
    StringView current_page_name() const noexcept {
      StringView ret;
      for_current_page([&ret](auto& page) { ret = page.get_name(); });
      return ret;
    }

    /**
     * @brief Get a page by index
     * @tparam I index
     * @return reference to page at index I
     */
    template <size_t I>
    page_at_t<I>& get_page() noexcept {
      return pages_.template get<I>();
    }

    /**
     * @brief Get the page object
     * @tparam I
     * @return const reference to page at index I
     */
    template <size_t I>
    const page_at_t<I>& get_page() const noexcept {
      return pages_.template get<I>();
    }

    /**
     * @brief get the index of the currently active page
     */
    constexpr size_t index() const noexcept { return index_; }

    /**
     * @brief get number of pages
     *
     * @return constexpr size_t
     */
    constexpr size_t size() const noexcept {
      return number_of_pages_v<Menu_t<LineWidth, CharT, Pages...>>;
    }

    /**
     * @brief get reference to item at ItemIndex from page at PageIndex
     *
     * @tparam PageIndex page index in menu
     * @tparam ItemIndex item index in page
     * @return item_at_t<PageIndex, ItemIndex>&
     */
    template <size_t PageIndex, size_t ItemIndex>
    item_at_t<PageIndex, ItemIndex>& get_item() noexcept {
      return pages_.template get<PageIndex>().template get_item<ItemIndex>();
    }

    /**
     * @brief get reference to item at ItemIndex from page at PageIndex
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
     *
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F>
    void for_each_page(F&& f) noexcept(noexcept(f)) {
      pages_.template for_each(std::forward<F>(f));
    }

    /**
     * @brief apply f on each page in menu.
     *
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F>
    void for_each_page(F&& f) const noexcept(noexcept(f)) {
      pages_.template for_each(std::forward<F>(f));
    }

    template <typename F>
    void for_current_page(F&& f) {
      for_current_page_impl<0>(std::forward<F>(f));
    }
    template <typename F>
    void for_current_page(F&& f) const {
      for_current_page_impl<0>(std::forward<F>(f));
    }
    friend Menu_t<LineWidth, CharT, Pages...>
        make_menu<LineWidth, CharT, Pages...>(StringView menu_name,
                                              sgl::Input start_edit,
                                              sgl::Input stop_edit,
                                              Pages&&... pages);

  private:
    static error default_handle_input(Menu_t<LineWidth, CharT, Pages...>& menu,
                                      Input input) noexcept {
      sgl::error ret;
      menu.for_current_page(
          [&ret, i = input](auto& page) { ret = page.handle_input(i); });
      return ret;
    }

    template <size_t I>
    sgl::error set_active_page_impl(StringView page_name) noexcept {
      if constexpr (I == num_pages) {
        return sgl::error::page_not_found;
      } else {
        if (page_name == StringView(pages_.template get<I>().get_name())) {
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
    void set_start_edit_for_pages(sgl::Input start_edit,
                                  sgl::index_seq_t<I...>) {
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
    void for_current_page_impl(F&& f) {
      if constexpr (I == num_pages) {
        return;
      } else {
        if (I == index_) {
          f(this->get_page<I>());
        } else {
          for_current_page_impl<I + 1>(std::forward<F>(f));
        }
      }
    }
    template <size_t I, typename F>
    void for_current_page_impl(F&& f) const {
      if constexpr (I == num_pages) {
        return;
      } else {
        if (I == index_) {
          f(this->get_page<I>());
        } else {
          for_current_page_impl<I + 1>(std::forward<F>(f));
        }
      }
    }

    tuple<Pages...> pages_;
    InputHandler_t  input_handler_{&default_handle_input};
    StringView      name_;
    size_t          index_{0};
  };

  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> menu_name,
                                               Pages&&... pages) {
    return Menu_t<LineWidth, CharT, Pages...>(menu_name,
                                              std::forward<Pages>(pages)...);
  }

  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> menu_name,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               Pages&&... pages) {
    auto m = Menu_t<LineWidth, CharT, Pages...>(menu_name,
                                                std::forward<Pages>(pages)...);
    m.set_start_edit_for_pages(start_edit);
    m.set_stop_edit_for_pages(stop_edit);
    return m;
  }
} // namespace sgl
#endif