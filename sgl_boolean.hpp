#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl_button.hpp"
namespace sgl {

  template <size_t LineWidth, typename CharT>
  class Boolean_t : public sgl::Button_t<LineWidth, CharT> {
  public:
    static constexpr CharT True[]{'T', 'R', 'U', 'E', '\0'};

    static constexpr CharT False[]{'F', 'A', 'L', 'S', 'E', '\0'};

    Boolean_t(string_view<CharT> item_name, bool initial_value)
        : Button_t<LineWidth, CharT>(
              item_name,
              initial_value ? string_view<CharT>(True)
                            : string_view<CharT>(False),
              [](Button_t<LineWidth, CharT>& button) {
                if (string_view<CharT>(button.get_text()) ==
                    string_view<CharT>(True)) {
                  button.set_text(False);
                } else {
                  button.set_text(True);
                }
                return sgl::error::no_error;
              }) {}
  };

  template <size_t LineWidth, typename CharT>
  struct is_item<Boolean_t<LineWidth, CharT>> : std::true_type {};
} // namespace sgl
#endif