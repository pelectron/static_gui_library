/// \file sgl/page_link.hpp
/// \author Pelé Constam (pelectron1602@gmail.com)
/// \version 0.1
/// \date 2022-09-23
///
/// \copyright Copyright (c) 2022

//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_PAGE_LINK_HPP
#define SGL_PAGE_LINK_HPP
#include "sgl/fwd.hpp"
#include "sgl/named_value.hpp"
#include "sgl/pass_through_button.hpp"

namespace sgl {
  /**
   * \ingroup item_types
   * \headerfile page_link.hpp "sgl/page_link.hpp"
   * \brief A page link is a button item, that, when clicked, causes the menu to switch to the page
   * specified by the link. To create a PageLink, there are three ways:
   * \code
   *  // version 1 : Directly from NAME macro
   *  auto link1 = PageLink(NAME("home_page")); // link2's text is "home_page" and links to a
   *  page named "home_page"
   *
   * // version 2 : from static name
   * static constexpr auto page_to_link_to = NAME("Foo"); // defined somewhere
   *
   * auto link2 = PageLink(page_to_link_to); // link2's text is "Foo" and links to a page called
   * "Foo"
   *
   * // or with different text
   * auto link3 = PageLink(NAME("home_page"), "back"); // link3's text is "back" and links to a
   * page called "home_page".
   * \endcode
   * \tparam chars characters of the page name
   * \tparam TextSize text size of the link
   * \tparam CharT character type of the pagelink
   */
  template <char... chars, size_t TextSize, typename CharT>
  class PageLink<sgl::Name<chars...>, TextSize, CharT>
      : public sgl::PassThroughButton<PageLink<sgl::Name<chars...>, TextSize, CharT>> {
  public:
    /// base class of PageLink
    using Base = sgl::PassThroughButton<PageLink<sgl::Name<chars...>, TextSize, CharT>>;
    /// string_view type of this item
    using StringView = typename Base::StringView;
    /**
     * \brief create a page link with the \ref sgl::Name "name" of the page it should link to. The
     * name will also be used to set the links's text. \see sgl::Name for more info. \param
     * page_to_link name of the page to link to.
     */
    explicit constexpr PageLink(sgl::Name<chars...> page_to_link) noexcept;

    /// \brief create page link with page name to link to and text to show
    /// \param page_to_link name of page to link to.
    /// \param text text to show.
    constexpr PageLink(sgl::Name<chars...> page_to_link, StringView text) noexcept;
    /**
     * \brief static overload of ItemBase::set_menu.
     * \details The menu type cannot be a part of the template parameters given to a page link on
     * construction. As such, the type of the menu and access to page changing functions can be
     * accessed with this helper. sgl::Menu will call set_menu() on all its pages , and as such all its items, in it constructors.
     * \tparam Menu menu type.
     * \param menu pointer to menu instance.
     */
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /// get name of the page to link to.
    /// \return sgl::string_view<char>
    [[nodiscard]] constexpr sgl::string_view<char> page_name() const noexcept {
      return sgl::Name<chars...>::view;
    }

    /// get pointer to menu as void*
    /// \return void*
    [[nodiscard]] constexpr void* get_menu() noexcept;

  private:
    void* menu_{nullptr}; ///< pointer to menu containing this link
  };

  /// \cond
  template <char... chars, size_t TextSize, typename CharT>
  PageLink(sgl::Name<chars...> page_name, const CharT (&text)[TextSize])
      -> PageLink<sgl::Name<chars...>, TextSize - 1, CharT>;

  template <char... chars>
  PageLink(sgl::Name<chars...> page_name) -> PageLink<sgl::Name<chars...>, sizeof...(chars), char>;
  /// \endcond

  /// \ingroup item_factories
  /// \addtogroup page_link_factories
  /// \{

  /// \brief create a pagelink item with only the name of the page to link to.
  /// \param page_name name of the page to link to.
  template <char... chars>
  constexpr PageLink<sgl::Name<chars...>, sizeof...(chars), char>
      make_pagelink(sgl::Name<chars...> page_name);

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
#endif /* SGL_PAGE_LINK_HPP */
