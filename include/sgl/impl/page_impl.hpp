//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_PAGE_IMPL_HPP
#define SGL_IMPL_PAGE_IMPL_HPP
#include "sgl/page.hpp"
namespace sgl {

  template <typename... Names, typename... Items>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::Page(
      const sgl::NamedValue<Names, Items>&... items) noexcept(nothrow_copy_constructible)
      : items_(std::move(items)...) {}

  template <typename... Names, typename... Items>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::Page(
      sgl::NamedValue<Names, Items>&&... items) noexcept(nothrow_move_constructible)
      : items_(std::move(items)...) {}

  template <typename... Names, typename... Items>
  constexpr size_t Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::size() const noexcept {
    return sizeof...(Items);
  }

  template <typename... Names, typename... Items>
  constexpr size_t Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::current_item_index()
      const noexcept {
    return index_;
  }

  template <typename... Names, typename... Items>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>&
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_current_item(
          size_t i) noexcept {
    index_ = i % sizeof...(Items);
    return *this;
  }

  template <typename... Names, typename... Items>
  template <typename Name>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>&
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_current_item(
          Name name) noexcept {
    if constexpr (!sgl::contains_v<Name, name_list>) {
      static_assert(sgl::contains_v<Name, name_list>,
                    "No Item with such a name exists in this page");
    } else {
      index_ = sgl::index_of_v<Name, name_list>;
      static_cast<void>(name);
    }
    return *this;
  }

  template <typename... Names, typename... Items>
  constexpr sgl::error Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::handle_input(
      sgl::Input i) noexcept {
    return input_handler_(*this, i);
  }

  template <typename... Names, typename... Items>
  constexpr bool
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::is_in_edit_mode() const noexcept {
    return elem_in_edit_;
  }

  template <typename... Names, typename... Items>
  constexpr void
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_edit_mode() noexcept {
    elem_in_edit_ = true;
  }

  template <typename... Names, typename... Items>
  constexpr void
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_navigation_mode() noexcept {
    elem_in_edit_ = false;
  }

  template <typename... Names, typename... Items>
  constexpr sgl::Input
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::get_start_edit() const noexcept {
    return start_edit_;
  }

  template <typename... Names, typename... Items>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>&
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_start_edit(
          sgl::Input start_edit) noexcept {
    start_edit_ = start_edit;
    return *this;
  }

  template <typename... Names, typename... Items>
  constexpr sgl::Input
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::get_stop_edit() const noexcept {
    return stop_edit_;
  }

  template <typename... Names, typename... Items>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>&
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_stop_edit(
          sgl::Input stop_edit) noexcept {
    stop_edit_ = stop_edit;
    return *this;
  }

  template <typename... Names, typename... Items>
  template <typename Menu>
  constexpr void
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_menu(Menu* menu) noexcept {
    sgl::for_each(items_, [menu](auto& item) noexcept { item.set_menu(menu); });
  }

  template <typename... Names, typename... Items>
  template <typename F>
  constexpr void Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_each_item(
      F&& f) noexcept(nothrow_applicable<F>) {
    sgl::for_each(items_, std::forward<F>(f));
  }

  template <typename... Names, typename... Items>
  template <typename F>
  constexpr void
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_each_item(F&& f) const
      noexcept(const_nothrow_applicable<F>) {
    sgl::for_each(items_, std::forward<F>(f));
  }

  template <typename... Names, typename... Items>
  template <typename F>
  constexpr void Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_each_item_with_name(
      F&& f) noexcept(nothrow_applicable_with_name<F>) {
    sgl::for_each_with_name(items_, std::forward<F>(f));
  }

  template <typename... Names, typename... Items>
  template <typename F>
  constexpr void
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_each_item_with_name(F&& f) const
      noexcept(const_nothrow_applicable_with_name<F>) {
    sgl::for_each_with_name(items_, std::forward<F>(f));
  }

  template <typename... Names, typename... Items>
  template <typename F>
  constexpr decltype(auto)
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_current_item(F&& f) noexcept(
          nothrow_applicable<F>) {
    return for_current_item_impl<0>(std::forward<F>(f));
  }

  template <typename... Names, typename... Items>
  template <typename F>
  constexpr decltype(auto)
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_current_item(F&& f) const
      noexcept(const_nothrow_applicable<F>) {
    return for_current_item_impl<0>(std::forward<F>(f));
  }

  template <typename... Names, typename... Items>
  template <typename Name>
  auto& Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::operator[](Name name) noexcept {
    if constexpr (!sgl::contains_v<Name, name_list>) {
      static_assert(sgl::contains_v<Name, name_list>, "No item with this name found in this page");
    } else {
      return items_[name];
    }
  }

