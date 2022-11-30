//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl/item_base.hpp"

namespace sgl {
  /// @headerfile boolean.hpp "sgl/boolean.hpp"
  /// @ingroup item_types

  /**
    This class implements a basic Boolean item. When clicked, it will
    toggle its value and set its text accordingly.

    A Boolean Item consists of the following data:
    - boolean value: the value of this item
    - 'true' string_view: the text to show when value == true.
    - 'false' string_view: the text to show when value == false.

    @tparam TextSize number of characters per line in the menu
    @tparam CharT character type of the item
   */
  template <size_t TextSize, typename CharT>
  class Boolean : public sgl::ItemBase<Boolean<TextSize, CharT>> {
  public:
    /// string_view type used by this item
    using StringView = typename sgl::ItemBase<Boolean<TextSize, CharT>>::StringView;

    using char_type = typename sgl::ItemBase<Boolean<TextSize, CharT>>::char_type;

    /// Construct a Boolean item, which toggles its text between "TRUE"
    /// and "FALSE" when clicked.
    /// @param initial_value initial value of the item
    explicit constexpr Boolean(bool initial_value) noexcept;

    /**
      Construct a Boolean item with custom true and false strings.
      @param value inital value of item
      @param true_text text to set when value is true
      @param false_text text to set when value is false
     */
    constexpr Boolean(bool                    value,
                      sgl::string_view<CharT> true_text,
                      sgl::string_view<CharT> false_text) noexcept;

    /// get value of item
    /// @return bool
    [[nodiscard]] constexpr bool get_value() const noexcept;

    /// set the value of the item. Also set's the text accordingly
    /// @param value value to set
    /// @return  sgl::error
    [[nodiscard]] constexpr sgl::error set_value(bool value) noexcept;

    /// get string_view to the true string
    /// @return sgl::string_view<CharT>
    [[nodiscard]] constexpr sgl::string_view<CharT> true_string() const noexcept;

    /// get string_view to false string
    /// @return sgl::string_view<CharT>
    [[nodiscard]] constexpr sgl::string_view<CharT> false_string() const noexcept;

  private:
    /// default input handler
    /// @return sgl::error
    [[nodiscard]] static constexpr sgl::error default_handle_input(Boolean<TextSize, CharT>& item,
                                                                   sgl::input i) noexcept;

    sgl::string_view<CharT> true_string_{};  ///< view of true string
    sgl::string_view<CharT> false_string_{}; ///< view of false string
    bool                    value_{false};   ///< value of item
  };

  /// @ingroup item_factories
  /// @addtogroup boolean_factories
  /// @{

  /// create a boolean item with default true and false string
  /// @param initial_value initial value of boolean
  /// @return sgl::Boolean
  constexpr auto boolean(bool initial_value);

  /**
    Create a boolean item with custom true and false string.
    @tparam CharT character type
    @tparam NTrue true string character array size
    @tparam NFalse false string character array size
    @param initial_value initial value of boolean
    @param true_string string to show when item's value is true
    @param false_string string to show when item's value is false
    @return sgl::Boolean
  */
  template <typename CharT, size_t NTrue, size_t NFalse>
  constexpr auto boolean(bool initial_value,
                         const CharT (&true_string)[NTrue],
                         const CharT (&false_string)[NFalse]);
  /// @}
} // namespace sgl

#include "sgl/impl/boolean_impl.hpp"
#endif /* SGL_BOOLEAN_HPP */
