//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_ITEM_TRAITS_HPP
#define SGL_ITEM_TRAITS_HPP
#include "sgl/fwd.hpp"
#include "sgl/item_concepts.hpp"

namespace sgl {
  /// \cond
  namespace traits_detail {
    template <typename T>
    static constexpr bool always_false = false;

    template <typename T, typename = void>
    struct has_item_type_typedef : std::false_type {};

    template <typename T>
    struct has_item_type_typedef<T, std::void_t<typename T::item_type>> : std::true_type {};

    template <typename T, typename = void>
    struct has_char_type_typedef : std::false_type {};

    template <typename T>
    struct has_char_type_typedef<T, std::void_t<typename T::char_type>> : std::true_type {};

    template <typename T, typename = void>
    struct has_text_size : std::false_type {};

    template <typename T>
    struct has_text_size<T, std::void_t<decltype(T::text_size)>> {
      static constexpr bool value = std::is_same_v<decltype(T::text_size), const size_t>;
    };

    template <typename T, typename = void>
    struct has_clickable : std::false_type {};

    template <typename T>
    struct has_clickable<T, std::void_t<decltype(T::clickable)>> {
      static constexpr bool value = std::is_same_v<decltype(T::clickable), const bool>;
    };

    template <typename T, bool has_click = false>
    struct get_clickable : std::false_type {};

    template <typename T>
    struct get_clickable<T, true> {
      static constexpr bool value = T::clickable;
    };
  } // namespace traits_detail

  /// @brief evaluates to true if T has han inner typename item_type.
  /// @tparam T type to check
  template <typename T>
  static constexpr bool has_item_type_typedef_v = traits_detail::has_item_type_typedef<T>::value;

  /// @brief evaluates to true if T has han inner typename char_type.
  /// @tparam T type to check
  template <typename T>
  static constexpr bool has_char_type_typedef_v = traits_detail::has_char_type_typedef<T>::value;

  /// @brief evaluates to true if the expression ``T::text_size`` is a constant of type```size_t``.
  /// @tparam T type to check
  template <typename T>
  static constexpr bool has_text_size_v = traits_detail::has_text_size<T>::value;

  /// @brief evaluates to ``T::clickable`` if the type of `T::clickable`` is bool
  /// @tparam T type to check
  template <typename T>
  static constexpr bool has_clickable_v = traits_detail::has_clickable<T>::value;

  /// evaluates to false if has_clickable_v<T> is false, else it evaluates to T::clickable
  /// @tparam T type to check
  template <typename T>
  static constexpr bool get_clickable_v =
      traits_detail::get_clickable<T, has_clickable_v<T>>::value;

  template <typename T>
  struct is_item_trait {
    static constexpr bool value =
        has_char_type_typedef_v<T> and has_item_type_typedef_v<T> and has_text_size_v<T>;
  };

  /// \endcond

  /// @brief check if T is an item trait.
  /// @tparam T type to check
  template <typename T>
  static constexpr bool is_item_trait_v = is_item_trait<T>::value;

  /// default traits structure passed to ItemBase when deriving from it. Not defined intentionally,
  /// so that manual specialization is required.
  template <typename ItemType>
  struct ItemTraits {
    static_assert(traits_detail::always_false<ItemType>,
                  "This traits structure is not correctly defined. Either manually specialize "
                  "ItemTraits or implement your own traits struct. You can statically assert that "
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
