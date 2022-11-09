#ifndef SGL_ITEM_TRAITS_HPP
#define SGL_ITEM_TRAITS_HPP
#include "sgl/fwd.hpp"
#include "sgl/item_concepts.hpp"

namespace sgl {
  /// default traits structure passed to ItemBase when deriving from it. Not defined intentionally,
  /// so that manual specialization is required.
  template <typename ItemType>
  struct ItemTraits {
    static_assert(
        detail::always_false<ItemType>,
        "This traits structure is not correctly defined. Either manually specialize "
        "sgl::ItemTraits or implement your own traits struct. You can statically assert that "
        "your traits structure is correctly implemented by using "
        "sgl::is_item_trait_v<YourTraitsType>");
  };

  /// \cond

  // button item traits
  template <size_t TextSize, typename CharT>
  struct ItemTraits<sgl::Button<TextSize, CharT>> {
    using item_type = sgl::Button<TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    static constexpr bool   clickable = true;
  };

  // boolean item traits
  template <size_t TextSize, typename CharT>
  struct ItemTraits<sgl::Boolean<TextSize, CharT>> {
    using item_type = sgl::Boolean<TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    static constexpr bool   clickable = true;
  };

  // enum item traits
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  struct ItemTraits<sgl::Enum<T, NumEnumerators, TextSize, CharT>> {
    using item_type = sgl::Enum<T, NumEnumerators, TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
  };

  // page link item traits
  template <typename PageName, size_t TextSize, typename CharT>
  struct ItemTraits<sgl::PageLink<PageName, TextSize, CharT>> {
    using item_type = sgl::PageLink<PageName, TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    static constexpr bool   clickable = true;
  };

  // numeric item traits
  template <size_t TextSize, typename CharT, typename T>
  struct ItemTraits<sgl::Numeric<TextSize, CharT, T>> {
    using item_type = sgl::Numeric<TextSize, CharT, T>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
  };

  /// \endcond
} // namespace sgl

#endif /* SGL_ITEM_TRAITS_HPP */
