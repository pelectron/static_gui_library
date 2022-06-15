#ifndef SGL_ITEM_BASE_DETAIL_HPP
#define SGL_ITEM_BASE_DETAIL_HPP
#include "item_fwd.hpp"
#include "sgl_item_concepts.hpp"

namespace sgl {
  template <typename T, typename = void>
  struct has_item_type_typedef : false_type {};
  template <typename T>
  struct has_item_type_typedef<T, void_t<typename T::item_type>> : true_type {};

  template <typename T, typename = void>
  struct has_char_type_typedef : false_type {};
  template <typename T>
  struct has_char_type_typedef<T, void_t<typename T::char_type>> : true_type {};

  template <typename T, typename = void>
  struct has_text_size : false_type {};

  template <typename T>
  struct has_text_size<T, void_t<decltype(T::text_size)>> {
    static constexpr bool value = is_convertible_v<decltype(T::text_size), size_t>;
  };

  template <typename T, typename = void>
  struct has_clickable : false_type {};

  template <typename T>
  struct has_clickable<T, void_t<decltype(T::clickable)>> {
    static constexpr bool value = is_convertible_v<decltype(T::clickable), bool>;
  };
  template <typename T, bool has_click = false>
  struct get_clickable : false_type {};
  template <typename T>
  struct get_clickable<T, true> {
    static constexpr bool value = T::clickable;
  };
  template <typename T>
  static constexpr bool has_item_type_typedef_v = has_item_type_typedef<T>::value;
  template <typename T>
  static constexpr bool has_char_type_typedef_v = has_char_type_typedef<T>::value;
  template <typename T>
  static constexpr bool has_text_size_v = has_text_size<T>::value;

  template <typename T>
  static constexpr bool has_clickable_v = has_clickable<T>::value;

  /// evaluates to false if has_clickable_v<T> is false, else it evaluates to T::clickable
  template <typename T>
  static constexpr bool get_clickable_v = get_clickable<T, has_clickable_v<T>>::value;

  template <typename T>
  struct is_item_trait {
    static constexpr bool value = has_char_type_typedef_v<T> and has_text_size_v<T>;
  };
  
  template <typename T>
  static constexpr bool is_item_trait_v = is_item_trait<T>::value;

  /// default traits structure passed to ItemBase when deriving from it. Not defined intentionally,
  /// so that manual specialization is required.
  template <typename ItemType>
  struct ItemTraits {
    static_assert(
        always_false<ItemType>,
        "This traits structure is not correctly defined. Either manually specialize it "
        "ItemTraits or implement your own. You can statically assert that your traits structure is "
        "correctly implemented by using sgl::is_item_trait_v<YourTraitsType> template");
  };

  template <size_t TextSize, typename CharT>
  struct ItemTraits<Button<TextSize, CharT>> {
    using item_type = Button<TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    static constexpr bool   clickable = true;
  };

  template <size_t TextSize, typename CharT>
  struct ItemTraits<Boolean<TextSize, CharT>> {
    using item_type = Boolean<TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    static constexpr bool   clickable = true;
  };

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  struct ItemTraits<Enum<T, NumEnumerators, TextSize, CharT>> {
    using item_type = Enum<T, NumEnumerators, TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
  };

  template <size_t TextSize, typename CharT>
  struct ItemTraits<PageLink<TextSize, CharT>> {
    using item_type = PageLink<TextSize, CharT>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    static constexpr bool   clickable = true;
  };

  template <size_t TextSize, typename CharT, typename T>
  struct ItemTraits<Numeric<TextSize, CharT, T>> {
    using item_type = Numeric<TextSize, CharT, T>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
  };
} // namespace sgl
#endif