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

    using string_view_t = typename sgl::string_view<CharT>;

    // constructors
    /**
     * @brief Construct a new Menu_t object
     *
     * @param menu_name
     * @param pages
     */
    Menu_t(string_view_t menu_name, Pages&&... pages)
        : pages_(std::forward<Pages>(pages)...), name_(menu_name) {
      set_menu_impl(sgl::index_sequence_for<Pages...>{});
    }

    Menu_t(const Menu_t& other)
        : pages_(other.pages_), input_handler_(other.input_handler_),
          name_(other.name_), index_(other.index_) {
      set_menu_impl(sgl::index_sequence_for<Pages...>{});
    }

    Menu_t(Menu_t&& other)
        : pages_(std::move(other.pages_)),
          input_handler_(std::move(other.input_handler_)),
          name_(std::move(other.name_)), index_(other.index_) {
      set_menu_impl(sgl::index_sequence_for<Pages...>{});
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
     * @brief Set the active page object
     *
     * @param page_name
     * @return error
     */
    sgl::error set_active_page(string_view_t page_name) {
      return set_active_page_impl<0>(page_name);
    }

    sgl::error set_active_page(size_t page_index) {
      if (page_index < num_pages) {
        index_ = page_index;
        return sgl::error::no_error;
      } else
        return sgl::error::invalid_page_index;
    }

    /**
     * @brief get current item
     * @return ItemBase&
     */
    ItemBase& current_item() noexcept { return current_item_impl<0>(); }

    const ItemBase& current_item() const noexcept {
      return const_current_item_impl<0>();
    }

    /**
     * @brief get menu_name of the current page
     * @return string_view_t
     */
    string_view_t current_page_name() const noexcept {
      return current_page_name_impl<0>();
    }

    /**
     * @brief Get a page by index
     *
     * @tparam I index
     * @return page_at_t<I>&
     */
    template <size_t                                           I,
              typename = std::enable_if_t<I<sizeof...(Pages)>> page_at_t<I>&
                  get_page() noexcept {
      if constexpr (I < sizeof...(Pages))
        return pages_.template get<I>();
      else
        static_assert(I < sizeof...(Pages), "index out of range");
    }

    /**
     * @brief Get the page object
     *
     * @tparam I
     * @return const page_at_t<I>&
     */
    template <size_t I,
              typename = std::enable_if_t<I<sizeof...(Pages)>> const
                  page_at_t<I>& get_page() const noexcept {
      if constexpr (I < sizeof...(Pages))
        return pages_.template get<I>();
      else
        static_assert(I < sizeof...(Pages), "index out of range");
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
    item_at_t<PageIndex, ItemIndex>& get_item() {
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
    const item_at_t<PageIndex, ItemIndex>& get_item() const{
      return pages_.template get<PageIndex>().template get_item<ItemIndex>();
    }

    friend Menu_t<LineWidth, CharT, Pages...>
        make_menu<LineWidth, CharT, Pages...>(string_view_t menu_name,
                                              sgl::Input    start_edit,
                                              sgl::Input    stop_edit,
                                              Pages&&... pages);


  private:
    static error default_handle_input(Menu_t<LineWidth, CharT, Pages...>& menu,
                                      Input input) noexcept {
      return default_handle_input_impl<0>(menu, input);
    }

    template <size_t I>
    static sgl::error
        default_handle_input_impl(Menu_t<LineWidth, CharT, Pages...>& menu,
                                  Input input) noexcept {
      if constexpr (I == sizeof...(Pages)) {
        // if this is reached, index_ has an invalid value.
        return sgl::error::invalid_page_index;
      } else {
        if (menu.index() == I)
          return menu.get_page<I>().handle_input(input);
        else
          return default_handle_input_impl<I + 1>(menu, input);
      }
    }

    template <size_t I>
    sgl::error set_active_page_impl(string_view_t page_name) noexcept {
      if constexpr (I == num_pages) {
        return sgl::error::page_not_found;
      } else {
        if (page_name == string_view_t(pages_.template get<I>().get_name())) {
          index_ = I;
          return error::no_error;
        } else {
          return set_active_page_impl<I + 1>(page_name);
        }
      }
    }

    template <size_t I>
    string_view_t current_page_name_impl() const noexcept {
      if constexpr (I == (num_pages - 1)) {
        return pages_.template get<I>().get_name();
      } else {
        if (I == index_)
          return pages_.template get<I>().get_name();
        else
          return current_page_name_impl<I + 1>();
      }
    }

    template <size_t... I>
    void set_menu_impl(sgl::index_seq_t<I...>) {
      // static overload of set_menu called!
      (pages_.template get<I>().set_menu(this), ...);
    }

    template <size_t I>
    ItemBase& current_item_impl() noexcept {
      if constexpr (I == (num_pages - 1))
        return get_page<I>().current_item();
      else {
        if (I == index_)
          return get_page<I>().current_item();
        else
          return current_item_impl<I + 1>();
      }
    }

    template <size_t I>
    const ItemBase& const_current_item_impl() const noexcept {
      if constexpr (I == (num_pages - 1))
        return get_page<I>().current_item();
      else {
        if (I == index_)
          return get_page<I>().current_item();
        else
          return const_current_item_impl<I + 1>();
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

    tuple<Pages...> pages_;
    InputHandler_t  input_handler_{&default_handle_input};
    string_view_t   name_;
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