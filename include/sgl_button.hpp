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
     * @brief create button with name and text
     * @{
     */
    constexpr Button(StringView name_and_text) noexcept;
    constexpr Button(StringView name, StringView text) noexcept;
    /// @}

    /**
     * @brief Construct a Button with name, text, and custom click handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     */
    constexpr Button(StringView name,
                     StringView text,
                     sgl::error (*click_handler)(Button&) noexcept) noexcept;

    /**
     * @brief Construct a Button with name, text, and click handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @tparam ClickHandler click handler type
     */
    template <typename ClickHandler,
              enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true>
    Button(StringView name, StringView text, ClickHandler&& click_handler) noexcept;

    /**
     * @brief Construct a Button with name, text, and custom click and input
     * handler.
     * @tparam ClickHandler click handler type
     * @tparam InputHandler input handler type
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @param input_handler input handler
     */
    template <typename ClickHandler,
              typename InputHandler,
              enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true,
              enable_if_is_input_handler<InputHandler, Button<TextSize, CharT>> = true>
    Button(StringView     name,
           StringView     text,
           ClickHandler&& click_handler,
           InputHandler&& input_handler) noexcept;
    /**
     * @brief Construct a Button with name, text, and custom click and input
     * handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @param input_handler input handler
     */
    constexpr Button(StringView name,
                     StringView text,
                     sgl::error (*click_handler)(item_type&) noexcept,
                     sgl::error (*input_handler)(item_type&, sgl::Input) noexcept) noexcept;
  };

  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(StringView name_and_text) noexcept
      : Base(name_and_text) {}

  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(StringView name, StringView text) noexcept
      : Base(name, text) {}

  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(
      StringView name,
      StringView text,
      sgl::error (*click_handler)(item_type&) noexcept) noexcept
      : Base(name, text, click_handler) {}

  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(
      StringView name,
      StringView text,
      sgl::error (*click_handler)(item_type&) noexcept,
      sgl::error (*input_handler)(item_type&, sgl::Input) noexcept) noexcept
      : Base(name, text, click_handler, input_handler) {}

  template <size_t TextSize, typename CharT>
  template <typename ClickHandler,
            sgl::enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>>
  Button<TextSize, CharT>::Button(StringView     name,
                                  StringView     text,
                                  ClickHandler&& click_handler) noexcept
      : Base(name, text, forward<ClickHandler>(click_handler)) {}

  template <size_t TextSize, typename CharT>
  template <typename ClickHandler,
            typename InputHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>,
            enable_if_is_input_handler<InputHandler, Button<TextSize, CharT>>>
  Button<TextSize, CharT>::Button(StringView     name,
                                  StringView     text,
                                  ClickHandler&& click_handler,
                                  InputHandler&& input_handler) noexcept
      : Base(name,
             text,
             forward<ClickHandler>(click_handler),
             forward<InputHandler>(input_handler)) {}

} // namespace sgl
#endif