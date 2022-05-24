#ifndef SGL_MENU_HPP
#define SGL_MENU_HPP
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
    template <size_t I>
    using type_at_index = type_at_t<I, type_list<Pages...>>;

    // constructors
    /**
     * @brief Construct a new Menu_t object
     *
     * @param name
     * @param pages
     */
    Menu_t(string_view<CharT> name, Pages&&... pages)
        : name_(name), pages_(std::forward<Pages>(pages)...) {
      set_menu_impl<0>();
    }

    Menu_t(const Menu_t& other)
        : name_(other.name_), pages_(other.pages_), index_(other.index_),
          input_handler_(other.input_handler_) {
      set_menu_impl<0>();
    }
    Menu_t(Menu_t&& other)
        : name_(std::move(other.name_)), pages_(std::move(other.pages_)),
          index_(other.index_),
          input_handler_(std::move(other.input_handler_)) {
      set_menu_impl<0>();
    }

    /**
     * @brief
     *
     * @param input
     * @return error
     */
    error handle_input(Input input) { return input_handler_(*this, input); }
    /**
     * @brief Set the active page object
     *
     * @param name
     * @return error
     */
    error set_active_page(string_view<CharT> name) {
      return set_active_page_impl<0>(name);
    }
    /**
     * @brief
     *
     * @return ItemBase&
     */
    ItemBase& current_item() noexcept { return current_item_impl<0>(); }
    string_view<CharT> current_page_name() noexcept {
      return current_page_name_impl<0>();
    }
    /**
     * @brief Get the page object
     *
     * @tparam I
     * @return type_at_index<I>&
     */
    template <size_t I>
    type_at_index<I>& get_page() noexcept {
      return pages_.template get<I>();
    }
    /**
     * @brief Get the page object
     *
     * @tparam I
     * @return const type_at_index<I>&
     */
    template <size_t I>
    const type_at_index<I>& get_page() const noexcept {
      return pages_.template get<I>();
    }
    /**
     * @brief index of the currently active page
     */
    constexpr size_t index() const noexcept { return index_; }

  private:
    static error default_handle_input(Menu_t<LineWidth, CharT, Pages...>& menu,
                                      Input input) noexcept {
      return menu.default_handle_input_impl<0>(input);
    }

    template <size_t I>
    error default_handle_input_impl(Input input) noexcept {
      if constexpr (I == (sizeof...(Pages) - 1)) {
          return get_page<sizeof...(Pages) - 1>().handle_input(
              input);          
      } else {
        if (index_ == I)
          return get_page<I>().handle_input(input);
        else
          return default_handle_input_impl<I + 1>(input);
      }
    }

    template <size_t I>
    error set_active_page_impl(string_view<CharT> name) noexcept {
      if (name ==
          string_view<CharT>(pages_.template get<I>().get_name())) {
        index_ = I;
        return error::no_error;
      } else {
        return set_active_page_impl<I + 1>(name);
      }
    }

    template <>
    error set_active_page_impl<sizeof...(Pages) - 1>(
        string_view<CharT> name) noexcept {
      if (name == string_view<CharT>(
                      pages_.template get<sizeof...(Pages) - 1>().get_name())) {
        index_ = sizeof...(Pages) - 1;
        return error::no_error;
      } else {
        return error::page_not_found;
      }
    }

    template <size_t I>
    string_view<CharT> current_page_name_impl() noexcept {
      if (index_ == I)
        return pages_.template get<I>().get_name();
      return current_page_name_impl<I + 1>();
    }
    template <>
    string_view<CharT>
        current_page_name_impl<sizeof...(Pages) - 1>() noexcept {
      return pages_.template get<sizeof...(Pages) - 1>().get_name();
    }
    template <size_t I>
    void set_menu_impl() noexcept {
      pages_.template get<I>()
          .template set_menu<Menu_t<LineWidth, CharT, Pages...>>(this);
      set_menu_impl<I + 1>();
    }
    template <>
    void set_menu_impl<sizeof...(Pages) - 1>() noexcept {
      pages_.template get<sizeof...(Pages) - 1>()
          .template set_menu<Menu_t<LineWidth, CharT, Pages...>>(this);
    }

    template <size_t I>
    ItemBase& current_item_impl() noexcept {
      if (index_ == I)
        return pages_.template get<I>().current_item();
      return current_item_impl<I + 1>();
    }
    template <>
    ItemBase& current_item_impl<sizeof...(Pages) - 1>() noexcept {
      return pages_.template get<sizeof...(Pages) - 1>().current_item();
    }
    string_view<CharT> name_;
    tuple<Pages...>          pages_;
    size_t                   index_{0};
    InputHandler_t           input_handler_{&default_handle_input};
  };

  template <size_t LineWidth, typename CharT, typename... Pages>
  Menu_t<LineWidth, CharT, Pages...> make_menu(string_view<CharT> name,
                                               Pages&&... pages) {
    return Menu_t<LineWidth, CharT, Pages...>(name,
                                              std::forward<Pages>(pages)...);
  }
} // namespace sgl
#endif