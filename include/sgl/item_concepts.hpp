//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_ITEM_CONCEPTS_HPP
#define SGL_ITEM_CONCEPTS_HPP

#include "sgl/error.hpp"
#include "sgl/format.hpp"
#include "sgl/input.hpp"
#include "sgl/static_string.hpp"

#include <type_traits>

namespace sgl {
  namespace detail {
    template <typename T>
    static constexpr bool always_false = false;

    template <typename T, typename = void>
    struct has_item_type_typedef : std::false_type {};

    template <typename T>
    struct has_item_type_typedef<T, std::void_t<typename T::item_type>> : std::true_type {};

    template <typename T, typename = void>
    struct has_char_type_typedef : std::false_type {};

    template <typename T>
    struct has_char_type_typedef<T, std::void_t<typename T::char_type>> {
      using Ch = typename T::char_type;
      static constexpr bool value =
          std::is_same_v<Ch, char> or std::is_same_v<Ch, char16_t> or std::is_same_v<Ch, char32_t>;
    };

    template <typename T, typename = void>
    struct has_text_size : std::false_type {};

    template <typename T>
    struct has_text_size<T, std::void_t<decltype(T::text_size)>> {
      static constexpr bool value = std::is_convertible_v<decltype(T::text_size), size_t>;
    };

    /// @brief evaluates to true if T has han inner typename item_type.
    /// @tparam T type to check
    template <typename T>
    static constexpr bool has_item_type_typedef_v = detail::has_item_type_typedef<T>::value;

    /// @brief evaluates to true if T has han inner typename char_type.
    /// @tparam T type to check
    template <typename T>
    static constexpr bool has_char_type_typedef_v = detail::has_char_type_typedef<T>::value;

    /// @brief evaluates to true if the expression ``T::text_size`` is a constant of
    /// type```size_t``.
    /// @tparam T type to check
    template <typename T>
    static constexpr bool has_text_size_v = detail::has_text_size<T>::value;

  } // namespace detail

  template <typename T>
  struct is_item_trait {
    static constexpr bool value = detail::has_char_type_typedef_v<T> and detail::has_text_size_v<T>;
  };

  /// @brief check if T is an Item Trait. An a class T must satisfy the following to be an Item
  /// Trait:
  ///
  /// expression or typename | type                        | notes                         |
  /// -----------------------|-----------------------------|-------------------------------|
  /// typename T::char_type  | char, char16_t or char32_t. |                               |
  /// T::text_size           | size_t                      | must be a constant expression |
  ///
  /// @tparam T type to check
  template <typename T>
  static constexpr bool is_item_trait_v = is_item_trait<T>::value;

  /// \addtogroup handler_traits Handler traits
  /// \ingroup sgl_traits
  /// \{

  /// @brief true if F is a valid input handler for Item.
  /// Look [here](markdown/concepts.md#input-handler) for more info.
  /// @tparam F handler type
  /// @tparam Item item type
  template <typename F, typename Item>
  inline constexpr bool is_input_handler_for_v =
      std::is_nothrow_invocable_r_v<sgl::error, F, Item&, sgl::Input>and
          std::is_trivially_destructible_v<F>and std::is_trivially_move_constructible_v<F>and
                                                 std::is_trivially_copyable_v<F>;

  /// @brief true if F is a valid click handler for Item.
  /// Look [here](markdown/concepts.md#click-handler) for more info.
  /// @tparam F handler type
  /// @tparam Item item type
  template <typename F, typename Item>
  static constexpr bool                                      is_click_handler_for_v =
      std::is_nothrow_invocable_r_v<sgl::error, F, Item&>and std::is_trivially_destructible_v<F>and
          std::is_trivially_move_constructible_v<F>and std::is_trivially_copyable_v<F>;

