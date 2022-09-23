/**
 * \file sgl/page_link.hpp
 * \author Pelé Constam (you@domain.com)
 * \version 0.1
 * @date 2022-09-23
 * 
 * @copyright Copyright (c) 2022
 * 
*/ 
#ifndef SGL_PAGE_LINK_HPP
#define SGL_PAGE_LINK_HPP
#include "sgl/fwd.hpp"
#include "sgl/named_value.hpp"
#include "sgl/pass_through_button.hpp"

namespace sgl {

  /// \ingroup item_types
  /// \brief A page link is a button item, that, when clicked, causes the menu to switch to the page
  /// specified by the link.
  /// \tparam chars characters of the page name
  /// \tparam TextSize text size of the link
  /// \tparam CharT character type of the pagelink
  template <char... chars, size_t TextSize, typename CharT>
  class PageLink<Name<chars...>, TextSize, CharT>
      : public sgl::PassThroughButton<PageLink<Name<chars...>, TextSize, CharT>> {
  public:
    /// base class of PageLink
    using Base = sgl::PassThroughButton<PageLink<Name<chars...>, TextSize, CharT>>;
    /// string_view type of this item
    using StringView = typename Base::StringView;

    /// \brief create a page link with the name of the page it should link to. The name will also be
    /// used to set this item's text. The arguments type must be sgl::Name<chars...>, where
    /// chars.. is the char sequence which makes up the page name.
    ///
    /// \code
    ///  // version 1 : Directly from NAME macro
    ///  auto link = PageLink(NAME("home_page")); // links to a page called "home_page" in menu
    ///
    /// // version 2 : from
    /// static constexpr auto page_to_link_to = NAME("Foo"); // defined somewhere
    ///
    /// auto link2 = PAGELINK(page_to_link_to);
    /// \endcode
    ///
    /// \see sgl::Name for more info.
    /// \param page_to_link name of the page to link to.
    ///
    explicit constexpr PageLink(Name<chars...> page_to_link) noexcept;

    /// \brief create page link with page name to link to and text to show
    /// \param page_to_link name of page to link to.
    /// \param text text to show.
    constexpr PageLink(Name<chars...> page_to_link, StringView text) noexcept;

    /// \brief static overload of ItemBase::set_menu. The menu type cannot be a
    /// part of the template parameters given to a page link on construction. As
    /// such, the type of the menu and access to page changing functions can be accessed with this
    /// helper. sgl::Menu will call set_menu() on all items in it constructors. \tparam Menu menu
    /// type. \param menu pointer to menu instance.
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /// get name of the page to link to.
    /// \return sgl::string_view<char>
    [[nodiscard]] constexpr string_view<char> page_name() const noexcept {
      return Name<chars...>::view;
    }
    /// get pointer to menu as void*
    /// \return void*
    [[nodiscard]] constexpr void* get_menu() noexcept;

  private:
    void* menu_{nullptr}; ///< pointer to menu containing this link
  };

  /// \cond
  template <char... chars, size_t TextSize, typename CharT>
  PageLink(Name<chars...> page_name, const CharT (&text)[TextSize])
      -> PageLink<Name<chars...>, TextSize - 1, CharT>;

  template <char... chars>
  PageLink(Name<chars...> page_name) -> PageLink<Name<chars...>, sizeof...(chars), char>;
  /// \endcond

  /// \ingroup item_factories
  /// \addtogroup page_link_factories
  /// \{

  /// \brief create a pagelink item with only the name of the page to link to.
  /// \param page_name name of the page to link to.
  template <char... chars>
  constexpr PageLink<Name<chars...>, sizeof...(chars), char>
      make_pagelink(Name<chars...> page_name);

  /// \brief create a pagelink with name and text literal. Name, Character type and textsize are
  /// automatically deduced.
  /// \code {.cpp}
  /// // TextSize of link is 14, charT is char, Name = sgl::Name<'a',' ','n','a','m','e'>
  /// auto link = make_pagelink(SGL_NAME("a name"), "displayed text");
  /// \endcode
  ///
  /// \tparam Name type of page name
  /// \tparam CharT character type of pagelink
  /// \tparam N size of text literal
  /// \param page_name name of the page to link to
  /// \param text text of the pagelink
  /// \return PageLink<Name, N - 1, CharT>
  template <typename Name, typename CharT, size_t N>
  constexpr PageLink<Name, N - 1, CharT> make_pagelink(Name page_name, const CharT (&text)[N]);

  /// \brief create pagelink by explicitly specifying the textsize. The rest of the template
  /// arguments are deduced.
  /// \code {.cpp}
  /// auto link = make_pagelink<40>(SGL_NAME("a name"), "displayed text"_sv);
  /// \endcode
  ///
  /// \tparam TextSize text size of pagelink
  /// \tparam CharT character type
  /// \tparam Name type of page name
  /// \param page_name name of the page to link to
  /// \param text text of the pagelink
  /// \return PageLink<Name, TextSize, CharT>
  template <size_t TextSize, typename CharT, typename Name>
  constexpr PageLink<Name, TextSize, CharT> make_pagelink(Name                    page_name,
                                                          sgl::string_view<CharT> text);
  /// \}
} // namespace sgl
#include "sgl/impl/page_link_impl.hpp"
#endif