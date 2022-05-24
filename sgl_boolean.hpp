#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl_button.hpp"
namespace sgl {

  template <size_t LineWidth, typename CharT>
  class Boolean_t : public sgl::Button_t<LineWidth, CharT> {
  public:
    Boolean_t(basic_string_view<CharT> name, bool initial_value)
        : Button_t<LineWidth, CharT>(
              name,
              initial_value ? "TRUE" : "FALSE",
              [](Button_t<LineWidth, CharT>& button) {
                if (basic_string_view<CharT>(button.get_text()) ==
                    basic_string_view<CharT>("TRUE")) {
                  button.set_text("FALSE");
                } else {
                  button.set_text("TRUE");
                }
                return sgl::error::no_error;
              }) {}
  };
} // namespace sgl
#endif