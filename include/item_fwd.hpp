#ifndef SGL_ITEM_FWD_HPP
#define SGL_ITEM_FWD_HPP
namespace sgl{
  template <typename Traits>
  class ItemBase;
  template <size_t TextSize, typename CharT>
  class Button;
  template <size_t TextSize, typename CharT>
  class Boolean;
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  class Enum;
  template <size_t TextSize, typename CharT>
  class PageLink;
  template <typename T, typename CharT>
  struct Pair;
  template <typename T, size_t TextSize, typename CharT>
  class Numeric;
}
#endif