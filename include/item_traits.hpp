#ifndef SGL_ITEM_TRAITS_HPP
#define SGL_ITEM_TRAITS_HPP

#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_traits.hpp"

namespace sgl {

  template <typename F, typename Item>
  static constexpr bool is_input_handler_for_v =
      is_nothrow_invocable_r_v<sgl::error, F, Item&, sgl::Input>;
  template <typename F, typename Item>
  static constexpr bool is_click_handler_for_v = is_nothrow_invocable_r_v<sgl::error, F, Item&>;
  template <typename F, typename Item>
  static constexpr bool is_value_formatter_v =
      is_nothrow_invocable_r_v<sgl::error, F, typename Item::String&, typename Item::value_type>;
  template <typename F, typename Item>
  using enable_if_is_input_handler = enable_if_t<is_input_handler_for_v<F, Item>, bool>;
  template <typename F, typename Item>
  using enable_if_is_value_formatter = enable_if_t<is_value_formatter_v<F, Item>, bool>;
  template <typename F, typename Item>
  using enable_if_is_click_handler =
      enable_if_t<is_nothrow_invocable_r_v<sgl::error, F, Item&>, bool>;

  /// item method traits
  /// @{
  template <typename T, typename = void>
  struct has_text : false_type {};

  template <typename T>
  struct has_text<T, void_t<decltype(declval<T>().text())>> {
    static constexpr bool value = is_same_v<typename T::StringView, decltype(declval<T>().text())>;
  };

  template <typename T>
  static constexpr bool has_text_v = has_text<T>::value;

  template <typename T, typename = void>
  struct has_handle_input : false_type {};

  template <typename T>
  struct has_handle_input<T, void_t<decltype(declval<T>().handle_input(declval<sgl::Input>()))>> {
    static constexpr bool value =
        is_same_v<sgl::error, decltype(declval<T>().handle_input(declval<sgl::Input>()))>;
  };

  template <typename T>
  static constexpr bool has_handle_input_v = has_handle_input<T>::value;

  template <typename T, typename = void>
  struct has_set_text : false_type {};

  template <typename T>
  struct has_set_text<T,
                      void_t<decltype(declval<T>().set_text(declval<typename T::StringView>()))>> {
    static constexpr bool value =
        is_same_v<sgl::error, decltype(declval<T>().set_text(declval<typename T::StringView>()))>;
  };

  template <typename T>
  static constexpr bool has_set_text_v = has_set_text<T>::value;

  template <typename T, typename = void>
  struct has_name : false_type {};

  template <typename T>
  struct has_name<T, void_t<decltype(declval<T>().name())>> {
    static constexpr bool value = is_same_v<typename T::StringView, decltype(declval<T>().name())>;
  };
  template <typename T>
  static constexpr bool has_name_v = has_name<T>::value;

  template <typename T>
  struct is_item {
    static constexpr bool value =
        has_text_v<T> and has_handle_input_v<T> and has_set_text_v<T> and has_name_v<T>;
  };

  template <typename T>
  static constexpr bool is_item_v = is_item<T>::value;
  /// @}

  /// page traits
  /// @{
  namespace detail {
    [[maybe_unused]] auto pf = [](auto& item) {};
    [[maybe_unused]] auto pcf = [](const auto& item) {};
    [[maybe_unused]] auto set_menu = [](auto* m) {};
    // dummy menu type
    struct M {};
  } // namespace detail

  template <typename T, typename = void>
  struct has_title : false_type {};

  template <typename T>
  struct has_title<T, void_t<decltype(declval<T>().title())>> {
    static constexpr bool value = is_same_v<typename T::StringView, decltype(declval<T>().title())>;
  };

  template <typename T>
  static constexpr bool has_title_v = has_title<T>::value;

  template <typename T, typename = void>
  struct has_index : false_type {};

  template <typename T>
  struct has_index<T, void_t<decltype(declval<T>().index())>> {
    static constexpr bool value = is_same_v<size_t, decltype(declval<T>().index())>;
  };

  template <typename T>
  static constexpr bool has_index_v = has_index<T>::value;

  template <typename T, typename = void>
  struct has_size : false_type {};

  template <typename T>
  struct has_size<T, void_t<decltype(declval<T>().size())>> {
    static constexpr bool value = is_same_v<size_t, decltype(declval<T>().size())>;
  };

  template <typename T>
  static constexpr bool has_size_v = has_size<T>::value;

  template <typename T, typename = void>
  struct has_get_item : false_type {};

