#ifndef SGL_IMPL_ENUM_MAP_IMPL_HPP
#define SGL_IMPL_ENUM_MAP_IMPL_HPP
#include "sgl/enum_map.hpp"
namespace sgl {
  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(const EnumMap& other) noexcept : data(other.data) {}

  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(EnumMap&& other) noexcept : data(std::move(other.data)) {}

  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(
      const Array<Pair<E, sgl::string_view<CharT>>, N>& map) noexcept
      : data(map) {}

  template <typename E, size_t N, typename CharT>
  constexpr EnumMap<E, N, CharT>::EnumMap(const Array<Pair<E, const CharT*>, N>& map) noexcept {
    for (size_t i = 0; i < N; ++i) {
      data[i].first = map[i].first;
      data[i].second = sgl::string_view<CharT>(map[i].second);
    }
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr E
      EnumMap<E, N, CharT>::operator[](sgl::string_view<CharT> str) const noexcept {
    for (const auto& elem : data) {
      if (elem.second == str)
        return elem.first;
    }
    return E{};
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr sgl::string_view<CharT>
      EnumMap<E, N, CharT>::operator[](E value) const noexcept {
    for (const auto& elem : data) {
      if (elem.first == value)
        return elem.second;
    }
    return sgl::string_view<CharT>{};
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr bool EnumMap<E, N, CharT>::contains(E value) const noexcept {
    return index_of(value) != numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr bool
      EnumMap<E, N, CharT>::contains(sgl::string_view<CharT> string) const noexcept {
    return index_of(string) != numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr size_t EnumMap<E, N, CharT>::index_of(E value) const noexcept {
    size_t i{0};
    for (const auto& elem : data) {
      if (elem.first == value)
        return i;
      ++i;
    }
    return numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr size_t
      EnumMap<E, N, CharT>::index_of(sgl::string_view<CharT> string) const noexcept {
    size_t i{0};
    for (const auto& elem : data) {
      if (elem.second == string)
        return i;
      ++i;
    }
    return numeric_limits<size_t>::max();
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr sgl::string_view<CharT>
      EnumMap<E, N, CharT>::get_view(size_t i) const noexcept {
    return data[i % N].second;
  }

  template <typename E, size_t N, typename CharT>
  [[nodiscard]] constexpr E EnumMap<E, N, CharT>::get_value(size_t i) const noexcept {
    return data[i % N].first;
  }

  template <typename T, typename T1, typename T2, typename... Ts>
  constexpr bool every_second_same() {
    if constexpr (sizeof...(Ts) < 2) {
      return std::is_same_v<T, T1>;
    } else {
      return std::is_same_v<T, T1> && every_second_same<T, Ts...>();
    }
  }
  template <typename T, typename T1, typename T2, typename... Ts>
  constexpr bool every_second_convertible() {
    if constexpr (sizeof...(Ts) < 2) {
      return std::is_constructible_v<T, T1>;
    } else {
      return std::is_constructible_v<T, T1> && every_second_convertible<T, Ts...>();
    }
  }

  template <typename Str>
  struct get_char_type;
  template <typename CharT>
  struct get_char_type<const CharT*> {
    using type = CharT;
  };
  template <typename CharT, size_t N>
  struct get_char_type<const CharT (&)[N]> {
    using type = CharT;
  };
  template <typename CharT>
  struct get_char_type<sgl::string_view<CharT>> {
    using type = CharT;
  };
  template <typename T>
  struct get_string_size {
    static constexpr size_t value = 0;
  };
  template <typename T, size_t N>
  struct get_string_size<T (&)[N]> {
    static constexpr size_t value = N - 1;
  };
  template <typename T, size_t N>
  struct get_string_size<T[N]> {
    static constexpr size_t value = N - 1;
  };
  template <typename E, typename Str, typename... Rs>
  constexpr bool check_template_params() noexcept {
    using char_type = typename get_char_type<Str>::type;
    return every_second_same<E, Rs...>() &&
           every_second_convertible<sgl::string_view<char_type>, Str, Rs...>();
  }

  template <typename CharT, size_t N, typename E, typename Str, typename... Rest>
  constexpr auto enum_map_impl2(Array<Pair<E, sgl::string_view<CharT>>, N>& data,
                                size_t                                      index,
                                const E&                                    e,
                                const Str&                                  s,
                                const Rest&... r) {
    data[index] = Pair(e, sgl::string_view<CharT>(s));
    if constexpr (sizeof...(r) != 0) {
      enum_map_impl2(data, index + 1, r...);
    }
  }

  template <typename E, typename Str, typename... Rest>
  constexpr auto enum_map_impl1(const E& e, const Str& s, const Rest&... r) {
    Array<Pair<E, sgl::string_view<typename get_char_type<const Str&>::type>>,
          sizeof...(Rest) / 2 + 1>
        data;
    enum_map_impl2(data, 0, e, s, r...);
    return EnumMap(data);
  }

  template <typename Str1, typename... Strs>
  constexpr size_t biggest_str_len(size_t max = 0) {
    auto s = (get_string_size<Str1>::value > max) ? get_string_size<Str1>::value : max;
    if constexpr (sizeof...(Strs) == 0) {
      return s;
    } else {
      return biggest_str_len<Strs...>(s);
    }
  }

  template <typename... EnumStringArgs>
  constexpr auto enum_map(const EnumStringArgs&... args) {
    if constexpr (check_template_params<decltype(args)...>()) {
      return enum_map_impl1(args...);
    } else {
      static_assert(check_template_params<decltype(args)...>(),
                    "parameters must be in following form: "
                    "Enum::val1, \"value 1\", Enum::val2, "
                    "\"value 2\", ...");
    }
  }
} // namespace sgl
#endif /* SGL_IMPL_ENUM_MAP_IMPL_HPP */
