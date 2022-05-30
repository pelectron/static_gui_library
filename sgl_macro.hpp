#ifndef SGL_MACRO_HPP
#define SGL_MACRO_HPP

#define DEFINE_SGL_TYPES(line_width, char_type)                              \
  using string_view = sgl::string_view<char_type>;                           \
  using Item = sgl::Item_t<line_width, char_type>;                           \
  using Button = sgl::Button_t<line_width, char_type>;                       \
  using Text = sgl::Text_t<line_width, char_type>;                           \
  using ConstText = sgl::ConstText_t<line_width, char_type>;                 \
  template <typename T>                                                      \
  using Number = sgl::Numeric_t<T, line_width, char_type>;                   \
  using Integer = Number<int>;                                               \
  using Floating = Number<double>;                                           \
  template <typename E, size_t N>                                            \
  using Enum = sgl::Enum_t<E, N, line_width, char_type>;                     \
  using PageLink = sgl::PageLink_t<line_width, char_type>;                   \
  using Boolean = sgl::Boolean_t<line_width, char_type>;                     \
  template <typename... Items>                                               \
  auto make_page(string_view name,                                           \
                 string_view title,                                          \
                 sgl::Input  start_edit,                                     \
                 sgl::Input  stop_edit,                                      \
                 Items&&... items) {                                         \
    return sgl::make_page<line_width, char_type>(                            \
        name,                                                                \
        title,                                                               \
        start_edit,                                                          \
        stop_edit,                                                           \
        std::forward<Items>(items)...);                                      \
  }                                                                          \
                                                                             \
  template <typename InputHandler,                                           \
            typename... Items,                                               \
            typename std::enable_if_t<std::is_invocable_r_v<                 \
                sgl::error,                                                  \
                InputHandler,                                                \
                sgl::Page_t<line_width, char_type, Items...>&,               \
                sgl::Input>>* = nullptr>                                     \
  sgl::Page_t<line_width, char_type, Items...> make_page(                    \
      string_view    name,                                                   \
      string_view    title,                                                  \
      sgl::Input     start_edit,                                             \
      sgl::Input     stop_edit,                                              \
      InputHandler&& input_handler,                                          \
      Items&&... items) {                                                    \
    return sgl::make_page<line_width, char_type>(                            \
        name,                                                                \
        title,                                                               \
        start_edit,                                                          \
        stop_edit,                                                           \
        std::forward<InputHandler>(input_handler),                           \
        std::forward<Items>(items)...);                                      \
  }                                                                          \
  template <typename... Items,                                               \
            std::enable_if_t<(sgl::is_item_v<Items> && ...)>* = nullptr>     \
  sgl::Page_t<line_width, char_type, Items...> make_page(string_view name,   \
                                                         string_view title,  \
                                                         Items&&... items) { \
    return sgl::Page_t<line_width, char_type, Items...>(                     \
        name,                                                                \
        title,                                                               \
        std::forward<Items>(items)...);                                      \
  }                                                                          \
  template <typename E, size_t NumEnumerators>                               \
  Enum<E, NumEnumerators> make_enum(                                         \
      string_view item_name,                                                 \
      const sgl::Pair<E, char_type>(&enum_map)[NumEnumerators],              \
      size_t start_index = 0) {                                              \
    return sgl::make_enum<E, line_width>(item_name, enum_map, start_index);  \
  }                                                                          \
  template <typename... Pages>                                               \
  sgl::Menu_t<line_width, char_type, Pages...> make_menu(                    \
      string_view menu_name,                                                 \
      sgl::Input  start_edit,                                                \
      sgl::Input  stop_edit,                                                 \
      Pages&&... pages) {                                                    \
    return sgl::make_menu<line_width, char_type, Pages...>(                  \
        menu_name,                                                           \
        start_edit,                                                          \
        stop_edit,                                                           \
        std::forward<Pages>(pages)...);                                      \
  }

#define DEFINE_SGL_TYPES_IN_NS(line_width, char_type, name_space) \
  namespace name_space {                                          \
    DEFINE_SGL_TYPE(line_width, char_type)                        \
  }

#endif