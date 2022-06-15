#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl_item_base.hpp"
namespace sgl {
  /**
   * \brief This class implements a basic Boolean item. When clicked, it will
   * toggle it's value and set it's text accordingly.
   * \details Boolean consists of the following data:
   * - Boolean value: the value of this item
   * - 'true' string: the text to show when value == true.
   * - 'false' string: the text to show when value == false.
   *
   * \tparam TextSize number of characters per line in the menu
   * \tparam CharT character type of the item
   */
  template <size_t TextSize, typename CharT>
  class Boolean : public sgl::ItemBase<Boolean<TextSize, CharT>> {
  public:
    using item_type = Boolean<TextSize, CharT>;
    using Base = sgl::ItemBase<Boolean<TextSize, CharT>>;

    using StringView = typename Base::StringView;

    /**
     * \brief Construct a Boolean item, which toggles its text between "TRUE"
     * and "FALSE" when clicked.
     *
     * \param name name of the Boolean item
     * \param initial_value initial value of the item
     */
    constexpr Boolean(StringView name, bool initial_value) noexcept;

    /**
     * \brief Construct a Boolean item with custom true and false strings.
     * \param name name of item
     * \param value inital value of item
     * \param true_text text to set when value is true
     * \param false_text text to set when value is false
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

    StringView true_string_{};
    StringView false_string_{};
    bool       value_{false};
  };

  template <typename CharT>
  Boolean(string_view<CharT> name, bool value) -> Boolean<5, CharT>;

  template <typename CharT, size_t N>
  Boolean(const CharT (&name)[N], bool value) -> Boolean<5, CharT>;

  template <typename CharT, size_t N, size_t NTrue, size_t NFalse>
  Boolean(const CharT (&name)[N],
          const CharT (&true_string)[NTrue],
          const CharT (&false_string)[NFalse],
          bool value) -> Boolean<((NTrue > NFalse) ? NTrue : NFalse), CharT>;
} // namespace sgl
#include "impl/sgl_boolean_impl.hpp"
#endif