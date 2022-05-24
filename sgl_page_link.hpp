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
    PageLink_t(string_view<CharT> name,
               string_view<CharT> text,
               string_view<CharT> page_to_link)
        : Button_t<LineWidth, CharT>(name, text), page_name(page_to_link) {}

    template <typename Menu>
    void set_menu(Menu* menu) {
      this->menu = static_cast<void*>(menu);
      this->set_click_handler(
          &PageLink_t<LineWidth, CharT>::handle_page_click<Menu>);
    }

  private:
    void*                    menu{nullptr};
    string_view<CharT> page_name;

    template <typename Menu>
    static error handle_page_click(Button_t<LineWidth, CharT>& page_link) {
      auto& link = static_cast<PageLink_t<LineWidth, CharT>&>(page_link);
      static_cast<Menu*>(link.menu)->set_active_page(link.page_name);
      return error::no_error;
    }
  };
} // namespace sgl
#endif