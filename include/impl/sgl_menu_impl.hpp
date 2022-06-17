#ifndef SGL_MENU_IMPL_HPP
#define SGL_MENU_IMPL_HPP
#include "../sgl_menu.hpp"
namespace sgl {
  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(Pages&&... pages) noexcept(
      nothrow_move_constructible)
      : pages_(std::move(pages)...) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(const Pages&... pages) noexcept(
      nothrow_copy_constructible)
      : pages_(pages...) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  // template <typename... Pages>
  // template <typename InputHandler, sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>>>
  // constexpr Menu<Pages...>::Menu(InputHandler&& handler, Pages&&... pages) noexcept(
  //     nothrow_move_constructible)
  //     : Menu<Pages...>(std::move(pages)...), input_handler_(std::forward<InputHandler>(handler)) {}

  // template <typename... Pages>
  // template <typename InputHandler, sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>>>
  // constexpr Menu<Pages...>::Menu(InputHandler&& handler, const Pages&... pages) noexcept(
  //     nothrow_move_constructible)
  //     : Menu<Pages...>(pages...), input_handler_(std::forward<InputHandler>(handler)) {}

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(const Menu& other) noexcept(
      nothrow_copy_constructible)
      : pages_(other.pages_), input_handler_(other.input_handler_), index_(other.index_) {
    sgl::for_each(pages_, [this](auto& page) noexcept { page.set_menu(this); });
  }

  template <typename... Pages>
  constexpr Menu<Pages...>::Menu(Menu&& other) noexcept(nothrow_move_constructible)
      : pages_(std::move(other.pages_)), input_handler_(std::move(other.input_handler_)),
        index_(std::move(other.index_)) {
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
    sgl::for_each(pages_, std::forward<F>(f));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_each_page(F&& f) const noexcept(noexcept(f)) {
    sgl::for_each(pages_, std::forward<F>(f));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_current_page(F&& f) noexcept(noexcept(f)) {
    for_current_page_impl<0>(std::forward<F>(f));
  }

  template <typename... Pages>
  template <typename F>
  constexpr void Menu<Pages...>::for_current_page(F&& f) const noexcept(noexcept(f)) {
    for_current_page_impl<0>(std::forward<F>(f));
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
    } else if constexpr (!std::is_convertible_v<
                             typename std::decay_t<decltype(sgl::get<I>(pages_))>::StringView,
                             sgl::string_view<CharT>>) {
      static_assert(std::is_convertible_v<typename decltype(sgl::get<I>(pages_))::StringView,
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
        for_current_page_impl<I + 1>(std::forward<F>(f));
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
        for_current_page_impl<I + 1>(std::forward<F>(f));
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
    menu.template for_each_page(std::forward<F>(f));
  }

  template <typename F, typename CharT, typename... Pages>
  constexpr void for_each(const Menu<Pages...>& menu, F&& f) noexcept(noexcept(f)) {
    menu.template for_each_page(std::forward<F>(f));
  }

  template <typename... Pages, std::enable_if_t<(sgl::is_page_v<Pages> && ...), bool> = true>
  constexpr Menu<std::decay_t<Pages>...> make_menu(Pages&&... pages) {
    return Menu<std::decay_t<Pages>...>(std::forward<Pages>(pages)...);
  }

  template <
      typename InputHandler,
      typename... Pages,
      std::enable_if_t<sgl::is_input_handler_for_v<InputHandler, Menu<std::decay_t<Pages>...>>> = true,
      std::enable_if_t<(sgl::is_page_v<Pages> && ...), bool> = true>
  constexpr Menu<std::decay_t<Pages>...> make_menu(InputHandler&& handler, Pages&&... pages) {
    return Menu<std::decay_t<Pages>...>(std::forward<InputHandler>(handler), std::forward<Pages>(pages)...);
  }
} // namespace sgl
#endif