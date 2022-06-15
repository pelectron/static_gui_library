#ifndef SGL_PAGE_LINK_IMPL_HPP
#define SGL_PAGE_LINK_IMPL_HPP
#include "../sgl_page_link.hpp"
namespace sgl{
  template <size_t TextSize, typename CharT>
  constexpr PageLink<TextSize, CharT>::PageLink(StringView name,
                                                StringView text,
                                                StringView page_to_link) noexcept
      : Base(name, text), page_name_(page_to_link) {}

  template <size_t TextSize, typename CharT>
  constexpr PageLink<TextSize, CharT>::PageLink(StringView name_and_text,
                                                StringView page_to_link) noexcept
      : Base(name_and_text), page_name_(page_to_link) {}

  template <size_t TextSize, typename CharT>
  template <typename Menu>
  constexpr void PageLink<TextSize, CharT>::set_menu(Menu* menu) noexcept {
    // simply set the button click handler as a lambda which captures the Menu
    // pointer by value.
    static_assert(
        PageLink<TextSize, CharT>::template validate_menu_v<Menu>,
        "The menu's set_active_page method cannot be invoked with this item's StringView type");
    menu_ = static_cast<void*>(menu);
    this->set_click_handler(+[](PageLink<TextSize, CharT>& page_link) noexcept {
      return static_cast<Menu*>(page_link.get_menu())
          ->set_active_page(page_link.page_name()); // setting active page by name
    });
  }

  template <size_t TextSize, typename CharT>
  constexpr void* PageLink<TextSize, CharT>::get_menu() noexcept {
    return menu_;
  }
}
#endif