  template <typename... Names, typename... Items>
  template <typename Name>
  const auto& Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::operator[](
      Name name) const noexcept {
    if constexpr (!sgl::contains_v<Name, name_list>) {
      static_assert(sgl::contains_v<Name, name_list>, "No item with this name found in this page");
    } else {
      return items_[name];
    }
  }

  template <typename... Names, typename... Items>
  template <typename Action>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>&
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_on_enter(
          Action&& action) noexcept {
    on_enter_ = std::forward<Action>(action);
    return *this;
  }

  template <typename... Names, typename... Items>
  template <typename Action>
  constexpr Page<sgl::type_list<Names...>, sgl::type_list<Items...>>&
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::set_on_exit(
          Action&& action) noexcept {
    on_exit_ = std::forward<Action>(action);
    return *this;
  }

  template <typename... Names, typename... Items>
  constexpr sgl::error
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::on_enter() noexcept {
    return on_enter_(*this);
  }

  template <typename... Names, typename... Items>
  constexpr sgl::error
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::on_exit() noexcept {
    return on_exit_(*this);
  }

  template <typename... Names, typename... Items>
  constexpr void Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::tick() noexcept {
    sgl::for_each(items_, [](auto& item) { item.tick(); });
  }

  template <typename... Names, typename... Items>
  constexpr sgl::error
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::default_handle_input(
          Page<sgl::type_list<Names...>, sgl::type_list<Items...>>& page,
          sgl::Input                                                i) noexcept {
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
          case sgl::Input::down:
            [[fallthrough]];
          case sgl::Input::right:
            page.set_current_item(page.current_item_index() + 1);
            break;
          case sgl::Input::up:
            [[fallthrough]];
          case sgl::Input::left:
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

  template <typename... Names, typename... Items>
  template <size_t I, typename F>
  constexpr decltype(auto)
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_current_item_impl(
          F&& f) noexcept(nothrow_applicable<F>) {
    if constexpr (I == (sizeof...(Items) - 1)) {
      return std::forward<F>(f)(sgl::get<I>(items_));
    } else {
      if (index_ == I) {
        return std::forward<F>(f)(sgl::get<I>(items_));
      }
      return for_current_item_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename... Names, typename... Items>
  template <size_t I, typename F>
  constexpr decltype(auto)
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::for_current_item_impl(F&& f) const
      noexcept(const_nothrow_applicable<F>) {
    if constexpr (I == (sizeof...(Items) - 1)) {
      return std::forward<F>(f)(sgl::get<I>(items_));
    } else {
      if (index_ == I) {
        return std::forward<F>(f)(sgl::get<I>(items_));
      }
      return for_current_item_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename... Names, typename... Items>
  constexpr sgl::error
      Page<sgl::type_list<Names...>, sgl::type_list<Items...>>::default_page_action(
          Page<sgl::type_list<Names...>, sgl::type_list<Items...>>& page) noexcept {
    static_cast<void>(page);
    return sgl::error::no_error;
  }

  template <typename NameList, typename ItemList, typename F>
  void for_each(Page<NameList, ItemList>& page,
                F&& f) noexcept(noexcept(page.template for_each_item(std::forward<F>(f)))) {
    return page.template for_each_item(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  void for_each(const Page<NameList, ItemList>& page,
                F&& f) noexcept(noexcept(page.template for_each_item(std::forward<F>(f)))) {
    return page.template for_each_item(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  void for_each_with_name(Page<NameList, ItemList>& page, F&& f) noexcept(
      noexcept(page.template for_each_item_with_name(std::forward<F>(f)))) {
    page.template for_each_item_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  void for_each_with_name(const Page<NameList, ItemList>& page, F&& f) noexcept(
      noexcept(page.template for_each_item_with_name(std::forward<F>(f)))) {
    page.template for_each_item_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  decltype(auto) for_current(Page<NameList, ItemList>& page, F&& f) noexcept(
      noexcept(std::declval<Page<NameList, ItemList>>().for_current_item(std::forward<F>(f)))) {
    return page.for_current_item(std::forward<F>(f));
  }

  template <typename NameList, typename ItemList, typename F>
  decltype(auto) for_current(const Page<NameList, ItemList>& page, F&& f) noexcept(noexcept(
      std::declval<const Page<NameList, ItemList>>().for_current_item(std::forward<F>(f)))) {
    return page.for_current_item(std::forward<F>(f));
  }

} // namespace sgl
#endif /* SGL_IMPL_PAGE_IMPL_HPP */
