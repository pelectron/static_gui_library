/**
 * \file sgl/numeric.hpp
 * \author Pelé Constam (you@domain.com)
 * \brief This file contains the sgl::Numeric class and the various sgl::make_numeric() overloads.
 * \version 0.1
 * \date 2022-09-23
 *
 *          Copyright Pele Constam 2022.
* Distributed under the Boost Software License, Version 1.0.
*    (See accompanying file LICENSE_1_0.txt or copy at
*          https://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef SGL_NUMERIC_HPP
#define SGL_NUMERIC_HPP
#include "sgl/cx_arg.hpp"
#include "sgl/format.hpp"
#include "sgl/item_base.hpp"
#include "sgl/item_concepts.hpp"

namespace sgl {

  /// \ingroup item_types
  /// \headerfile numeric.hpp "sgl/numeric.hpp"
  /// \brief This class models a numeric item. It consists of a value of type T,
  /// a delta value of type T and a formatter to format T's into strings.
  /// \details The default behaviour of this item when it is edited is as follows: an input of
  /// sgl::Input::up/right increases the item's value by it's delta value, i.e. value += delta. An
  /// input equal to sgl::Input::down/left decreases the item's value by it's delta value, i.e.
  /// value -= value.
  /// \tparam T value type
  /// \tparam TextSize display width
  /// \tparam CharT character type
  template <size_t TextSize, typename CharT, typename T>
  class Numeric : public sgl::ItemBase<Numeric<TextSize, CharT, T>> {
  public:
    static_assert(!std::is_same_v<bool, T>,
                  "T must not be bool. Use sgl::boolean for a boolean item.");
    /// base class of Numeric
    using Base = sgl::ItemBase<Numeric<TextSize, CharT, T>>;
    /// string_view type of this item
    using StringView = typename Base::StringView;
    /// string type of this item
    using String = typename Base::String;
    /// value type of this item
    using value_type = T;
    /// concrete formatter type
    using Formatter_t = Callable<sgl::format_result(CharT*, size_t, T, uint32_t, sgl::format)>;

    /// \brief Construct a numeric item with default formatter and default
    /// input handling. This constructor is constexpr if T is integral.
    /// \param initial_value initial value
    /// \param delta delta value
    constexpr Numeric(T initial_value, T delta) noexcept;

    /// \brief Construct a numeric item with default formatter and default
    /// input handling. This constructor is constexpr if T is integral.
    /// \param initial_value initial value
    /// \param delta delta value
    /// \param initial_text text of the numeric item
    constexpr Numeric(T initial_value, T delta, StringView initial_text) noexcept;

    /// \brief create a numeric item at compile time for floats and doubles.
    /// \details This is only needed for compile time floating point numeric items. Use the overload
    /// with a normal T for integral types.
    ///
    /// \tparam CxSize cx_arg string size.
    /// \param initial_value initial value as cx_arg
    /// \param delta delta value
    constexpr Numeric(const cx_arg<T, TextSize>& initial_value, T delta) noexcept;

    /// \brief Construct a numeric item with custom formatter, but default
    /// input handling.
    ///
    /// \tparam Formatter formatter type, see formatter_check for more details.
    /// \param initial_value inital value
    /// \param delta delta value
    /// \param formatter formatter instance
    template <typename Formatter,
              enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>> = true>
    constexpr Numeric(T initial_value, T delta, Formatter&& formatter) noexcept;

    /// \brief Construct a numeric item with custom formatter and input
    /// handler.
    ///
    /// \tparam Formatter formatter type, see formatter_check for more details.
    /// \tparam InputHandler Input handler type, see input_handler_for_v
    /// for more details.
    /// \param initial_value initial value
    /// \param delta delta value
    /// \param formatter formatter instance
    /// \param handler handler instance
    template <typename Formatter,
              typename InputHandler,
              enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
              enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>> = true>
    constexpr Numeric(T              initial_value,
                      T              delta,
                      Formatter&&    formatter,
                      InputHandler&& handler) noexcept;

    /// get the delta value.
    /// \return T
    constexpr T get_delta() const noexcept;

    /// set the delta value.
    /// \param delta delta value to set
    /// \return sgl::error
    constexpr sgl::error set_delta(T delta) noexcept;

    /// get stored value
    /// \return T
    constexpr T get_value() const noexcept;

    /// set value
    /// \param value value to set
    /// \return sgl::error
    constexpr sgl::error set_value(T value) noexcept;

    /// @brief set the formatting precision.
    /// @param precision number of fractional digits
    constexpr void set_precision(uint32_t precision) noexcept;

    /// @brief set the formatting type.
    /// @param format format to use
    constexpr void set_format(sgl::format format) noexcept;

  private:
    /// \brief execute the formatter and format val into this item's text
    /// \param val value to format
    /// \return sgl::error
    constexpr sgl::error        format(T val) noexcept;
    constexpr static sgl::error default_handle_input(Numeric<TextSize, CharT, T>& numeric_item,
                                                     sgl::Input                   input) noexcept;

    constexpr static sgl::format_result default_format(CharT*      str,
                                                       size_t      len,
                                                       T           value,
                                                       uint32_t    precision,
                                                       sgl::format format) noexcept;

    Formatter_t format_{&default_format};         ///< formatter
    T           value_{0};                        ///< value
    T           delta_{1};                        ///< delta value
    uint32_t    precision_{6};                    ///< formatting precision
    sgl::format format_type_{sgl::format::fixed}; ///< formatting type
  };

  /// \ingroup item_factories
  /// \addtogroup numeric_factories
  /// \brief Construct a numeric item with default formatter and default
  /// input handling. This function is constexpr if T is integral.
  /// \tparam TextSize text size
  /// \tparam CharT character type
  /// \tparam T value type
  /// \param initial_value value of item
  /// \param delta delta value of item
  /// \return Numeric<TextSize, CharT, T>
  /// \{
  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T> make_numeric(T initial_value, T delta) noexcept;

  /// \brief Construct a numeric item with default formatter and default
  /// input handling, but custom initial text.
  /// \tparam TextSize text size
  /// \tparam CharT character type
  /// \tparam T value type
  /// \param initial_value value of item
  /// \param delta delta value of item
  /// \param initial_text text of the item
  /// \return constexpr Numeric<TextSize, CharT, T>
  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>
      make_numeric(T initial_value, T delta, sgl::string_view<CharT> initial_text) noexcept;

  /// \brief Construct a numeric item with default formatter and default
  /// input handling, but custom initial text.
  /// \tparam TextSize text size
  /// \tparam CharT character type
  /// \tparam T value type
  /// \param initial_value value of item
  /// \param delta delta value of item
  /// \param initial_text text of the item
  /// \return constexpr Numeric<N-1, CharT, T>
  template <size_t N, typename CharT, typename T>
  constexpr Numeric<N - 1, CharT, T>
      make_numeric(T initial_value, T delta, const CharT (&initial_text)[N]) noexcept;

  /// \brief create a numeric item at compile time for floats and doubles with default formatter and
  /// default input handling.
  /// \details This is only needed for compile time floating point numeric
  /// items. Use the overload with a normal T parameter for integral types.
  ///
  /// \tparam CxSize cx_arg string size.
  /// \param initial_value initial value as cx_arg
  /// \param delta delta value
  /// \return constexpr Numeric<TextSize, char, T>
  template <size_t CxSize, typename T, size_t TextSize = CxSize>
  constexpr Numeric<TextSize, char, T> make_numeric(const cx_arg<T, CxSize>& initial_value,
                                                    T                        delta) noexcept;

  /// \brief Construct a numeric item with custom formatter, but default
  /// input handling.
  /// \tparam TextSize text size
  /// \tparam CharT character type
  /// \tparam T value type
  /// \tparam Formatter formatter type, see formatter_check for more details.
  /// \param initial_value inital value
  /// \param delta delta value
  /// \param formatter formatter instance
  /// \return Numeric<TextSize, CharT, T>
  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T>
      make_numeric(T initial_value, T delta, Formatter&& formatter) noexcept;

  /// \brief Construct a numeric item with custom formatter and input
  /// handler.
  /// \tparam TextSize text size
  /// \tparam CharT character type
  /// \tparam T value type
  /// \tparam Formatter formatter type, see enable_if_is_value_formatter for more details.
  /// \tparam InputHandler input handler type. See enable_if_is_input_handler for more details.
  /// \param initial_value inital value
  /// \param delta delta value
  /// \param formatter formatter instance
  /// \param handler input handler instance
  /// \return Numeric<TextSize, CharT, T>
  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
            enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T> make_numeric(T              initial_value,
                                                     T              delta,
                                                     Formatter&&    formatter,
                                                     InputHandler&& handler) noexcept;
  /// \}
} // namespace sgl

#include "sgl/impl/numeric_impl.hpp"
#endif /* SGL_NUMERIC_HPP */
