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

  template <char... chars, size_t TextSize, typename CharT>
  constexpr PageLink<Name<chars...>, TextSize, CharT>::PageLink(
      Name<chars...> page_to_link) noexcept
      : Base(sgl::string_view<char>(Name<chars...>{})) {
    static_cast<void>(page_to_link);
  }

  template <char... chars, size_t TextSize, typename CharT>
  constexpr PageLink<Name<chars...>, TextSize, CharT>::PageLink(Name<chars...> page_to_link,
                                                                StringView     text) noexcept
      : Base(text) {
    static_cast<void>(page_to_link);
  }

  template <char... chars, size_t TextSize, typename CharT>
  template <typename Menu>
  constexpr void PageLink<Name<chars...>, TextSize, CharT>::set_menu(Menu* menu) noexcept {
    static_assert(
        sgl::contains_v<Name<chars...>, typename Menu::name_list>,
        "PageLink doe not link to a valid page. menu does not contain a page with this name");
    menu_ = static_cast<void*>(menu);
    this->set_click_handler([](auto& page_link) noexcept -> sgl::error {
      return static_cast<Menu*>(page_link.get_menu())
          ->set_current_page(Name<chars...>{}); // setting active page by name
    });
  }

  template <char... chars, size_t TextSize, typename CharT>
  constexpr void* PageLink<Name<chars...>, TextSize, CharT>::get_menu() noexcept {
    return menu_;
  }

  template <char... chars>
  constexpr PageLink<Name<chars...>, sizeof...(chars), char>
      make_pagelink(Name<chars...> page_name) {
    return PageLink(page_name);
  }

  template <typename Name, typename CharT, size_t N>
  constexpr PageLink<Name, N - 1, CharT> make_pagelink(Name page_name, const CharT (&text)[N]) {
    return PageLink<Name, N - 1, CharT>(page_name, sgl::string_view<CharT>(text));
  }

  template <size_t TextSize, typename CharT, typename Name>
  constexpr PageLink<Name, TextSize, CharT> make_pagelink(Name                    page_name,
                                                          sgl::string_view<CharT> text) {
    return PageLink<Name, TextSize, CharT>(page_name, text);
  }
} // namespace sgl
#endif /* SGL_IMPL_PAGE_LINK_IMPL_HPP */
