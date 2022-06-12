#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl_item_base.hpp"
namespace sgl {
  /// @cond
  namespace impl {
    template <typename CharT>
    static constexpr CharT True[] = {'T', 'R', 'U', 'E', '\0'};
    template <typename CharT>
    static constexpr CharT False[] = {'F', 'A', 'L', 'S', 'E', '\0'};
  }; // namespace impl
  /// @endcond

  /**
   * @brief This class implements a basic Boolean item. When clicked, it will
   * toggle it's value and set it's text accordingly.
   * @details Boolean consists of the following data:
   * - Boolean value: the value of this item
   * - 'true' string: the text to show when value == true.
   * - 'false' string: the text to show when value == false.
   *
   * @tparam TextSize number of characters per line in the menu
   * @tparam CharT character type of the item
   */
  template <size_t TextSize, typename CharT>
  class Boolean : public sgl::ItemBase<traits<Boolean<TextSize, CharT>>> {
  public:
    using item_type = Boolean<TextSize, CharT>;
    using Base = sgl::ItemBase<traits<Boolean<TextSize, CharT>>>;

    using StringView = typename Base::StringView;

    /**
     * @brief Construct a Boolean item, which toggles its text between "TRUE"
     * and "FALSE" when clicked.
     *
     * @param name name of the Boolean item
     * @param initial_value initial value of the item
     */
    constexpr Boolean(StringView name, bool initial_value) noexcept;

    /**
     * @brief Construct a Boolean item with custom true and false strings.
     * @param name name of item
     * @param value inital value of item
     * @param true_text text to set when value is true
     * @param false_text text to set when value is false
     */
    constexpr Boolean(StringView name,
                      bool       value,
                      StringView true_text,
                      StringView false_text) noexcept;

    constexpr bool get_value() const noexcept;

    constexpr sgl::error set_value(bool value) noexcept;

    constexpr StringView true_string() const noexcept;

    constexpr StringView false_string() const noexcept;

  private:
    static constexpr sgl::error default_handle_input(item_type& Boolean, sgl::Input) noexcept;

    StringView true_string_{sgl::impl::True<CharT>};
    StringView false_string_{sgl::impl::False<CharT>};
    bool       value_{false};
  };

  template <typename CharT>
  Boolean(string_view<CharT> name, bool value) -> Boolean<5, CharT>;

  template <typename CharT>
  Boolean(const CharT* name, bool value) -> Boolean<5, CharT>;

  template <size_t TextSize, typename CharT>
  constexpr Boolean<TextSize, CharT>::Boolean(StringView name, bool initial_value) noexcept
      : Base(name,
             initial_value ? StringView(sgl::impl::True<CharT>)
                           : StringView(sgl::impl::False<CharT>),
             &default_handle_input),
        value_(initial_value) {}

  template <size_t TextSize, typename CharT>
  constexpr Boolean<TextSize, CharT>::Boolean(StringView name,
                                              bool       value,
                                              StringView true_text,
                                              StringView false_text) noexcept
      : Base(name, value ? true_text : false_text, &default_handle_input), true_string_(true_text),
        false_string_(false_text), value_(value) {}

  template <size_t TextSize, typename CharT>
  constexpr bool Boolean<TextSize, CharT>::get_value() const noexcept {
    return value_;
  }

  template <size_t TextSize, typename CharT>
  constexpr sgl::error Boolean<TextSize, CharT>::set_value(bool value) noexcept {
    value_ = value;
    if (value)
      this->set_text(true_string());
    else
      this->set_text(false_string());
    return sgl::error::no_error;
  }

  template <size_t TextSize, typename CharT>
  constexpr typename Boolean<TextSize, CharT>::StringView
      Boolean<TextSize, CharT>::true_string() const noexcept {
    return true_string_;
  }

  template <size_t TextSize, typename CharT>
  constexpr typename Boolean<TextSize, CharT>::StringView
      Boolean<TextSize, CharT>::false_string() const noexcept {
    return false_string_;
  }

  template <size_t TextSize, typename CharT>
  constexpr sgl::error Boolean<TextSize, CharT>::default_handle_input(item_type& Boolean,
                                                                      sgl::Input) noexcept {
    if (Boolean.get_value()) {
      Boolean.set_value(false);
    } else {
      Boolean.set_value(true);
    }
    return sgl::error::no_error;
  }
} // namespace sgl
#endif