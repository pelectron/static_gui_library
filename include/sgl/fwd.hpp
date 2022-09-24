#ifndef SGL_FWD_HPP
#define SGL_FWD_HPP
#include <cstdint>
namespace sgl {
  /// \cond
  enum class Input : uint64_t;
  enum class error : int;
  struct format_result;

  template <typename NameList, typename PageTypeList>
  class Menu;

  template <typename ItemNameList, typename ItemTypeList>
  class Page;

  template <typename Impl, typename Traits>
  class ItemBase;

  template <size_t TextSize, typename CharT>
  class Button;

  template <size_t TextSize, typename CharT>
  class Boolean;

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  class Enum;

  template <typename E, size_t N, typename CharT>
  struct EnumMap;

  template <typename PageName, size_t TextSize, typename CharT>
  class PageLink;

  template <size_t TextSize, typename CharT, typename T>
  class Numeric;

  template <typename T, typename CharT>
  struct Pair;

  template <char... Chars>
  struct Name;

  template <typename Name, typename T>
  class NamedValue;

  template <typename NameList, typename ValueTypeList>
  class NamedTuple;

  template <typename T, size_t N>
  struct cx_arg;

  template <typename CharT, size_t Capacity>
  class static_string;

  template <typename CharT>
  class string_view;

  template <size_t... I>
  struct index_seq_t;
  /// \endcond
} // namespace sgl
#endif /* SGL_FWD_HPP */