  /// @brief true if F is a valid value formatter for Item.
  /// Look [here](markdown/concepts.md#formatter) for more info.
  /// @tparam F handler type
  /// @tparam Item item type
  template <typename F, typename Item>
  static constexpr bool                             is_value_formatter_v =
      std::is_nothrow_invocable_r_v<sgl::format_result,
                                    F,
                                    typename Item::char_type*,
                                    size_t,
                                    typename Item::value_type,
                                    uint32_t,
                                    sgl::format>and std::is_trivially_destructible_v<F>and
          std::is_trivially_move_constructible_v<F>and std::is_trivially_copyable_v<F>;

  /// \brief true if F is a TickHandler for Item.
  /// @tparam F handler type
  /// @tparam Item item type
  template <typename F, typename Item>
  inline constexpr bool is_tick_handler_for_v = std::is_nothrow_invocable_r_v<void, F, Item&>;

  /// \}

  template <typename F, typename Item>
  using enable_if_is_input_handler =
      std::enable_if_t<is_input_handler_for_v<F, std::decay_t<Item>>, bool>;

  template <typename F, typename Item>
  using enable_if_is_value_formatter =
      std::enable_if_t<is_value_formatter_v<F, std::decay_t<Item>>, bool>;

  template <typename F, typename Item>
  using enable_if_is_click_handler =
      std::enable_if_t<std::is_nothrow_invocable_r_v<sgl::error, F, Item&>, bool>;

  template <typename F, typename Item>
  using enable_if_is_tick_handler = std::enable_if_t<is_tick_handler_for_v<F, Item>, bool>;

  /// \addtogroup item_traits Item Traits
  /// \ingroup sgl_traits
  /// \{
  /// \cond

  template <typename T, typename = void>
  struct has_text : std::false_type {};

  template <typename T>
  struct has_text<
      T,
      std::void_t<decltype(std::declval<T>().text()), decltype(std::declval<const T>().text())>> {

    using R = std::remove_reference_t<decltype(std::declval<T>().text())>;
    using CR = std::remove_reference_t<decltype(std::declval<const T>().text())>;
    using String = sgl::static_string<typename T::char_type, T::text_size>;

    static constexpr bool value = std::is_same_v<R, String> and std::is_same_v<CR, const String>;
  };

  template <typename T>
  inline constexpr bool has_text_v = has_text<T>::value;

  template <typename T, typename = void>
  struct has_handle_input : std::false_type {};

  template <typename T>
  struct has_handle_input<
      T,
      std::void_t<decltype(std::declval<T>().handle_input(std::declval<sgl::Input>()))>> {
    static constexpr bool value = std::is_same_v<
        sgl::error,
        decltype(std::declval<T>().handle_input(
            std::declval<sgl::Input>()))>&& noexcept(std::declval<T>()
                                                         .handle_input(std::declval<sgl::Input>()));
  };

  template <typename T>
  inline constexpr bool has_handle_input_v = has_handle_input<T>::value;

  template <typename T, typename = void>
  struct has_set_text : std::false_type {};

  template <typename T>
  struct has_set_text<
      T,
      std::void_t<decltype(std::declval<T>().set_text(std::declval<typename T::StringView>()))>> {
    static constexpr bool value = std::is_same_v<sgl::error,
                                                 decltype(std::declval<T>().set_text(
                                                     std::declval<typename T::StringView>()))>;
  };

  template <typename T, typename = void>
  struct has_tick : std::false_type {};

  template <typename T>
  struct has_tick<T, decltype(std::declval<T>().tick())> {
    static constexpr bool value = noexcept(std::declval<T>().tick());
  };

  template <typename T>
  inline constexpr bool has_set_text_v = has_set_text<T>::value;

  template <typename T>
  inline constexpr bool has_tick_v = has_tick<T>::value;

  /// \endcond

  /// \}

  /// \addtogroup page_traits Page traits
  /// \ingroup sgl_traits
  /// \{
  /// \cond
  namespace detail {
    [[maybe_unused]] auto pf = [](auto&) {};
    [[maybe_unused]] auto pcf = [](const auto&) {};
  } // namespace detail

