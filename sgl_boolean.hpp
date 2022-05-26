#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl_button.hpp"
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
   * @addtogroup item_types Item Types
   * @{
   * @brief This class implements a basic boolean item.
   *
   * @tparam LineWidth number of characters per line in the menu
   * @tparam CharT character type of the item
   */
  template <size_t LineWidth, typename CharT>
  class Boolean_t : public sgl::Button_t<LineWidth, CharT> {
  public:
    using string_view_t =
        typename sgl::Button_t<LineWidth, CharT>::string_view_t;

    /**
     * @brief Construct a boolean item, which toggles its text between "TRUE"
     * and "FALSE" when clicked.
     *
     * @param item_name name of the boolean item
     * @param initial_value initial value of the item
     */
    Boolean_t(string_view_t item_name, bool initial_value)
        : Button_t<LineWidth, CharT>(
              item_name,
              initial_value ? string_view_t(impl::True<CharT>)
                            : string_view_t(impl::False<CharT>),
              [](Button_t<LineWidth, CharT>& button) {
                auto& b = static_cast<Boolean_t<LineWidth, CharT>&>(button);
                if (b.get_value()) {
                  b.set_text(impl::False<CharT>);
                  b.set_value(false);
                } else {
                  b.set_text(impl::True<CharT>);
                  b.set_value(true);
                }
                return sgl::error::no_error;
              }),
          value_(initial_value) {}
    /**
     * @brief Construct a boolean item with custom click handler.
     *
     * @tparam ClickHandler
     * @param item_name
     * @param initial_value
     * @param click_handler
     */
    template <typename ClickHandler>
    Boolean_t(string_view_t  item_name,
              string_view_t  initial_value,
              ClickHandler&& click_handler)
        : Button_t<LineWidth, CharT>(
              item_name,
              initial_value,
              std::forward<ClickHandler>(click_handler)) {}

    bool get_value() const { return value_; }
    void set_value(bool value) { value_ = value; }

  private:
    bool value_;
  };
  /// @}

} // namespace sgl
#endif