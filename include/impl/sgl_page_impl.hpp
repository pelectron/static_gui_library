#ifndef SGL_PAGE_IMPL_HPP
#define SGL_PAGE_IMPL_HPP
#include "../sgl_page.hpp"
namespace sgl {
  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(const Page& other) noexcept(nothrow_copy_constructible)
      : items_(other.items_), input_handler_(other.input_handler_), on_enter_(other.on_enter_),
        on_exit_(other.on_exit_), name_(other.name_), title_(other.title_),
        start_edit_(other.start_edit_), stop_edit_(other.stop_edit_),
        elem_in_edit_(other.elem_in_edit_), index_(other.index_) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(Page&& other) noexcept(nothrow_move_constructible)
      : items_(std::move(other.items_)), input_handler_(std::move(other.input_handler_)),
        on_enter_(std::move(other.on_enter_)), on_exit_(std::move(other.on_exit_)),
        name_(std::move(other.name_)), title_(std::move(other.title_)),
        start_edit_(other.start_edit_), stop_edit_(other.stop_edit_),
        elem_in_edit_(other.elem_in_edit_), index_(other.index_) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(StringView name,
                                        StringView title,
                                        Items&&... items) noexcept(nothrow_move_constructible)
      : items_(std::move(items)...), name_(name), title_(title) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(StringView name,
                                        StringView title,
                                        const Items&... items) noexcept(nothrow_copy_constructible)
      : items_(items...), name_(name), title_(title) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(sgl::string_view<CharT> name,
                                        sgl::string_view<CharT> title,
                                        sgl::Input              start_edit,
                                        sgl::Input              stop_edit,
                                        Items&&... items) noexcept(nothrow_move_constructible)
      : Page<CharT, Items...>(name,
                              title,
                              start_edit,
                              stop_edit,
                              0,
                              &default_page_action,
                              &default_page_action,
                              &default_handle_input,
                              std::move(items)...) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(sgl::string_view<CharT> name,
                                        sgl::string_view<CharT> title,
                                        sgl::Input              start_edit,
                                        sgl::Input              stop_edit,
                                        const Items&... items) noexcept(nothrow_copy_constructible)
      : Page<CharT, Items...>(name,
                              title,
                              start_edit,
                              stop_edit,
                              0,
                              &default_page_action,
                              &default_page_action,
                              &default_handle_input,
                              items...) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(sgl::string_view<CharT> name,
                                        sgl::string_view<CharT> title,
                                        sgl::Input              start_edit,
                                        sgl::Input              stop_edit,
                                        size_t                  start_index,
                                        Items&&... items) noexcept(nothrow_move_constructible)
      : Page<CharT, Items...>(name,
                              title,
                              start_edit,
                              stop_edit,
                              start_index,
                              &default_page_action,
                              &default_page_action,
                              &default_handle_input,
                              std::move(items)...) {}

  template <typename CharT, typename... Items>
  constexpr Page<CharT, Items...>::Page(sgl::string_view<CharT> name,
                                        sgl::string_view<CharT> title,
                                        sgl::Input              start_edit,
                                        sgl::Input              stop_edit,
                                        size_t                  start_index,
                                        const Items&... items) noexcept(nothrow_copy_constructible)
      : Page<CharT, Items...>(name,
                              title,
                              start_edit,
                              stop_edit,
                              start_index,
                              &default_page_action,
                              &default_page_action,
                              &default_handle_input,
                              items...) {}

  template <typename CharT, typename... Items>
  constexpr typename Page<CharT, Items...>::StringView
      Page<CharT, Items...>::name() const noexcept {
    return name_;
  }

  template <typename CharT, typename... Items>
  constexpr typename Page<CharT, Items...>::StringView
      Page<CharT, Items...>::title() const noexcept {
    return title_;
  }

  template <typename CharT, typename... Items>
  constexpr size_t Page<CharT, Items...>::size() const noexcept {
    return sizeof...(Items);
  }

  template <typename CharT, typename... Items>
  constexpr size_t Page<CharT, Items...>::index() const noexcept {
    return index_;
  }

  template <typename CharT, typename... Items>
  constexpr sgl::error Page<CharT, Items...>::handle_input(sgl::Input i) noexcept {
    return input_handler_(*this, i);
  }

  template <typename CharT, typename... Items>
  template <size_t I>
  constexpr typename Page<CharT, Items...>::template item_at_t<I>&
      Page<CharT, Items...>::get_item() noexcept {
    return sgl::get<I>(items_);
  }

  template <typename CharT, typename... Items>
  template <size_t I>
  constexpr const typename Page<CharT, Items...>::template item_at_t<I>&
      Page<CharT, Items...>::get_item() const noexcept {
    return sgl::get<I>(items_);
  }

  template <typename CharT, typename... Items>
  constexpr void Page<CharT, Items...>::set_item_cursor(size_t i) noexcept {
    index_ = i % sizeof...(Items);
  }

  template <typename CharT, typename... Items>
  constexpr bool Page<CharT, Items...>::is_in_edit_mode() const noexcept {
    return elem_in_edit_;
  }