  template <typename T, typename = void>
  struct has_title : std::false_type {};

  template <typename T>
  struct has_title<T, std::void_t<decltype(std::declval<T>().title())>> {
    static constexpr bool value =
        std::is_same_v<typename T::StringView, decltype(std::declval<T>().title())>;
  };

  template <typename T>
  inline constexpr bool has_title_v = has_title<T>::value;

  template <typename T, typename = void>
  struct has_index : std::false_type {};

  template <typename T>
  struct has_index<T, std::void_t<decltype(std::declval<T>().index())>> {
    static constexpr bool value = std::is_same_v<size_t, decltype(std::declval<T>().index())>;
  };

  template <typename T>
  inline constexpr bool has_index_v = has_index<T>::value;

  template <typename T, typename = void>
  struct has_size : std::false_type {};

  template <typename T>
  struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> {
    static constexpr bool value = std::is_same_v<size_t, decltype(std::declval<T>().size())>;
  };

  template <typename T>
  inline constexpr bool has_size_v = has_size<T>::value;

  template <typename T, typename = void>
  struct has_get_item : std::false_type {};

  template <typename T>
  struct has_get_item<
      T,
      std::void_t<decltype(std::declval<T>().template get_item<size_t{0}>()),
                  decltype(std::declval<const T>().template get_item<size_t{0}>())>> {
    using R1 = decltype(std::declval<T>().template get_item<size_t{0}>());
    using R2 = decltype(std::declval<const T>().template get_item<size_t{0}>());
    static constexpr bool value = std::is_lvalue_reference_v<R1> and
                                  std::is_lvalue_reference_v<R2> and
                                  std::is_const_v<std::remove_reference_t<R2>>;
  };

  template <typename T>
  inline constexpr bool has_get_item_v = has_get_item<T>::value;

  template <typename T, typename = void>
  struct has_set_item_cursor : std::false_type {};

  template <typename T>
  struct has_set_item_cursor<T, std::void_t<decltype(std::declval<T>().set_item_cursor(size_t{0}))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_set_item_cursor_v = has_set_item_cursor<T>::value;

  template <typename T, typename = void>
  struct has_is_in_edit_mode : std::false_type {};

  template <typename T>
  struct has_is_in_edit_mode<T, std::void_t<decltype(std::declval<T>().is_in_edit_mode())>> {
    static constexpr bool value =
        std::is_same_v<bool, decltype(std::declval<T>().is_in_edit_mode())>;
  };

  template <typename T>
  inline constexpr bool has_is_in_edit_mode_v = has_is_in_edit_mode<T>::value;

  template <typename T, typename = void>
  struct has_set_edit_mode : std::false_type {};

  template <typename T>
  struct has_set_edit_mode<T, std::void_t<decltype(std::declval<T>().set_edit_mode())>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_set_edit_mode_v = has_set_edit_mode<T>::value;

  template <typename T, typename = void>
  struct has_set_navigation_mode : std::false_type {};

  template <typename T>
  struct has_set_navigation_mode<T, std::void_t<decltype(std::declval<T>().set_navigation_mode())>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_set_navigation_mode_v = has_set_navigation_mode<T>::value;

  template <typename T, typename = void>
  struct has_get_start_edit : std::false_type {};

  template <typename T>
  struct has_get_start_edit<T, std::void_t<decltype(std::declval<T>().get_start_edit())>> {
    static constexpr bool value =
        std::is_same_v<sgl::Input, decltype(std::declval<T>().get_start_edit())>;
  };

  template <typename T>
  inline constexpr bool has_get_start_edit_v = has_get_start_edit<T>::value;

  template <typename T, typename = void>
  struct has_set_start_edit : std::false_type {};

  template <typename T>
  struct has_set_start_edit<
      T,
      std::void_t<decltype(std::declval<T>().set_start_edit(std::declval<sgl::Input>()))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_set_start_edit_v = has_set_start_edit<T>::value;

