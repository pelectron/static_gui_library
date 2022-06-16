#ifndef SGL_BASIC_BUTTON_HPP
#define SGL_BASIC_BUTTON_HPP
#include "sgl_pass_through_button.hpp"
namespace sgl {

  template <size_t TextSize, typename CharT>
  class Button : public sgl::PassThroughButton<Button<TextSize, CharT>> {
  public:
    using item_type = Button<TextSize, CharT>;
    using Base = sgl::PassThroughButton<item_type>;
    using char_type = CharT;
    static constexpr size_t text_size = TextSize;
    using StringView = typename Base::StringView;
    /// concrete type used for storing click handlers.
    using ClickHandler_t = Callable<sgl::error(item_type&)>;

    /**
     * \brief create button with name and text
     * \{
     */
    constexpr Button(StringView name_and_text) noexcept;
    constexpr Button(StringView name, StringView text) noexcept;
    /// \}

    /**
     * \brief Construct a Button with name, text, and click handler.
     * \param name name of item
     * \param text text of item
     * \param click_handler click handler
     * \tparam ClickHandler click handler type
     */
    template <typename ClickHandler,
              enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true>
    constexpr Button(StringView name, StringView text, ClickHandler&& click_handler) noexcept;

    /**
     * \brief Construct a Button with name, text, and custom click and input
     * handler.
     * \tparam ClickHandler click handler type
     * \tparam InputHandler input handler type
     * \param name name of item
     * \param text text of item
     * \param click_handler click handler
     * \param input_handler input handler
     */
    template <typename ClickHandler,
              typename InputHandler,
              enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true,
              enable_if_is_input_handler<InputHandler, Button<TextSize, CharT>> = true>
    constexpr Button(StringView     name,
                     StringView     text,
                     ClickHandler&& click_handler,
                     InputHandler&& input_handler) noexcept;
  };

} // namespace sgl
#include "impl/sgl_button_impl.hpp"
#endif