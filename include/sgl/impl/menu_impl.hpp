//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_MENU_IMPL_HPP
#define SGL_IMPL_MENU_IMPL_HPP
#include "sgl/menu.hpp"
#include "sgl/page.hpp"

namespace sgl {

  template <typename... Names, typename... Pages>
  constexpr Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::Menu(
      const Menu& other) noexcept(nothrow_copy_constructible)
      : pages_(other.pages_), input_handler_(other.input_handler_), index_(other.index_) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename... Names, typename... Pages>
  constexpr Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::Menu(Menu&& other) noexcept(
      nothrow_move_constructible)
      : pages_(std::move(other.pages_)), input_handler_(std::move(other.input_handler_)),
        index_(std::move(other.index_)) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename... Names, typename... Pages>
  constexpr Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::Menu(
      const sgl::NamedValue<Names, Pages>&... pages) noexcept(nothrow_copy_constructible)
      : pages_(pages...) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename... Names, typename... Pages>
  constexpr Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::Menu(
      sgl::NamedValue<Names, Pages>&&... pages) noexcept(nothrow_move_constructible)
      : pages_(std::move(pages)...) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename... Names, typename... Pages>
  sgl::error Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::handle_input(
      sgl::Input i) noexcept {
    return for_current_page([i](auto& page) { return page.handle_input(i); });
  }

  template <typename... Names, typename... Pages>
  constexpr void Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::tick() noexcept {
    for_each(pages_, [](auto& page) { page.tick(); });
  }

  template <typename... Names, typename... Pages>
  constexpr size_t Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::current_page_index()
      const noexcept {
    return index_;
  }

  template <typename... Names, typename... Pages>
  constexpr size_t Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::size() const noexcept {
    return sizeof...(Pages);
  }

  template <typename... Names, typename... Pages>
  constexpr sgl::error Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::set_current_page(
      size_t page_index) noexcept {
    auto ec = for_current_page([](auto& page) { return page.on_exit(); });
    if (ec != sgl::error::no_error) {
      return ec;
    }
    index_ = page_index % sizeof...(Pages);
    return for_current_page([](auto& page) { return page.on_enter(); });
  }

  template <typename... Names, typename... Pages>
  template <typename Name>
  constexpr sgl::error Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::set_current_page(
      Name name) noexcept {
    if constexpr (!sgl::contains_v<Name, name_list>) {
      static_assert(sgl::contains_v<Name, name_list>,
                    "No page with such a name found in this menu");
    } else {
      static_cast<void>(name);
      return set_current_page(sgl::index_of_v<Name, name_list>);
    }
  }

  template <typename... Names, typename... Pages>
  template <typename Name>
  auto& Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::operator[](Name name) noexcept {
    if constexpr (!sgl::contains_v<Name, name_list>) {
      static_assert(sgl::contains_v<Name, name_list>,
                    "No page with such a name found in this menu");
    } else {
      return pages_[name];
    }
  }

  template <typename... Names, typename... Pages>
  template <typename Name>
  const auto& Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::operator[](
      Name name) const noexcept {
    if constexpr (!sgl::contains_v<Name, name_list>) {
      static_assert(sgl::contains_v<Name, name_list>,
                    "No page with such a name found in this menu");
    } else {
      return pages_[name];
    }
  }

  template <typename... Names, typename... Pages>
  template <size_t I>
  constexpr auto& Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::get_page() noexcept {
    if constexpr (I >= sizeof...(Pages)) {
      static_assert(I < sizeof...(Pages), "index out of range");
    } else {
      return sgl::get<I>(pages_);
    }
  }

  template <typename... Names, typename... Pages>
  template <size_t I>
  constexpr const auto&
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::get_page() const noexcept {
    if constexpr (I >= sizeof...(Pages)) {
      static_assert(I < sizeof...(Pages), "index out of range");
    } else {
      return sgl::get<I>(pages_);
    }
  }

  template <typename... Names, typename... Pages>
  template <typename F>
  constexpr void Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::for_each_page(
      F&& f) noexcept(nothrow_applicable<F>) {
    sgl::for_each(pages_, std::forward<F>(f));
  }

  template <typename... Names, typename... Pages>
  template <typename F>
  constexpr void
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::for_each_page(F&& f) const
      noexcept(const_nothrow_applicable<F>) {
    sgl::for_each(pages_, std::forward<F>(f));
  }

  template <typename... Names, typename... Pages>
  template <typename F>
  constexpr decltype(auto)
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::for_current_page(F&& f) noexcept(
          nothrow_applicable<F>) {
    return for_current_page_impl<0>(std::forward<F>(f));
  }

  template <typename... Names, typename... Pages>
  template <typename F>
  constexpr decltype(auto)
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::for_current_page(F&& f) const
      noexcept(const_nothrow_applicable<F>) {
    return for_current_page_impl<0>(std::forward<F>(f));
  }

  template <typename... Names, typename... Pages>
  constexpr sgl::error
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::default_handle_input(
          Menu& menu,
          Input input) noexcept {
    return menu.for_current_page([input](auto& page) { return page.handle_input(input); });
  }

  template <typename... Names, typename... Pages>
  template <size_t I, typename F>
  constexpr decltype(auto)
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::for_current_page_impl(
          F&& f) noexcept(nothrow_applicable<F>) {
    if constexpr (I == (sizeof...(Pages) - 1)) {
      return std::forward<F>(f)(sgl::get<I>(pages_));
    } else {
      if (I == index_) {
        return std::forward<F>(f)(sgl::get<I>(pages_));
      }
      return for_current_page_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename... Names, typename... Pages>
  template <size_t I, typename F>
  constexpr decltype(auto)
      Menu<sgl::type_list<Names...>, sgl::type_list<Pages...>>::for_current_page_impl(F&& f) const
      noexcept(const_nothrow_applicable<F>) {
    if constexpr (I == (sizeof...(Pages) - 1)) {
      return std::forward<F>(f)(sgl::get<I>(pages_));
    } else {
      if (I == index_) {
        return std::forward<F>(f)(sgl::get<I>(pages_));
      }
      return for_current_page_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename NameList, typename PageList, typename F>
  void for_each(Menu<NameList, PageList>& menu, F&& f) noexcept(
      noexcept(std::declval<Menu<NameList, PageList>>().for_each_page(std::declval<F>()))) {
    menu.for_each_page(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  void for_each(const Menu<NameList, PageList>& menu, F&& f) noexcept(
      noexcept(std::declval<Menu<NameList, PageList>>().for_each_page(std::declval<F>()))) {
    menu.for_each_page(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(Menu<NameList, PageList>& menu, F&& f) noexcept(noexcept(
      std::declval<Menu<NameList, PageList>>().for_each_page_with_name(std::declval<F>()))) {
    menu.for_each_page_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(const Menu<NameList, PageList>& menu, F&& f) noexcept(noexcept(
      std::declval<const Menu<NameList, PageList>>().for_each_page_with_name(std::declval<F>()))) {
    menu.for_each_page_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_current(Menu<NameList, PageList>& menu, F&& f) noexcept(
      noexcept(std::declval<Menu<NameList, PageList>>().for_current_page(std::declval<F>()))) {
    return menu.for_current_page(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_current(const Menu<NameList, PageList>& menu, F&& f) noexcept(noexcept(
      std::declval<const Menu<NameList, PageList>>().for_current_page(std::declval<F>()))) {
    return menu.for_current_page(std::forward<F>(f));
  }
} // namespace sgl
#endif /* SGL_IMPL_MENU_IMPL_HPP */
