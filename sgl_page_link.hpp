#ifndef SGL_PAGE_LINK_HPP
#define SGL_PAGE_LINK_HPP
#include "sgl_button.hpp"
namespace sgl {

  /**
   * @brief
   *
   * @tparam LineWidth
   * @tparam CharT
   */
  template <size_t LineWidth, typename CharT>
  class PageLink_t : public Button_t<LineWidth, CharT> {
  public:
    PageLink_t(string_view<CharT> item_name,
               string_view<CharT> text,
               string_view<CharT> page_to_link)
        : Button_t<LineWidth, CharT>(item_name, text),
          page_name_(page_to_link) {}
    // static overload of Item_t::set_menu
    template <typename Menu>
    void set_menu(Menu* menu) {
      this->set_click_handler(
          [m = menu](Button_t<LineWidth, CharT>& page_link) {
            auto& link = static_cast<PageLink_t<LineWidth, CharT>&>(
                page_link); // casting to correct type
            return m->set_active_page(
                link.page_name()); // setting active page by name
          });
    }

    constexpr string_view<CharT> page_name() const { return page_name_; }

  private:
    string_view<CharT> page_name_;
  };

  template <size_t LineWidth, typename CharT>
  struct is_item<PageLink_t<LineWidth, CharT>> : std::true_type {};

} // namespace sgl
#endif