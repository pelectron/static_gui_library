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
     * \brief Construct a page link with a name, text, and name of the page to
     * link to
     * \param name name of the link
     * \param text text of the link
     * \param page_to_link name of the page to link to
     */
    constexpr PageLink(StringView name, StringView text, StringView page_to_link) noexcept;

    /**
     * \brief Construct a page link with name and text, and page to link to.
     * \param name_and_text
     * \param page_to_link
     */
    constexpr PageLink(StringView name_and_text, StringView page_to_link) noexcept;

    /**
     * \brief static overload of item_type::set_menu. The Menu type cannot be a
     * part of the template parameters given to a page link on construction. As
     * such, the type and access to page changing functions can  made here.
     * Menu_t will call set_menu() on all items in it constructors.
     * \tparam Menu menu type.
     * \param menu pointer to menu instance.
     */
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /// get name of the page to link to.
    constexpr StringView page_name() const noexcept { return page_name_; }

    constexpr void* get_menu() noexcept;

  private:
    template <typename T, typename = void>
    struct validate_menu : std::false_type {};
    template <typename T>
    struct validate_menu<T,
                         std::void_t<decltype(std::declval<T>().set_active_page(size_t{0})),
                                decltype(std::declval<T>().set_active_page(std::declval<StringView>()))>> {
      static constexpr bool value =
          std::is_same_v<sgl::error, decltype(std::declval<T>().set_active_page(size_t{0}))> and
          std::is_same_v<sgl::error, decltype(std::declval<T>().set_active_page(std::declval<StringView>()))>;
    };

    template <typename T>
    static constexpr bool validate_menu_v = validate_menu<T>::value;

    StringView page_name_;     ///< name of the page to link to.
    void*      menu_{nullptr}; ///< pointer to menu containing this link
  };
  /// deduction guides for PageLink
  /// \{
  template <typename CharT, size_t N1, size_t N2, size_t N3>
  PageLink(const CharT (&name)[N1], const CharT (&text)[N2], const CharT (&page_name)[N3])
      -> PageLink<N2, CharT>;

  template <typename CharT, size_t N1, size_t N2>
  PageLink(const CharT (&name)[N1], const CharT (&page_name)[N2]) -> PageLink<N1, CharT>;
  /// \}
} // namespace sgl
#include "impl/sgl_page_link_impl.hpp"
#endif