  template <typename CharT, typename... Items>
  constexpr void Page<CharT, Items...>::set_edit_mode() noexcept {
    elem_in_edit_ = true;
  }

  template <typename CharT, typename... Items>
  constexpr void Page<CharT, Items...>::set_navigation_mode() noexcept {
    elem_in_edit_ = false;
  }

  template <typename CharT, typename... Items>
  constexpr sgl::Input Page<CharT, Items...>::get_start_edit() const noexcept {
    return start_edit_;
  }

  template <typename CharT, typename... Items>
  constexpr void Page<CharT, Items...>::set_start_edit(sgl::Input start_edit) noexcept {
    start_edit_ = start_edit;
  }

  template <typename CharT, typename... Items>
  constexpr sgl::Input Page<CharT, Items...>::get_stop_edit() const noexcept {
    return stop_edit_;
  }

  template <typename CharT, typename... Items>
  constexpr void Page<CharT, Items...>::set_stop_edit(sgl::Input stop_edit) noexcept {
    stop_edit_ = stop_edit;
  }

  template <typename CharT, typename... Items>
  template <typename Menu>
  constexpr void Page<CharT, Items...>::set_menu(Menu* menu) noexcept {
    sgl::for_each(items_, [menu](auto& item) noexcept { item.set_menu(menu); });
  }

  template <typename CharT, typename... Items>
  template <typename F>
  constexpr void Page<CharT, Items...>::for_each_item(F&& f) noexcept(noexcept(f)) {
    sgl::for_each(items_, std::forward<F>(f));
  }

  template <typename CharT, typename... Items>
  template <typename F>
  constexpr void Page<CharT, Items...>::for_each_item(F&& f) const noexcept(noexcept(f)) {
    sgl::for_each(items_, std::forward<F>(f));
  }

  template <typename CharT, typename... Items>
  template <typename F>
  constexpr void Page<CharT, Items...>::for_current_item(F&& f) noexcept(noexcept(f)) {
    for_current_item_impl<0>(std::forward<F>(f));
  }

  template <typename CharT, typename... Items>
  template <typename F>
  constexpr void Page<CharT, Items...>::for_current_item(F&& f) const noexcept(noexcept(f)) {
    for_current_item_impl<0>(std::forward<F>(f));
  }

  template <typename CharT, typename... Items>
  constexpr sgl::error Page<CharT, Items...>::on_enter() noexcept {
    return on_enter_(*this);
  }

  template <typename CharT, typename... Items>
  constexpr sgl::error Page<CharT, Items...>::on_exit() noexcept {
    return on_exit_(*this);
  }

  template <typename CharT, typename... Items>
  constexpr sgl::error Page<CharT, Items...>::default_handle_input(Page<CharT, Items...>& page,
                                                                   sgl::Input i) noexcept {
    if ((i == page.get_start_edit() and not page.is_in_edit_mode())) {
      page.set_edit_mode();
    } else if ((i == page.get_stop_edit()) and page.is_in_edit_mode()) {
      page.set_navigation_mode();
    }
    if (page.is_in_edit_mode()) {
      // edit mode-> delegate input to current item
      sgl::error ec{sgl::error::no_error};
      page.for_current_item([&ec, &i](auto& item) noexcept { ec = item.handle_input(i); });
      switch (ec) {
        case error::edit_finished:
          page.set_navigation_mode();
          return error::no_error;
        default:
          return ec;
      }
    } else {
      // page navigation mode -> move up/down items_ with keypad
      if (!is_keyboard_input(i)) {
        switch (i) {
          case sgl::Input::down:
            [[fallthrough]];
          case sgl::Input::right:
            page.set_item_cursor(page.index() + 1);
            break;
          case sgl::Input::up:
            [[fallthrough]];
          case sgl::Input::left:
            page.set_item_cursor(page.index() == 0 ? page.size() - 1 : page.index() - 1);
            break;
          default:
            break;
        }
      }
    }
    return sgl::error::no_error;
  }

  template <typename CharT, typename... Items>
  template <size_t I, typename F>
  constexpr void Page<CharT, Items...>::for_current_item_impl(F&& f) noexcept(noexcept(f)) {
    if constexpr (I == (sizeof...(Items) - 1)) {
      f(this->get_item<I>());
    } else {
      if (index_ == I) {
        f(this->get_item<I>());
      } else {
        this->for_current_item_impl<I + 1>(std::forward<F>(f));
      }
    }
  }

  template <typename CharT, typename... Items>
  template <size_t I, typename F>
  constexpr void Page<CharT, Items...>::for_current_item_impl(F&& f) const noexcept(noexcept(f)) {
    if constexpr (I == (sizeof...(Items) - 1)) {
      f(this->get_item<I>());
    } else {
      if (index_ == I) {
        f(this->get_item<I>());
      } else {
        this->for_current_item_impl<I + 1>(std::forward<F>(f));
      }
    }
  }

  template <typename CharT, typename... Items>
  constexpr sgl::error Page<CharT, Items...>::default_page_action(Page<CharT, Items...>&) noexcept {
    return sgl::error::no_error;
  }
} // namespace sgl
#endif