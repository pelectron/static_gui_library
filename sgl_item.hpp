#ifndef SGL_ITEM_HPP
#define SGL_ITEM_HPP
#include "callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "static_string.hpp"

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

    Item_t(string_view<CharT> name, string_view<CharT> text)
        : name(name), text(text) {}

    Item_t(string_view<CharT> name_and_text)
        : name(name_and_text), text(name_and_text) {}

    template <typename InputHandler,
              typename = input_handler_check<InputHandler>>
    Item_t(string_view<CharT> name,
           string_view<CharT> text,
           InputHandler&&           handler)
        : handler(std::forward<InputHandler>(handler)), name(name), text(text) {
    }

    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Item_t(string_view<CharT> name_and_text, InputHandler&& handler)
        : handler(std::forward<InputHandler>(handler)),
          name(name_and_text), text{name_and_text} {}
    template <typename Menu>
    void       set_menu(Menu*) {}
    sgl::error handle_input(sgl::Input input) { return handler(*this, input); }
    sgl::error set_text(const CharT* str, size_t n) {
      return sgl::error::no_error;
    }
    sgl::error set_text(string_view<CharT> new_text) {
      text = new_text;
      return sgl::error::no_error;
    }
    const StaticString<CharT, LineWidth>& get_text() const { return text; }
    StaticString<CharT, LineWidth>&       get_text() { return text; }
    string_view<CharT>              get_name() const { return name; }
    void                                  clear_text() { text.reset(); }

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
    string_view<CharT>       name{};
    StaticString<CharT, LineWidth> text{};
  };
} // namespace sgl
#endif