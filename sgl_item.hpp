#ifndef SGL_ITEM_HPP
#define SGL_ITEM_HPP
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_static_string.hpp"

#include <type_traits>

namespace sgl {
  template <size_t LineWidth, typename CharT>
  class Item_t {
  public:
    using InputHandler_t =
        Callable<sgl::error(Item_t<LineWidth, CharT>&, sgl::Input)>;
    template <typename T>
    using input_handler_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error,
                                               T,
                                               Item_t<LineWidth, CharT>&,
                                               sgl::Input>>;

    Item_t(string_view<CharT> item_name, string_view<CharT> text)
        : name_(item_name), text_(text) {}

    Item_t(string_view<CharT> name_and_text)
        : name_(name_and_text), text_(name_and_text) {}

    template <typename InputHandler,
              typename = input_handler_check<InputHandler>>
    Item_t(string_view<CharT> item_name,
           string_view<CharT> text,
           InputHandler&&     handler)
        : handler(std::forward<InputHandler>(handler)), name_(item_name),
          text_(text) {}

    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Item_t(string_view<CharT> name_and_text, InputHandler&& handler)
        : handler(std::forward<InputHandler>(handler)),
          name_(name_and_text), text_{name_and_text} {}

    template <typename Menu>
    void set_menu(Menu*) {}

    sgl::error handle_input(sgl::Input input) { return handler(*this, input); }

    sgl::error set_text(sgl::string_view<CharT> new_text) {
      text_ = new_text;
      return sgl::error::no_error;
    }

    const StaticString<CharT, LineWidth>& get_text() const { return text_; }
    StaticString<CharT, LineWidth>&       get_text() { return text_; }
    string_view<CharT>                    get_name() const { return name_; }
    void                                  clear_text() { text_.reset(); }

    template <typename InputHandler,
              typename = input_handler_check<InputHandler>>
    void set_input_handler(InputHandler&& handler) {
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
    string_view<CharT>             name_{};
    StaticString<CharT, LineWidth> text_{};
  };

  template <typename Item>
  struct is_item : std::false_type {};
  template <size_t LineWidth, typename CharT>
  struct is_item<Item_t<LineWidth, CharT>> : std::true_type {};
  template <typename Item>
  static constexpr bool is_item_v = is_item<Item>::value;
} // namespace sgl
#endif