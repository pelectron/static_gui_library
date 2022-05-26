#ifndef SGL_PAGE_LINK_HPP
#define SGL_PAGE_LINK_HPP
#include "sgl_button.hpp"
namespace sgl {

  /**
   * @brief This class models a page link.
   *
   * @tparam LineWidth display width
   * @tparam CharT character type
   * @addtogroup item_types Item Types
   * @{
   */
  template <size_t LineWidth, typename CharT>
  class PageLink_t : public Button_t<LineWidth, CharT> {
  public:
    using string_view_t = typename sgl::Item_t<LineWidth, CharT>::string_view_t;
    /**
     * @brief Construct a new PageLink_t
     * 
     * @param item_name name of the link
     * @param text text of the link
     * @param page_to_link name of the page to link to
     */
    PageLink_t(string_view_t item_name,
               string_view_t text,
               string_view_t page_to_link)
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

    constexpr string_view_t page_name() const { return page_name_; }

  private:
    string_view_t page_name_;
  };
  /// @}

} // namespace sgl
#endif