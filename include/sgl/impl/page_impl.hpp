//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_PAGE_IMPL_HPP
#define SGL_IMPL_PAGE_IMPL_HPP
#include "sgl/page.hpp"

namespace sgl {

  template <typename NameList, typename ItemList>
  template <typename... Names, typename... Items>
  constexpr Page<NameList, ItemList>::Page(const sgl::NamedValue<Names, Items>&... items) noexcept(
      std::is_nothrow_copy_constructible_v<ItemTuple>)
      : items_(std::move(items)...) {}

  template <typename NameList, typename ItemList>
  template <typename... Names, typename... Items>
  constexpr Page<NameList, ItemList>::Page(sgl::NamedValue<Names, Items>&&... items) noexcept(
      std::is_nothrow_move_constructible_v<ItemTuple>)
      : items_(std::move(items)...) {}

  template <typename NameList, typename ItemList>
  constexpr size_t Page<NameList, ItemList>::size() const noexcept {
    return sgl::list_size_v<ItemList>;
  }

  template <typename NameList, typename ItemList>
  constexpr size_t Page<NameList, ItemList>::current_item_index() const noexcept {
    return index_;
  }

  template <typename NameList, typename ItemList>
  constexpr Page<NameList, ItemList>&
      Page<NameList, ItemList>::set_current_item(size_t i) noexcept {
    index_ = i % sgl::list_size_v<ItemList>;
    return *this;
  }

