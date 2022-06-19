#ifndef SGL_BASIC_NUMERIC_HPP
#define SGL_BASIC_NUMERIC_HPP
#include "sgl_cx_arg.hpp"
#include "sgl_format.hpp"
#include "sgl_item_base.hpp"
#include "sgl_item_concepts.hpp"


namespace sgl {
  /**
   * \brief This class models a numeric item. It consists of a value of type T,
   * a delta value of type T and a formatter to format T's into strings.
   * \details The default behaviour of this item when it is edited is as follows: an input of
   * sgl::Input::up/right increases the item's value by it's delta value, i.e. value += delta. An
   * input equal to sgl::Input::down/left decreases the item's value by it's delta value, i.e. value
   * -= value.
   * \tparam T value type
   * \tparam TextSize display width
   * \tparam CharT character type
   */
  template <size_t TextSize, typename CharT, typename T>
  class Numeric : public sgl::ItemBase<Numeric<TextSize, CharT, T>> {
  public:
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                  "T must be an integral type");
    static_assert(!std::is_same_v<bool, T>,
                  "T must not be bool. Use Boolean_t for a boolean item.");
    using item_type = Numeric<TextSize, CharT, T>;
    using Base = sgl::ItemBase<item_type>;

    using StringView = typename Base::StringView;
    using String = typename Base::String;
    using value_type = T;
    /// concrete formatter type
    using Formatter_t = Callable<sgl::error(sgl::static_string<CharT, TextSize>&, value_type)>;

    static constexpr size_t text_size = TextSize;

    /**
     * \brief Construct a new Numeric with default formatter and default
     * input handling. This constructor is constexpr if T is integral
     * \param name name of item
     * \param initial_value initial value
     * \param delta delta value
     */
    constexpr Numeric(StringView name, T initial_value, T delta) noexcept;

    constexpr Numeric(StringView name, T initial_value, StringView initial_text, T delta) noexcept;

    /**
     * \brief create a Numeric item in constexpr with a sgl::cx_arg.
     * \details If you don't need a constexpr Numeric, use the overload with an ordinary T as
     * initial value. This is only needed for floating point numeric items.
     *
     * \tparam CxSize cx_arg string size.
     * \param name name of item
     * \param initial_value initial value as cx_arg
     * \param delta delta value
     */
    template <size_t CxSize>
    constexpr Numeric(StringView name, const cx_arg<T, CxSize>& initial_value, T delta) noexcept;

    /**
     * \brief Construct a new Numeric with custom formatter, but default
     * input handling.
     *
     * \tparam Formatter formatter type, see formatter_check for more details.
     * \param name name of item
     * \param initial_value inital value
     * \param delta delta value
     * \param formatter formatter instance
     */
    template <typename Formatter, enable_if_is_value_formatter<Formatter, item_type> = true>
    constexpr Numeric(StringView name, T initial_value, T delta, Formatter&& formatter) noexcept;

    /**
     * \brief Construct a Numeric object with custom formatter and input
     * handler.
     *
     * \tparam Formatter formatter type, see formatter_check for more details.
     * \tparam InputHandler Input handler type, see input_handler_for_v
     * for more details.
     * \param name name of item
     * \param initial_value initial value
     * \param delta delta value
     * \param formatter formatter instance
     * \param handler handler instance
     */
    template <typename Formatter,
              typename InputHandler,
              enable_if_is_value_formatter<Formatter, item_type>,
              enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>> = true>
    constexpr Numeric(StringView     name,
                      T              initial_value,
                      T              delta,
                      Formatter&&    formatter,
                      InputHandler&& handler) noexcept;

    /// execute the formatter
    constexpr sgl::error format(static_string<CharT, TextSize>& str, T val) noexcept;

    /// get delta value.
    constexpr T get_delta() const noexcept;

    /// set delta value.
    constexpr sgl::error set_delta(T delta) noexcept;

    /// get value stored.
    constexpr T get_value() const noexcept;

    /// set value
    constexpr sgl::error set_value(T value) noexcept;

    /// get format buffer
    constexpr static_string<CharT, TextSize>& get_buffer() noexcept;

  private:
    /// default input handler
    constexpr static sgl::error default_handle_input(item_type& numeric_item,
                                                     sgl::Input input) noexcept;

    constexpr static sgl::error default_format(static_string<CharT, TextSize>& text,
                                               T                               value) noexcept;

    Formatter_t                    format_{&default_format}; ///< formatter
    T                              value_{0};                ///< value
    T                              delta_{1};                ///< delta value
    static_string<CharT, TextSize> format_buffer_;           ///< format buffer
  };

  /**
   * \brief
   * \tparam TextSize
   * \tparam CharT
   * \tparam T
   * \param name
   * \param initial_value
   * \param delta
   * \return Numeric<TextSize, CharT, T>
   * \{
   */
  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>
      make_numeric(sgl::string_view<CharT> name, T initial_value, T delta) noexcept;

  template <size_t TextSize, typename CharT, typename T, size_t N>
  constexpr Numeric<TextSize, CharT, T>
      make_numeric(const CharT (&name)[N], T initial_value, T delta) noexcept;
  /// \}

  /**
   * \brief
   * \tparam TextSize
   * \tparam CharT
   * \tparam T
   * \param name
   * \param initial_value
   * \param initial_text
   * \param delta
   * \return constexpr Numeric<TextSize, CharT, T>
   */
  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     sgl::string_view<CharT> initial_text,
                                                     T                       delta) noexcept;

  /**
   * \brief
   * \tparam TextSize
   * \tparam T
   * \tparam CxSize
   * \param name
   * \param initial_value
   * \param delta
   * \return constexpr Numeric<TextSize, char, T>
   * \{
   */
  template <size_t TextSize, typename T, size_t CxSize>
  constexpr Numeric<TextSize, char, T> make_numeric(sgl::string_view<char>   name,
                                                    const cx_arg<T, CxSize>& initial_value,
                                                    T                        delta) noexcept;

  template <size_t TextSize, typename T, size_t N, size_t CxSize>
  constexpr Numeric<TextSize, char, T>
      make_numeric(const char (&name)[N], const cx_arg<T, CxSize>& initial_value, T delta) noexcept;
  /// \}

  /**
   * \brief
   * \tparam TextSize
   * \tparam CharT
   * \tparam T
   * \tparam Formatter
   * \param name
   * \param initial_value
   * \param delta
   * \param formatter
   * \return Numeric<TextSize, CharT, T>
   */
  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     T                       delta,
                                                     Formatter&&             formatter) noexcept;

  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
            enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     T                       delta,
                                                     Formatter&&             formatter,
                                                     InputHandler&&          handler) noexcept;
} // namespace sgl

#include "impl/sgl_numeric_impl.hpp"
#endif