  template <typename T>
  struct has_get_item<T,
                      void_t<decltype(declval<T>().template get_item<size_t{0}>()),
                             decltype(declval<const T>().template get_item<size_t{0}>())>> {
    using R1 = decltype(declval<T>().template get_item<size_t{0}>());
    using R2 = decltype(declval<const T>().template get_item<size_t{0}>());
    static constexpr bool value = is_lvalue_reference_v<R1> and is_lvalue_reference_v<R2> and
                                  is_const_v<remove_reference_t<R2>>;
  };

  template <typename T>
  static constexpr bool has_get_item_v = has_get_item<T>::value;

  template <typename T, typename = void>
  struct has_set_item_cursor : false_type {};

  template <typename T>
  struct has_set_item_cursor<T, void_t<decltype(declval<T>().set_item_cursor(size_t{0}))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_set_item_cursor_v = has_set_item_cursor<T>::value;

  template <typename T, typename = void>
  struct has_is_in_edit_mode : false_type {};

  template <typename T>
  struct has_is_in_edit_mode<T, void_t<decltype(declval<T>().is_in_edit_mode())>> {
    static constexpr bool value = is_same_v<bool, decltype(declval<T>().is_in_edit_mode())>;
  };

  template <typename T>
  static constexpr bool has_is_in_edit_mode_v = has_is_in_edit_mode<T>::value;

  template <typename T, typename = void>
  struct has_set_edit_mode : false_type {};

  template <typename T>
  struct has_set_edit_mode<T, void_t<decltype(declval<T>().set_edit_mode())>> : true_type {};

  template <typename T>
  static constexpr bool has_set_edit_mode_v = has_set_edit_mode<T>::value;

  template <typename T, typename = void>
  struct has_set_navigation_mode : false_type {};

  template <typename T>
  struct has_set_navigation_mode<T, void_t<decltype(declval<T>().set_navigation_mode())>>
      : true_type {};

  template <typename T>
  static constexpr bool has_set_navigation_mode_v = has_set_navigation_mode<T>::value;

  template <typename T, typename = void>
  struct has_get_start_edit : false_type {};

  template <typename T>
  struct has_get_start_edit<T, void_t<decltype(declval<T>().get_start_edit())>> {
    static constexpr bool value = is_same_v<sgl::Input, decltype(declval<T>().get_start_edit())>;
  };

  template <typename T>
  static constexpr bool has_get_start_edit_v = has_get_start_edit<T>::value;

  template <typename T, typename = void>
  struct has_set_start_edit : false_type {};

  template <typename T>
  struct has_set_start_edit<T, void_t<decltype(declval<T>().set_start_edit(declval<sgl::Input>()))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_set_start_edit_v = has_set_start_edit<T>::value;

  template <typename T, typename = void>
  struct has_get_stop_edit : false_type {};

  template <typename T>
  struct has_get_stop_edit<T, void_t<decltype(declval<T>().get_stop_edit())>> {
    static constexpr bool value = is_same_v<sgl::Input, decltype(declval<T>().get_stop_edit())>;
  };

  template <typename T>
  static constexpr bool has_get_stop_edit_v = has_get_stop_edit<T>::value;

  template <typename T, typename = void>
  struct has_set_stop_edit : false_type {};

  template <typename T>
  struct has_set_stop_edit<T, void_t<decltype(declval<T>().set_stop_edit(declval<sgl::Input>()))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_set_stop_edit_v = has_set_stop_edit<T>::value;

  template <typename T, typename = void>
  struct has_set_menu : false_type {};

  template <typename T>
  struct has_set_menu<
      T,
      void_t<decltype(declval<T>().template set_menu<detail::M>(declval<detail::M*>()))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_set_menu_v = has_set_menu<T>::value;

  template <typename T, typename = void>
  struct has_for_each_item : false_type {};

  template <typename T>
  struct has_for_each_item<T,
                           void_t<decltype(declval<T>().template for_each_item(detail::pf)),
                                  decltype(declval<const T>().template for_each_item(detail::pcf))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_for_each_item_v = has_for_each_item<T>::value;

  template <typename T, typename = void>
  struct has_for_current_item : false_type {};

  template <typename T>
  struct has_for_current_item<T,
                              void_t<decltype(declval<T>().for_current_item(detail::pf)),
                                     decltype(declval<const T>().for_current_item(detail::pcf))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_for_current_item_v = has_for_current_item<T>::value;

  template <typename T, typename = void>
  struct has_on_enter : false_type {};

  template <typename T>
  struct has_on_enter<T, void_t<decltype(declval<T>().on_enter())>> {
    static constexpr bool value = is_same_v<sgl::error, decltype(declval<T>().on_enter())>;
  };

