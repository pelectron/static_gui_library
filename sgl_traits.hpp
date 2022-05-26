#ifndef SGL_TRAITS_HPP
#define SGL_TRAITS_HPP
#include "sgl_fwd.hpp"
namespace sgl {
  /// type trait to get the number of pages in a menu.
  template <typename Menu>
  struct number_of_pages;

  template <size_t LineWidth, typename CharT, typename... Pages>
  struct number_of_pages<Menu_t<LineWidth, CharT, Pages...>> {
    static constexpr size_t value = sizeof...(Pages);
  };
  /// type trait to determine the number of items stored in a page.
  template <typename Page>
  struct number_of_items;

  template <size_t LineWidth, typename CharT, typename... Items>
  struct number_of_items<Page_t<LineWidth, CharT, Items...>> {
    static constexpr size_t value = sizeof...(Items);
  };

  template <typename Menu>
  static constexpr size_t number_of_pages_v = number_of_pages<Menu>::value;

  template <typename Item>
  struct is_item : std::false_type {};
  template <size_t LineWidth, typename CharT>
  struct is_item<Item_t<LineWidth, CharT>> : std::true_type {};
  template <typename Item>
  static constexpr bool is_item_v = is_item<Item>::value;

  template <size_t LineWidth, typename CharT>
  struct is_item<Boolean_t<LineWidth, CharT>> : std::true_type {};

  template <size_t LineWidth, typename CharT>
  struct is_item<Button_t<LineWidth, CharT>> : std::true_type {};

  template <typename EnumT,
            size_t NumEnumerators,
            size_t LineWidth,
            typename CharT>
  struct is_item<Enum_t<EnumT, NumEnumerators, LineWidth, CharT>>
      : std::true_type {};

  template <typename T, size_t LineWidth, typename CharT>
  struct is_item<Numeric_t<T, LineWidth, CharT>> : std::true_type {};

  template <size_t LineWidth, typename CharT>
  struct is_item<PageLink_t<LineWidth, CharT>> : std::true_type {};

  template <size_t LineWidth, typename CharT>
  struct is_item<ConstText_t<LineWidth, CharT>> : std::true_type {};

  template <size_t LineWidth, typename CharT>
  struct is_item<Text_t<LineWidth, CharT>> : std::true_type {};
} // namespace sgl
#endif