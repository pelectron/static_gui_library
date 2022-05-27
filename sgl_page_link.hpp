#ifndef SGL_PAGE_LINK_HPP
#define SGL_PAGE_LINK_HPP
#include "sgl_button.hpp"
namespace sgl {

  /**
   * @brief This class models a page link. Clicking a page link item will change
   * the menus active page to the page referenced in the page_to_link parameter
   * in the constructors. Pages are linked by name, as to keep everything index
   * and type free(but still typesafe, see set_menu() for details).
   *
   * @tparam LineWidth display width
   * @tparam CharT character type
   */
  template <size_t LineWidth, typename CharT>
  class PageLink_t : public sgl::Button_t<LineWidth, CharT> {
  public:
    using StringView = typename sgl::Button_t<LineWidth, CharT>::StringView;

    /**
     * @brief Construct a page link with a name, text, and name of the page to
     * link to
     * @param item_name name of the link
     * @param text text of the link
     * @param page_to_link name of the page to link to
     */
    PageLink_t(StringView item_name, StringView text, StringView page_to_link)
        : Button_t<LineWidth, CharT>(item_name, text),
          page_name_(page_to_link) {}

    /**
     * @brief Construct a page link with name and text, and page to link to.
     * @param name_and_text
     * @param page_to_link
     */
    PageLink_t(StringView name_and_text, StringView page_to_link)
        : Button_t<LineWidth, CharT>(name_and_text), page_name_(page_to_link) {}

    /**
     * @brief static overload of Item_t::set_menu. The Menu type cannot be a
     * part of the template parameters given to a page link on construction. As
     * such, the type and access to page changing functions can  made here.
     * Menu_t will call set_menu() on all items in it constructors.
     * @tparam Menu menu type.
     * @param menu pointer to menu instance.
     */
    template <typename Menu>
    void set_menu(Menu* menu) {
      static_assert(sgl::is_menu_v<Menu>, "Menu type must be a menu");
      // simply set the button click handler as a lambda which captures the Menu
      // pointer by value.
      auto handler = [menu](Button_t<LineWidth, CharT>& page_link) {
        // casting to correct type from Button_t
        auto& link = static_cast<PageLink_t<LineWidth, CharT>&>(page_link);
        // set active page by name.
        return menu->set_active_page(
            link.page_name()); // setting active page by name
      };

      this->set_click_handler(std::move(handler));
    }

    /// get name of the page to link to.
    constexpr StringView page_name() const { return page_name_; }

  private:
    StringView page_name_; /// name of the page to link to.
  };

} // namespace sgl
#endif