  template <typename T>
  static constexpr bool has_on_enter_v = has_on_enter<T>::value;

  template <typename T, typename = void>
  struct has_on_exit : false_type {};

  template <typename T>
  struct has_on_exit<T, void_t<decltype(declval<T>().on_exit())>> {
    static constexpr bool value = is_same_v<sgl::error, decltype(declval<T>().on_exit())>;
  };

  template <typename T>
  static constexpr bool has_on_exit_v = has_on_exit<T>::value;

  template <typename T>
  struct is_page {
    static constexpr bool value =
        has_name_v<T> and has_title_v<T> and has_size_v<T> and has_index_v<T> and
        has_handle_input_v<T> and has_get_item_v<T> and has_set_item_cursor_v<T> and
        has_is_in_edit_mode_v<T> and has_set_edit_mode_v<T> and has_set_navigation_mode_v<T> and
        has_get_start_edit_v<T> and has_set_start_edit_v<T> and has_get_stop_edit_v<T> and
        has_set_stop_edit_v<T> and has_set_menu_v<T> and has_for_each_item_v<T> and
        has_for_current_item_v<T> and has_on_enter_v<T> and has_on_exit_v<T>;
  };

  template <typename T>
  static constexpr bool is_page_v = is_page<T>::value;
  /// @}

  /// menu traits
  /// @{

  template <typename T, typename = void>
  struct has_set_active_page : false_type {};

  template <typename T>
  struct has_set_active_page<
      T,
      void_t<decltype(declval<T>().set_active_page(size_t{0})),
             decltype(declval<T>().set_active_page(declval<typename T::StringView>()))>> {
    static constexpr bool value =
        is_same_v<sgl::error, decltype(declval<T>().set_active_page(size_t{0}))> and
        is_same_v<sgl::error,
                  decltype(declval<T>().set_active_page(declval<typename T::StringView>()))>;
  };

  template <typename T>
  static constexpr bool has_set_active_page_v = has_set_active_page<T>::value;

  template <typename T, typename = void>
  struct has_for_each_page : false_type {};

  template <typename T>
  struct has_for_each_page<T,
                           void_t<decltype(declval<T>().template for_each_page(detail::pf)),
                                  decltype(declval<const T>().template for_each_page(detail::pcf))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_for_each_page_v = has_for_each_page<T>::value;

  template <typename T, typename = void>
  struct has_for_current_page : false_type {};

  template <typename T>
  struct has_for_current_page<T,
                              void_t<decltype(declval<T>().for_current_page(detail::pf)),
                                     decltype(declval<const T>().for_current_page(detail::pcf))>>
      : true_type {};

  template <typename T>
  static constexpr bool has_for_current_page_v = has_for_current_page<T>::value;

  template <typename T, typename = void>
  struct has_get_page : false_type {};

  template <typename T>
  struct has_get_page<T,
                      void_t<decltype(declval<T>().template get_page<size_t{0}>()),
                             decltype(declval<const T>().template get_page<size_t{0}>())>> {
    using R1 = decltype(declval<T>().template get_page<size_t{0}>());
    using R2 = decltype(declval<const T>().template get_page<size_t{0}>());
    static constexpr bool value = is_lvalue_reference_v<R1> and is_lvalue_reference_v<R2> and
                                  is_const_v<remove_reference_t<R2>>;
  };

  template <typename T>
  static constexpr bool has_get_page_v = has_get_page<T>::value;

  template <typename T, typename = void>
  struct has_get_item_at_page : false_type {};

  template <typename T>
  struct has_get_item_at_page<
      T,
      void_t<decltype(declval<T>().template get_item<size_t{0}, size_t{0}>()),
             decltype(declval<const T>().template get_item<size_t{0}, size_t{0}>())>> {
    using R1 = decltype(declval<T>().template get_item<size_t{0}, size_t{0}>());
    using R2 = decltype(declval<const T>().template get_item<size_t{0}, size_t{0}>());
    static constexpr bool value = is_lvalue_reference_v<R1> and is_lvalue_reference_v<R2> and
                                  is_const_v<remove_reference_t<R2>>;
  };

  template <typename T>
  static constexpr bool has_get_item_at_page_v = has_get_item_at_page<T>::value;

  template <typename T>
  struct is_menu {
    static constexpr bool value = has_set_active_page_v<T> and has_for_each_page_v<T> and
                                  has_for_current_page_v<T> and has_get_page_v<T> and
                                  has_get_item_at_page_v<T> and has_index_v<T> and has_size_v<T>;
  };

  template <typename T>
  static constexpr bool is_menu_v = is_menu<T>::value;

  /// @}
} // namespace sgl
#endif