  template <typename T, typename = void>
  struct has_get_stop_edit : std::false_type {};

  template <typename T>
  struct has_get_stop_edit<T, std::void_t<decltype(std::declval<T>().get_stop_edit())>> {
    static constexpr bool value =
        std::is_same_v<sgl::Input, decltype(std::declval<T>().get_stop_edit())>;
  };

  template <typename T>
  inline constexpr bool has_get_stop_edit_v = has_get_stop_edit<T>::value;

  template <typename T, typename = void>
  struct has_set_stop_edit : std::false_type {};

  template <typename T>
  struct has_set_stop_edit<
      T,
      std::void_t<decltype(std::declval<T>().set_stop_edit(std::declval<sgl::Input>()))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_set_stop_edit_v = has_set_stop_edit<T>::value;

  template <typename T, typename = void>
  struct has_set_menu : std::false_type {};

  template <typename T>
  struct has_set_menu<T, std::void_t<decltype(std::declval<T>().set_menu(std::declval<void*>()))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_set_menu_v = has_set_menu<T>::value;

  template <typename T, typename = void>
  struct has_for_each_item : std::false_type {};

  template <typename T>
  struct has_for_each_item<
      T,
      std::void_t<decltype(std::declval<T>().template for_each_item(detail::pf)),
                  decltype(std::declval<const T>().template for_each_item(detail::pcf))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_for_each_item_v = has_for_each_item<T>::value;

  template <typename T, typename = void>
  struct has_for_current_item : std::false_type {};

  template <typename T>
  struct has_for_current_item<
      T,
      std::void_t<decltype(std::declval<T>().for_current_item(detail::pf)),
                  decltype(std::declval<const T>().for_current_item(detail::pcf))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_for_current_item_v = has_for_current_item<T>::value;

  template <typename T, typename = void>
  struct has_on_enter : std::false_type {};

  template <typename T>
  struct has_on_enter<T, std::void_t<decltype(std::declval<T>().on_enter())>> {
    static constexpr bool value =
        std::is_same_v<sgl::error, decltype(std::declval<T>().on_enter())>;
  };

  template <typename T>
  inline constexpr bool has_on_enter_v = has_on_enter<T>::value;

  template <typename T, typename = void>
  struct has_on_exit : std::false_type {};

  template <typename T>
  struct has_on_exit<T, std::void_t<decltype(std::declval<T>().on_exit())>> {
    static constexpr bool value = std::is_same_v<sgl::error, decltype(std::declval<T>().on_exit())>;
  };

  template <typename T>
  inline constexpr bool has_on_exit_v = has_on_exit<T>::value;

  template <typename T>
  struct is_page {
    static constexpr bool value =
        has_title_v<T> and has_size_v<T> and has_index_v<T> and has_handle_input_v<T> and
        has_get_item_v<T> and has_set_item_cursor_v<T> and has_is_in_edit_mode_v<T> and
        has_set_edit_mode_v<T> and has_set_navigation_mode_v<T> and has_get_start_edit_v<T> and
        has_set_start_edit_v<T> and has_get_stop_edit_v<T> and has_set_stop_edit_v<T> and
        has_set_menu_v<T> and has_for_each_item_v<T> and has_for_current_item_v<T> and
        has_on_enter_v<T> and has_on_exit_v<T>;
  };

  /// \endcond

  /// @brief true if T fulfills the page concept
  /// \tparam T type to check
  template <typename T>
  inline constexpr bool is_page_v = is_page<T>::value;

  /// \}

  /// \addtogroup menu_traits Menu Traits
  /// \ingroup sgl_traits
  /// \{

  /// \cond
  template <typename T, typename = void>
  struct has_set_active_page : std::false_type {};

