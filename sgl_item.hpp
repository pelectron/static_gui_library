#ifndef SGL_ITEM_HPP
#define SGL_ITEM_HPP
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_static_string.hpp"

#include <type_traits>

namespace sgl {
  /**
   * @brief
   *
   * @tparam LineWidth number of characters per line in the menu
   * @tparam CharT character type of the item
   * @addtogroup item_types Item Types
   * @{
   */
  template <size_t LineWidth, typename CharT>
  class Item_t {
  public:
    using string_view_t = sgl::string_view<CharT>;

    using InputHandler_t =
        Callable<sgl::error(Item_t<LineWidth, CharT>&, sgl::Input)>;
    /// SFINAE alias to check input handler types
    template <typename T>
    using input_handler_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error,
                                               T,
                                               Item_t<LineWidth, CharT>&,
                                               sgl::Input>>;

    /**
     * @brief Construct a Item_t with a name and text.
     *
     * @param item_name name of the item
     * @param text text of the item
     */
    Item_t(string_view_t item_name, string_view_t text)
        : name_(item_name), text_(text) {}

    /**
     * @brief Construct a new Item_t which has the same text for its name and
     * value.
     *
     * @param name_and_text name and text of item.
     */
    Item_t(string_view_t name_and_text)
        : name_(name_and_text), text_(name_and_text) {}

    template <typename InputHandler,
              typename = input_handler_check<InputHandler>>
    Item_t(string_view_t item_name, string_view_t text, InputHandler&& handler)
        : handler(std::forward<InputHandler>(handler)), name_(item_name),
          text_(text) {}

    template <typename InputHandler,
              typename = input_handler_check<InputHandler>>
    Item_t(string_view_t name_and_text, InputHandler&& handler)
        : handler(std::forward<InputHandler>(handler)),
          name_(name_and_text), text_{name_and_text} {}

    template <typename Menu>
    void set_menu(Menu*) {}

    sgl::error handle_input(sgl::Input input) { return handler(*this, input); }

    sgl::error set_text(string_view_t new_text) {
      text_ = new_text;
      return sgl::error::no_error;
    }

    const StaticString<CharT, LineWidth>& get_text() const { return text_; }
    StaticString<CharT, LineWidth>&       get_text() { return text_; }
    string_view_t                         get_name() const { return name_; }
    void                                  clear_text() { text_.reset(); }

    template <typename InputHandler>
    void set_input_handler(InputHandler&& handler) {
      static_assert(std::is_invocable_r_v<sgl::error,
                                          T,
                                          Item_t<LineWidth, CharT>&,
                                          sgl::Input>,
                    "the provided handler is not a valid input handler");
      if constexpr (std::is_same_v<InputHandler, decltype(handler)>) {
        this->handler = handler;
      } else {
        this->handler.bind(std::forward<InputHandler>(handler));
      }
    }

  private:
    static sgl::error default_handle_input(Item_t<LineWidth, CharT>&,
                                           sgl::Input) {
      return sgl::error::edit_finished;
    }

    InputHandler_t                 handler{&default_handle_input};
    string_view_t                  name_{};
    StaticString<CharT, LineWidth> text_{};
  };
  /// @}

} // namespace sgl
#endif