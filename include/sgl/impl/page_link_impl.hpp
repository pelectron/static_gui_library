//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_PAGE_LINK_IMPL_HPP
#define SGL_IMPL_PAGE_LINK_IMPL_HPP
#include "sgl/page_link.hpp"
#include "sgl/type_list.hpp"

namespace sgl {

  template <typename Name, size_t TextSize, typename CharT>
  constexpr PageLink<Name, TextSize, CharT>::PageLink(Name page_to_link) noexcept
      : Base(sgl::string_view<char>(Name{})) {
    static_cast<void>(page_to_link);
  }

  template <typename Name, size_t TextSize, typename CharT>
  constexpr PageLink<Name, TextSize, CharT>::PageLink(Name page_to_link, StringView text) noexcept
      : Base(text) {
    static_cast<void>(page_to_link);
  }

  template <typename Name, size_t TextSize, typename CharT>
  template <typename Menu>
  constexpr void PageLink<Name, TextSize, CharT>::set_menu(Menu* menu) noexcept {
    static_assert(
        sgl::contains_v<Name, typename Menu::name_list>,
        "PageLink doe not link to a valid page. menu does not contain a page with this name");
    menu_ = static_cast<void*>(menu);
    this->set_click_handler([](auto& page_link) noexcept -> sgl::error {
      return static_cast<Menu*>(page_link.get_menu())
          ->set_current_page(Name{}); // setting active page by name
    });
  }

  template <typename Name, size_t TextSize, typename CharT>
  constexpr void* PageLink<Name, TextSize, CharT>::get_menu() noexcept {
    return menu_;
  }

  template <char... Chars>
  constexpr PageLink<sgl::Name<Chars...>, sizeof...(Chars), char>
      pagelink(sgl::Name<Chars...> page_name) noexcept {
    return {page_name};
  }

  template <size_t TextSize, typename CharT, char... Chars>
  constexpr auto pagelink(sgl::Name<Chars...> page_name, const CharT (&text)[TextSize ]) noexcept
      -> PageLink<sgl::Name<Chars...>, TextSize-1, CharT> {
    return {page_name, sgl::string_view<CharT>(text)};
  }

  template <size_t TextSize, typename CharT, char... Chars>
  constexpr auto pagelink(sgl::Name<Chars...> page_name, sgl::string_view<CharT> text) noexcept
      -> PageLink<sgl::Name<Chars...>, TextSize, CharT> {
    return {page_name, text};
  }
} // namespace sgl
#endif /* SGL_IMPL_PAGE_LINK_IMPL_HPP */
