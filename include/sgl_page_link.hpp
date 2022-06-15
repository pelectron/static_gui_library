#ifndef SGL_BASIC_PAGE_LINK
#define SGL_BASIC_PAGE_LINK
#include "sgl_pass_through_button.hpp"
namespace sgl {
  template <size_t TextSize, typename CharT>
  class PageLink : public sgl::PassThroughButton<PageLink<TextSize, CharT>> {
  public:
    static constexpr size_t text_size = TextSize;
    using Base = sgl::PassThroughButton<PageLink<TextSize, CharT>>;
    using StringView = typename Base::StringView;

    /**
     * @brief Construct a page link with a name, text, and name of the page to
     * link to
     * @param name name of the link
     * @param text text of the link
     * @param page_to_link name of the page to link to
     */
    constexpr PageLink(StringView name, StringView text, StringView page_to_link) noexcept;

    /**
     * @brief Construct a page link with name and text, and page to link to.
     * @param name_and_text
     * @param page_to_link
     */
    constexpr PageLink(StringView name_and_text, StringView page_to_link) noexcept;

    /**
     * @brief static overload of item_type::set_menu. The Menu type cannot be a
     * part of the template parameters given to a page link on construction. As
     * such, the type and access to page changing functions can  made here.
     * Menu_t will call set_menu() on all items in it constructors.
     * @tparam Menu menu type.
     * @param menu pointer to menu instance.
     */
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /// get name of the page to link to.
    constexpr StringView page_name() const noexcept { return page_name_; }

    constexpr void* get_menu() noexcept;

  private:
    template <typename T, typename = void>
    struct validate_menu : false_type {};
    template <typename T>
    struct validate_menu<T,
                         void_t<decltype(declval<T>().set_active_page(size_t{0})),
                                decltype(declval<T>().set_active_page(declval<StringView>()))>> {
      static constexpr bool value =
          is_same_v<sgl::error, decltype(declval<T>().set_active_page(size_t{0}))> and
          is_same_v<sgl::error, decltype(declval<T>().set_active_page(declval<StringView>()))>;
    };

    template <typename T>
    static constexpr bool validate_menu_v = validate_menu<T>::value;

    StringView page_name_; ///< name of the page to link to.
    void*      menu_{nullptr};
  };

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
} // namespace sgl
#endif