#ifndef SGL_BUTTON_PASS_THROUGH_HPP
#define SGL_BUTTON_PASS_THROUGH_HPP
#include "sgl_item_base.hpp"
namespace sgl {
  template <typename ItemImpl, typename Traits = sgl::ItemTraits<ItemImpl>>
  class PassThroughButton : public sgl::ItemBase<ItemImpl, Traits> {
  public:
    using item_type = typename Traits::item_type;
    using Base = sgl::ItemBase<ItemImpl, Traits>;
    using StringView = typename Base::StringView;
    /// concrete type used for storing click handlers.
    using ClickHandler_t = Callable<sgl::error(item_type&)>;

    /**
     * \brief create button with name and text
     * \{
     */
    constexpr PassThroughButton(StringView name_and_text) noexcept;
    constexpr PassThroughButton(StringView name, StringView text) noexcept;
    /// \}

    /**
     * \brief Construct a Button with name, text, and click handler.
     * \param name name of item
     * \param text text of item
     * \param click_handler click handler
     * \tparam ClickHandler click handler type
     */
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, item_type> = true>
    constexpr PassThroughButton(StringView     name,
                                StringView     text,
                                ClickHandler&& click_handler) noexcept;

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
              enable_if_is_click_handler<ClickHandler, item_type> = true,
              enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr PassThroughButton(StringView     name,
                                StringView     text,
                                ClickHandler&& click_handler,
                                InputHandler&& input_handler) noexcept;

    /**
     * \brief Set the click handler
     * \tparam ClickHandler
     * \param click_handler
     */
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, item_type> = true>
    void           set_click_handler(ClickHandler&& click_handler) noexcept;
    constexpr void set_click_handler(sgl::error (*click_handler)(item_type&) noexcept) noexcept;

    // execute click handler
    constexpr sgl::error click() noexcept;

  private:
    constexpr static sgl::error default_handle_click(item_type&) noexcept {
      return sgl::error::no_error;
    }

    ClickHandler_t click_handler_{&default_handle_click};
  };

} // namespace sgl
#include "impl/sgl_pass_through_button_impl.hpp"
#endif