  template <typename T>
  struct has_set_active_page<T,
                             std::void_t<decltype(std::declval<T>().set_active_page(size_t{0})),
                                         decltype(std::declval<T>().set_active_page(
                                             std::declval<typename T::StringView>()))>> {
    static constexpr bool value =
        std::is_same_v<sgl::error, decltype(std::declval<T>().set_active_page(size_t{0}))> and
        std::is_same_v<sgl::error,
                       decltype(std::declval<T>().set_active_page(
                           std::declval<typename T::StringView>()))>;
  };

  template <typename T>
  inline constexpr bool has_set_active_page_v = has_set_active_page<T>::value;

  template <typename T, typename = void>
  struct has_for_each_page : std::false_type {};

  template <typename T>
  struct has_for_each_page<
      T,
      std::void_t<decltype(std::declval<T>().template for_each_page(detail::pf)),
                  decltype(std::declval<const T>().template for_each_page(detail::pcf))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_for_each_page_v = has_for_each_page<T>::value;

  template <typename T, typename = void>
  struct has_for_current_page : std::false_type {};

  template <typename T>
  struct has_for_current_page<
      T,
      std::void_t<decltype(std::declval<T>().for_current_page(detail::pf)),
                  decltype(std::declval<const T>().for_current_page(detail::pcf))>>
      : std::true_type {};

  template <typename T>
  inline constexpr bool has_for_current_page_v = has_for_current_page<T>::value;

  template <typename T, typename = void>
  struct has_get_page : std::false_type {};

  template <typename T>
  struct has_get_page<
      T,
      std::void_t<decltype(std::declval<T>().template get_page<size_t{0}>()),
                  decltype(std::declval<const T>().template get_page<size_t{0}>())>> {
    using R1 = decltype(std::declval<T>().template get_page<size_t{0}>());
    using R2 = decltype(std::declval<const T>().template get_page<size_t{0}>());
    static constexpr bool value = std::is_lvalue_reference_v<R1> and
                                  std::is_lvalue_reference_v<R2> and
                                  std::is_const_v<std::remove_reference_t<R2>>;
  };

  template <typename T>
  inline constexpr bool has_get_page_v = has_get_page<T>::value;

  template <typename T, typename = void>
  struct has_get_item_at_page : std::false_type {};

  template <typename T>
  struct has_get_item_at_page<
      T,
      std::void_t<decltype(std::declval<T>().template get_item<size_t{0}, size_t{0}>()),
                  decltype(std::declval<const T>().template get_item<size_t{0}, size_t{0}>())>> {
    using R1 = decltype(std::declval<T>().template get_item<size_t{0}, size_t{0}>());
    using R2 = decltype(std::declval<const T>().template get_item<size_t{0}, size_t{0}>());
    static constexpr bool value = std::is_lvalue_reference_v<R1> and
                                  std::is_lvalue_reference_v<R2> and
                                  std::is_const_v<std::remove_reference_t<R2>>;
  };

  template <typename T>
  inline constexpr bool has_get_item_at_page_v = has_get_item_at_page<T>::value;

  template <typename T>
  struct is_menu {
    static constexpr bool value = has_for_each_page_v<T> and has_for_current_page_v<T> and
                                  has_get_page_v<T> and has_get_item_at_page_v<T> and
                                  has_index_v<T> and has_size_v<T>;
  };

  /// \endcond

  /// @brief  true if T fulfills the menu concept
  /// @tparam T type to check
  template <typename T>
  inline constexpr bool is_menu_v = is_menu<T>::value;

  /// \}

  template <typename T>
  struct is_item {
    static constexpr bool value =
        has_text_v<T> and has_set_menu_v<T> and has_handle_input_v<T> and has_tick_v<T>;
  };

  /// @brief  check if T fulfills the item concept.
  /// @tparam T type to check
  template <typename T>
  inline constexpr bool is_item_v = is_item<T>::value;

} // namespace sgl
#endif /* SGL_ITEM_CONCEPTS_HPP */
