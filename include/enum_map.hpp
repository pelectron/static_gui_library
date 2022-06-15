#ifndef SGL_ENUM_MAP_HPP
#define SGL_ENUM_MAP_HPP
#include "sgl_string_view.hpp"
namespace sgl {
  template <typename T, typename CharT>
  struct Pair {
    constexpr Pair() noexcept = default;
    constexpr Pair& operator=(const Pair&) noexcept = default;
    constexpr Pair& operator=(Pair&&) noexcept = default;

    template <size_t N>
    constexpr Pair(const T& t, const CharT (&str)[N]) noexcept : value(t), string(str) {}
    constexpr Pair(const T& t, sgl::string_view<CharT> str) noexcept : value(t), string(str) {}
    constexpr Pair(const Pair&) noexcept = default;
    constexpr Pair(Pair&&) noexcept = default;

    T                       value{};
    sgl::string_view<CharT> string{};
  };
  template <typename E, size_t N, typename CharT>
  struct EnumMap {
    
    Pair<E, CharT> data[N];
  };
} // namespace sgl
#endif