  template <typename NameList, typename ItemList>
  template <char... Cs>
  constexpr Page<NameList, ItemList>&
      Page<NameList, ItemList>::set_current_item(sgl::Name<Cs...> name) noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, NameList>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, NameList>,
                    "No Item with such a name exists in this page");
    } else {
      index_ = sgl::index_of_v<sgl::Name<Cs...>, NameList>;
      static_cast<void>(name);
    }
    return *this;
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::error Page<NameList, ItemList>::handle_input(sgl::input i) noexcept {
    return input_handler_(*this, i);
  }

  template <typename NameList, typename ItemList>
  constexpr bool Page<NameList, ItemList>::is_in_edit_mode() const noexcept {
    return elem_in_edit_;
  }

  template <typename NameList, typename ItemList>
  constexpr void Page<NameList, ItemList>::set_edit_mode() noexcept {
    elem_in_edit_ = true;
  }

  template <typename NameList, typename ItemList>
  constexpr void Page<NameList, ItemList>::set_navigation_mode() noexcept {
    elem_in_edit_ = false;
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::input Page<NameList, ItemList>::get_start_edit() const noexcept {
    return start_edit_;
  }

  template <typename NameList, typename ItemList>
  constexpr Page<NameList, ItemList>&
      Page<NameList, ItemList>::set_start_edit(sgl::input start_edit) noexcept {
    start_edit_ = start_edit;
    return *this;
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::input Page<NameList, ItemList>::get_stop_edit() const noexcept {
    return stop_edit_;
  }

  template <typename NameList, typename ItemList>
  constexpr Page<NameList, ItemList>&
      Page<NameList, ItemList>::set_stop_edit(sgl::input stop_edit) noexcept {
    stop_edit_ = stop_edit;
    return *this;
  }

  template <typename NameList, typename ItemList>
  template <typename Menu>
  constexpr void Page<NameList, ItemList>::set_menu(Menu* menu) noexcept {
    sgl::for_each(items_, [menu](auto& item) noexcept { item.set_menu(menu); });
  }

  template <typename NameList, typename ItemList>
  template <typename F>
  constexpr void Page<NameList, ItemList>::for_each_item(F&& f) {
    sgl::for_each(items_, std::forward<F>(f));
  }

  template <typename NameList, typename ItemList>
  template <typename F>
  constexpr void Page<NameList, ItemList>::for_each_item(F&& f) const {
    sgl::for_each(items_, std::forward<F>(f));
  }

  template <typename NameList, typename ItemList>
  template <typename F>
  constexpr void Page<NameList, ItemList>::for_each_item_with_name(F&& f) {
    sgl::for_each_with_name(items_, std::forward<F>(f));
  }

  template <typename NameList, typename ItemList>
  template <typename F>
  constexpr void Page<NameList, ItemList>::for_each_item_with_name(F&& f) const {
    sgl::for_each_with_name(items_, std::forward<F>(f));
  }

  template <typename NameList, typename ItemList>
  template <typename F>
  constexpr decltype(auto) Page<NameList, ItemList>::for_current_item(F&& f) {
    return for_current_item_impl<0>(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList>
  template <typename F>
  constexpr decltype(auto) Page<NameList, ItemList>::for_current_item(F&& f) const {
    return for_current_item_impl<0>(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList>
  template <char... Cs>
  constexpr auto& Page<NameList, ItemList>::operator[](sgl::Name<Cs...> name) noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, NameList>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, NameList>,
                    "No item with this name found in this page");
    } else {
      return items_[name];
    }
  }

  template <typename NameList, typename ItemList>
  template <char... Cs>
  constexpr const auto& Page<NameList, ItemList>::operator[](sgl::Name<Cs...> name) const noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, NameList>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, NameList>,
                    "No item with this name found in this page");
    } else {
      return items_[name];
    }
  }

  template <typename NameList, typename ItemList>
  template <typename Action>
  constexpr Page<NameList, ItemList>&
      Page<NameList, ItemList>::set_on_enter(Action&& action) noexcept {
    on_enter_.bind(std::forward<Action>(action));
    return *this;
  }

  template <typename NameList, typename ItemList>
  template <typename Action>
  constexpr Page<NameList, ItemList>&
      Page<NameList, ItemList>::set_on_exit(Action&& action) noexcept {
    on_exit_.bind(std::forward<Action>(action));
    return *this;
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::error Page<NameList, ItemList>::on_enter() noexcept {
    return on_enter_(*this);
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::error Page<NameList, ItemList>::on_exit() noexcept {
    return on_exit_(*this);
  }

  template <typename NameList, typename ItemList>
  constexpr void Page<NameList, ItemList>::tick() noexcept {
    sgl::for_each(items_, [](auto& item) { item.tick(); });
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::string_view<char> Page<NameList, ItemList>::item_name(size_t i) const noexcept {
    return item_name_impl<0>(i);
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::string_view<typename Page<NameList, ItemList>::char_type>
      Page<NameList, ItemList>::item_text(size_t i) const noexcept {
    return this->item_text_impl(i);
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::error
      Page<NameList, ItemList>::default_handle_input(Page<NameList, ItemList>& page,
                                                     sgl::input                i) noexcept {
    if ((i == page.get_start_edit()) and not page.is_in_edit_mode()) {
      page.set_edit_mode();
    } else if ((i == page.get_stop_edit()) and page.is_in_edit_mode()) {
      page.set_navigation_mode();
    }
    if (page.is_in_edit_mode()) {
      // edit mode-> delegate input to current item
      sgl::error ec{sgl::error::no_error};
      for_current(page, [&ec, &i](auto& item) noexcept -> void { ec = item.handle_input(i); });
      switch (ec) {
        case sgl::error::edit_finished:
          page.set_navigation_mode();
          return error::no_error;
        case sgl::error::no_error:
          return ec;
        default:
          page.set_navigation_mode();
          return ec;
      }
    } else {
      // page navigation mode -> move up/down items_ with keypad
      if (!is_keyboard_input(i)) {
        switch (i) {
          case sgl::input::down:
            [[fallthrough]];
          case sgl::input::right:
            page.set_current_item(page.current_item_index() + 1);
            break;
          case sgl::input::up:
            [[fallthrough]];
          case sgl::input::left:
            page.set_current_item(page.current_item_index() == 0 ? page.size() - 1
                                                                 : page.current_item_index() - 1);
            break;
          default:
            break;
        }
      }
    }
    return sgl::error::no_error;
  }

  template <typename NameList, typename ItemList>
  template <size_t I, typename F>
  constexpr decltype(auto) Page<NameList, ItemList>::for_current_item_impl(F&& f) {
    if constexpr (I == (list_size_v<ItemList> - 1)) {
      return std::forward<F>(f)(sgl::get<I>(items_));
    } else {
      if (index_ == I) {
        return std::forward<F>(f)(sgl::get<I>(items_));
      }
      return for_current_item_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename NameList, typename ItemList>
  template <size_t I, typename F>
  constexpr decltype(auto) Page<NameList, ItemList>::for_current_item_impl(F&& f) const {
    if constexpr (I == (list_size_v<ItemList> - 1)) {
      return std::forward<F>(f)(sgl::get<I>(items_));
    } else {
      if (index_ == I) {
        return std::forward<F>(f)(sgl::get<I>(items_));
      }
      return for_current_item_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename NameList, typename ItemList>
  constexpr sgl::error
      Page<NameList, ItemList>::default_page_action(Page<NameList, ItemList>& page) noexcept {
    static_cast<void>(page);
    return sgl::error::no_error;
  }

  template <typename NameList, typename ItemList>
  template <size_t I>
  constexpr sgl::string_view<char>
      Page<NameList, ItemList>::item_name_impl(size_t i) const noexcept {
    if constexpr (I == list_size_v<ItemList>) {
      return {};
    } else {
      if (I == i) {
        return sgl::type_at_t<I, NameList>{}.to_view();
      }
      return this->item_name_impl<I + 1>(i);
    }
  }

  template <typename NameList, typename ItemList>
  template <size_t I>
  constexpr sgl::string_view<typename Page<NameList, ItemList>::char_type>
      Page<NameList, ItemList>::item_text_impl(size_t i) const noexcept {
    if constexpr (I == list_size_v<ItemList>) {
      return {};
    } else {
      if (I == i) {
        return sgl::get<I>(items_).text();
      }
      return item_text_impl<I + 1>(i);
    }
  }

  template <typename NameList, typename ItemList, typename F>
  constexpr void for_each(Page<NameList, ItemList>& page, F&& f) {
    return page.for_each_item(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  constexpr void for_each(const Page<NameList, ItemList>& page, F&& f) {
    return page.for_each_item(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  constexpr void for_each_with_name(Page<NameList, ItemList>& page, F&& f) {
    page.for_each_item_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  constexpr void for_each_with_name(const Page<NameList, ItemList>& page, F&& f) {
    page.for_each_item_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  constexpr decltype(auto) for_current(Page<NameList, ItemList>& page, F&& f) {
    return page.for_current_item(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  constexpr decltype(auto) for_current(const Page<NameList, ItemList>& page, F&& f) {
    return page.for_current_item(std::forward<F>(f));
  }

} // namespace sgl
#endif /* SGL_IMPL_PAGE_IMPL_HPP */
