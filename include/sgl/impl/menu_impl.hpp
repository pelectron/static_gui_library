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

  template <typename NameList, typename PageList>
  constexpr Menu<NameList, PageList>::Menu(const Menu& other) noexcept(
      std::is_nothrow_copy_constructible_v<PageTuple>)
      : pages_(other.pages_), input_handler_(other.input_handler_), index_(other.index_) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename NameList, typename PageList>
  constexpr Menu<NameList, PageList>::Menu(Menu&& other) noexcept(
      std::is_nothrow_move_constructible_v<PageTuple>)
      : pages_(std::move(other.pages_)), input_handler_(std::move(other.input_handler_)),
        index_(std::move(other.index_)) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename NameList, typename PageList>
  template <typename... Names, typename... Pages>
  constexpr Menu<NameList, PageList>::Menu(const sgl::NamedValue<Names, Pages>&... pages) noexcept(
      std::is_nothrow_copy_constructible_v<PageTuple>)
      : pages_(pages...) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename NameList, typename PageList>
  template <typename... Names, typename... Pages>
  constexpr Menu<NameList, PageList>::Menu(sgl::NamedValue<Names, Pages>&&... pages) noexcept(
      std::is_nothrow_move_constructible_v<PageTuple>)
      : pages_(std::move(pages)...) {
    for_each(pages_, [this](auto& page) { page.set_menu(this); });
  }

  template <typename NameList, typename PageList>
  constexpr sgl::error Menu<NameList, PageList>::handle_input(sgl::Input i) noexcept {
    return for_current_page(
        [i](auto& page) noexcept -> sgl::error { return page.handle_input(i); });
  }

  template <typename NameList, typename PageList>
  constexpr void Menu<NameList, PageList>::tick() noexcept {
    for_each(pages_, [](auto& page) { page.tick(); });
  }

  template <typename NameList, typename PageList>
  constexpr size_t Menu<NameList, PageList>::current_page_index() const noexcept {
    return index_;
  }

  template <typename NameList, typename PageList>
  constexpr size_t Menu<NameList, PageList>::size() const noexcept {
    return sgl::list_size_v<PageList>;
  }

  template <typename NameList, typename PageList>
  constexpr sgl::error Menu<NameList, PageList>::set_current_page(size_t page_index) noexcept {
    auto ec = for_current_page([](auto& page) { return page.on_exit(); });
    if (ec != sgl::error::no_error) {
      return ec;
    }
    index_ = page_index % sgl::list_size_v<PageList>;
    return for_current_page([](auto& page) { return page.on_enter(); });
  }

  template <typename NameList, typename PageList>
  template <char... Cs>
  constexpr sgl::error Menu<NameList, PageList>::set_current_page(sgl::Name<Cs...> name) noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, NameList>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, NameList>,
                    "No page with such a name found in this menu");
    } else {
      static_cast<void>(name);
      return set_current_page(sgl::index_of_v<sgl::Name<Cs...>, NameList>);
    }
  }

  template <typename NameList, typename PageList>
  template <char... Cs>
  constexpr auto& Menu<NameList, PageList>::operator[](sgl::Name<Cs...> name) noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, NameList>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, NameList>,
                    "No page with such a name exists in this menu");
    } else {
      return pages_[name];
    }
  }

  template <typename NameList, typename PageList>
  template <char... Cs>
  constexpr const auto& Menu<NameList, PageList>::operator[](sgl::Name<Cs...> name) const noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, NameList>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, NameList>,
                    "No page with such a name exists in this menu");
    } else {
      return pages_[name];
    }
  }

  template <typename NameList, typename PageList>
  template <size_t I>
  constexpr auto& Menu<NameList, PageList>::get_page() noexcept {
    if constexpr (I >= sgl::list_size_v<PageList>) {
      static_assert(I < sgl::list_size_v<PageList>, "index out of range");
    } else {
      return sgl::get<I>(pages_);
    }
  }

  template <typename NameList, typename PageList>
  template <size_t I>
  constexpr const auto& Menu<NameList, PageList>::get_page() const noexcept {
    if constexpr (I >= sgl::list_size_v<PageList>) {
      static_assert(I < sgl::list_size_v<PageList>, "index out of range");
    } else {
      return sgl::get<I>(pages_);
    }
  }

  template <typename NameList, typename PageList>
  constexpr sgl::string_view<char> Menu<NameList, PageList>::page_name() const noexcept {
    return page_name_impl<0>();
  }

  template <typename NameList, typename PageList>
  constexpr sgl::string_view<char> Menu<NameList, PageList>::item_name(size_t i) const noexcept {
    return this->item_name_impl(i);
  }

  template <typename NameList, typename PageList>
  constexpr sgl::string_view<typename Menu<NameList, PageList>::char_type>
      Menu<NameList, PageList>::item_text(size_t i) const noexcept {
    return this->item_text_impl(i);
  }

  template <typename NameList, typename PageList>
  template <typename F>
  constexpr void Menu<NameList, PageList>::for_each_page(F&& f) {
    sgl::for_each(pages_, std::forward<F>(f));
  }

  template <typename NameList, typename PageList>
  template <typename F>
  constexpr void Menu<NameList, PageList>::for_each_page(F&& f) const {
    sgl::for_each(pages_, std::forward<F>(f));
  }

  template <typename NameList, typename PageList>
  template <typename F>
  constexpr decltype(auto) Menu<NameList, PageList>::for_current_page(F&& f) {
    return for_current_page_impl<0>(std::forward<F>(f));
  }

  template <typename NameList, typename PageList>
  template <typename F>
  constexpr decltype(auto) Menu<NameList, PageList>::for_current_page(F&& f) const {
    return for_current_page_impl<0>(std::forward<F>(f));
  }

  template <typename NameList, typename PageList>
  constexpr sgl::error Menu<NameList, PageList>::default_handle_input(Menu& menu,
                                                                      Input input) noexcept {
    return menu.for_current_page([input](auto& page) { return page.handle_input(input); });
  }

  template <typename NameList, typename PageList>
  template <size_t I, typename F>
  constexpr decltype(auto) Menu<NameList, PageList>::for_current_page_impl(F&& f) {
    if constexpr (I == (sgl::list_size_v<PageList> - 1)) {
      return std::forward<F>(f)(sgl::get<I>(pages_));
    } else {
      if (I == index_) {
        return std::forward<F>(f)(sgl::get<I>(pages_));
      }
      return for_current_page_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename NameList, typename PageList>
  template <size_t I, typename F>
  constexpr decltype(auto) Menu<NameList, PageList>::for_current_page_impl(F&& f) const {
    if constexpr (I == (sgl::list_size_v<PageList> - 1)) {
      return std::forward<F>(f)(sgl::get<I>(pages_));
    } else {
      if (I == index_) {
        return std::forward<F>(f)(sgl::get<I>(pages_));
      }
      return for_current_page_impl<I + 1>(std::forward<F>(f));
    }
  }

  template <typename NameList, typename PageList>
  template <size_t I>
  constexpr sgl::string_view<char> Menu<NameList, PageList>::page_name_impl() const noexcept {
    if constexpr (I == sgl::list_size_v<PageList>)
      return {};
    else {
      if (index_ == I)
        return sgl::type_at_t<I, NameList>{}.to_view();
      return this->page_name_impl<I + 1>();
    }
  }

  template <typename NameList, typename PageList>
  [[nodiscard]] constexpr sgl::string_view<char>
      Menu<NameList, PageList>::item_name_impl(size_t i) const noexcept {
    return this->for_current_page(
        [i](const auto& page) -> sgl::string_view<char> { return page.item_name(i); });
  }

  template <typename NameList, typename PageList>
  constexpr sgl::string_view<typename Menu<NameList, PageList>::char_type>
      Menu<NameList, PageList>::item_text_impl(size_t i) const noexcept {
    return this->for_current_page([i](const auto& page) { return page.item_text(i); });
  }

  template <typename NameList, typename PageList, typename F>
  void for_each(Menu<NameList, PageList>& menu, F&& f){
    menu.for_each_page(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  void for_each(const Menu<NameList, PageList>& menu, F&& f) {
    menu.for_each_page(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(Menu<NameList, PageList>& menu, F&& f) {
    menu.for_each_page_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  void for_each_with_name(const Menu<NameList, PageList>& menu, F&& f) {
    menu.for_each_page_with_name(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_current(Menu<NameList, PageList>& menu, F&& f) {
    return menu.for_current_page(std::forward<F>(f));
  }

  template <typename NameList, typename PageList, typename F>
  decltype(auto) for_current(const Menu<NameList, PageList>& menu, F&& f) {
    return menu.for_current_page(std::forward<F>(f));
  }
} // namespace sgl
#endif /* SGL_IMPL_MENU_IMPL_HPP */
