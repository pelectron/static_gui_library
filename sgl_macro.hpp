#ifndef SGL_MACRO_HPP
#define SGL_MACRO_HPP
#define DEFINE_SGL_TYPES(line_width, char_type)                   \
  using Element = sgl::Item_t<line_width, char_type>;             \
  using Button = sgl::Button_t<line_width, char_type>;            \
  using Text = sgl::Text_t<line_width, char_type>;                \
  using ConstText = sgl::ConstText_t<line_width, char_type>;      \
  using Number = sgl::Integer_t<int, line_width, char_type>;      \
  template <typename E, size_t N>                                 \
  using Enum = sgl::Enum_t<E, N, line_width, char_type>;          \
  using PageLink = sgl::PageLink_t<line_width, char_type>;        \
  using Boolean = sgl::Boolean_t<line_width, char_type>;          \
  template <typename... Items>                                    \
  auto make_page(sgl::string_view<char_type> name,          \
                 sgl::string_view<char_type> title,         \
                 sgl::Input                        start_edit,    \
                 sgl::Input                        stop_edit,     \
                 Items&&... items) {                              \
    return sgl::make_page<line_width, char_type>(                 \
        name,                                                     \
        title,                                                    \
        start_edit,                                               \
        stop_edit,                                                \
        std::forward<Items>(items)...);                           \
  }                                                               \
                                                                  \
  template <typename InputHandler,                                \
            typename... Items,                                    \
            typename std::enable_if_t<std::is_invocable_r_v<      \
                sgl::error,                                       \
                InputHandler,                                     \
                sgl::Page_t<line_width, char_type, Items...>&,    \
                sgl::Input>>* = nullptr>                          \
  auto make_page(sgl::string_view<char_type> name,          \
                 sgl::string_view<char_type> title,         \
                 sgl::Input                        start_edit,    \
                 sgl::Input                        stop_edit,     \
                 InputHandler&&                    input_handler, \
                 Items&&... items) {                              \
    return sgl::make_page<line_width, char_type>(                 \
        name,                                                     \
        title,                                                    \
        start_edit,                                               \
        stop_edit,                                                \
        std::forward<InputHandler>(input_handler),                \
        std::forward<Items>(items)...);                           \
  }

#define UNDEF_SGL_MACROS #undef DEFINE_SGL_TYPES

#endif