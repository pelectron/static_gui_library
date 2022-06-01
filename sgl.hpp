#ifndef SGL_HPP
#define SGL_HPP
#include "sgl_boolean.hpp"
#include "sgl_button.hpp"
#include "sgl_config.h"
#include "sgl_enum.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_item.hpp"
#include "sgl_menu.hpp"
#include "sgl_numeric.hpp"
#include "sgl_page.hpp"
#include "sgl_page_link.hpp"
#include "sgl_string_view.hpp"
#include "sgl_text.hpp"

#if SGL_INSTANTIATE == 1

namespace sgl {
  using StringView = sgl::string_view<SGL_CHAR_TYPE>;
  using Item = sgl::Item_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using Button = sgl::Button_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using Text = sgl::Text_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using ConstText = sgl::ConstText_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  template <typename T>
  using Number = sgl::Numeric_t<T, SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using Integer = Number<int>;
  using Floating = Number<double>;
  template <typename E, size_t N>
  using Enum = sgl::Enum_t<E, N, SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using PageLink = sgl::PageLink_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;
  using Boolean = sgl::Boolean_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE>;

  template <typename... Items>
  auto make_page(StringView name,
                 StringView title,
                 sgl::Input start_edit,
                 sgl::Input stop_edit,
                 Items&&... items) {
    return sgl::make_page<SGL_LINE_WIDTH, SGL_CHAR_TYPE>(
        name,
        title,
        start_edit,
        stop_edit,
        std::forward<Items>(items)...);
  }

  template <typename InputHandler,
            typename... Items,
            typename std::enable_if_t<std::is_invocable_r_v<
                sgl::error,
                InputHandler,
                sgl::Page_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE, Items...>&,
                sgl::Input>>* = nullptr>
  sgl::Page_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE, Items...>
      make_page(StringView     name,
                StringView     title,
                sgl::Input     start_edit,
                sgl::Input     stop_edit,
                InputHandler&& input_handler,
                Items&&... items) {
    return sgl::make_page<SGL_LINE_WIDTH, SGL_CHAR_TYPE>(
        name,
        title,
        start_edit,
        stop_edit,
        std::forward<InputHandler>(input_handler),
        std::forward<Items>(items)...);
  }
  template <typename... Items,
            std::enable_if_t<(sgl::is_item_v<Items> && ...)>* = nullptr>
  sgl::Page_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE, Items...>
      make_page(StringView name, StringView title, Items&&... items) {
    return sgl::Page_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE, Items...>(
        name,
        title,
        std::forward<Items>(items)...);
  }
  template <typename E, size_t NumEnumerators>
  Enum<E, NumEnumerators>
      make_enum(StringView item_name,
                const sgl::Pair<E, SGL_CHAR_TYPE> (&enum_map)[NumEnumerators],
                size_t start_index = 0) {
    return sgl::make_enum<E, SGL_LINE_WIDTH>(item_name, enum_map, start_index);
  }
  template <typename... Pages>
  sgl::Menu_t<SGL_LINE_WIDTH, SGL_CHAR_TYPE, Pages...>
      make_menu(StringView menu_name,
                sgl::Input start_edit,
                sgl::Input stop_edit,
                Pages&&... pages) {
    return sgl::make_menu<SGL_LINE_WIDTH, SGL_CHAR_TYPE, Pages...>(
        menu_name,
        start_edit,
        stop_edit,
        std::forward<Pages>(pages)...);
  }
} // namespace sgl
#endif // SGL_INSTANTIATE
#endif // SGL_HPP