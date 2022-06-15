#ifndef SGL_ENUM_MAP_HPP
#define SGL_ENUM_MAP_HPP
#include "sgl_string_view.hpp"
#include "sgl_type_traits.hpp"
namespace sgl {
  template <typename T, typename CharT>
  struct Pair {
    using value_type = T;
    using char_type = CharT;
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
    constexpr EnumMap(const Pair<E, CharT> (&map)[N]){
      for (size_t i = 0; i < N; ++i) {
        data[i] = map[i];
      }
    }

    Pair<E, CharT> data[N]{};
  };
  template <typename E, size_t N, typename CharT>
  EnumMap(const Pair<E, CharT> (&map)[N]) -> EnumMap<E, N, CharT>;
  template <typename E, size_t N, typename C>
  E consume_map(const Pair<E, C> (&map)[N]){
    (void)map;
    return map[0].value;
  }
} // namespace sgl
#endif