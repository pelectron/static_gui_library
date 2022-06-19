#ifndef SGL_ENUM_MAP_IMPL_HPP
#define SGL_ENUM_MAP_IMPL_HPP
#include "../sgl_enum_map.hpp"
namespace sgl {
  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(const EnumMap& other) noexcept : data(other.data) {}

  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(EnumMap&& other) noexcept : data(std::move(other.data)) {}

  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(
      const array<Pair<E, sgl::string_view<CharT>>, N>& map) noexcept
      : data(map) {}

  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(const array<Pair<E, const CharT*>, N>& map) noexcept {
    for (size_t i = 0; i < N; ++i) {
      data[i].first = map[i].first;
      data[i].second = sgl::string_view<CharT>(map[i].second);
    }
  }

  template <typename E, size_t N, typename CharT>
  constexpr E EnumMap<E, N, CharT>::operator[](sgl::string_view<CharT> str) const noexcept {
    for (const auto& elem : data) {
      if (elem.second == str)
        return elem.first;
    }
    return E{};
  }

  template <typename E, size_t N, typename CharT>
  constexpr sgl::string_view<CharT> EnumMap<E, N, CharT>::operator[](E value) const noexcept {
    for (const auto& elem : data) {
      if (elem.first == value)
        return elem.second;
    }
    return sgl::string_view<CharT>{};
  }

  template <typename E, size_t N, typename CharT>
  constexpr bool EnumMap<E, N, CharT>::contains(E value) const noexcept {
    return index_of(value) != numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  constexpr bool EnumMap<E, N, CharT>::contains(sgl::string_view<CharT> string) const noexcept {
    return index_of(string) != numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  constexpr size_t EnumMap<E, N, CharT>::index_of(E value) const noexcept {
    size_t i{0};
    for (const auto& elem : data) {
      if (elem.first == value)
        return i;
      ++i;
    }
    return numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  constexpr size_t EnumMap<E, N, CharT>::index_of(sgl::string_view<CharT> string) const noexcept {
    size_t i{0};
    for (const auto& elem : data) {
      if (elem.second == string)
        return i;
      ++i;
    }
    return numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  constexpr sgl::string_view<CharT> EnumMap<E, N, CharT>::get_view(size_t i) const noexcept {
    return data[i % N].second;
  }

  template <typename E, size_t N, typename CharT>
  constexpr E EnumMap<E, N, CharT>::get_value(size_t i) const noexcept {
    return data[i % N].first;
  }
} // namespace sgl
#endif