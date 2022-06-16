#ifndef SGL_ENUM_MAP_HPP
#define SGL_ENUM_MAP_HPP
#include "sgl_limits.hpp"
#include "sgl_string_view.hpp"
#include "sgl_type_traits.hpp"

namespace sgl {
  template <typename T1, typename T2>
  struct Pair {
    using first_type = T1;
    using second_type = T2;
    constexpr Pair() = default;
    constexpr Pair(const Pair& other) : first(other.first), second(other.second) {}
    constexpr Pair(Pair&& other) : first(move(other.first)), second(move(other.second)) {}
    constexpr Pair(const T1& t1, const T2& t2) : first(t1), second(t2) {}
    constexpr Pair(T1&& t1, T2&& t2) : first(move(t1)), second(move(t2)) {}
    constexpr Pair& operator=(const Pair& other) {
      first = other.first;
      second = other.second;
      return *this;
    }
    constexpr Pair& operator=(Pair&& other) {
      first = move(other.first);
      second = move(other.second);
      return *this;
    }
    T1 first{};
    T2 second{};
  };
  template <typename T1, typename T2>
  Pair(T1&&, T2&&) -> Pair<decay_t<T1>, decay_t<T2>>;

  template <typename T, size_t N>
  struct array {
  public:
    constexpr T*       begin() { return data; }
    constexpr const T* begin() const { return data; }
    constexpr T*       end() { return data + N; }
    constexpr const T* end() const { return data + N; }
    constexpr size_t   size() const { return N; }
    constexpr T&       operator[](size_t i) { return data[i]; }
    constexpr const T& operator[](size_t i) const { return data[i]; }

    T data[N];
  };
  template <typename T, size_t N>
  array(const T (&)[N]) -> array<T, N>;
  template <class T, class... U>
  array(T, U...) -> array<T, 1 + sizeof...(U)>;
  template <typename E, size_t N, typename CharT>
  struct EnumMap {
    constexpr EnumMap() = default;
    constexpr EnumMap(const EnumMap& other) : data(other.data) {}

    constexpr EnumMap(EnumMap&& other) : data(move(other.data)) {}

    constexpr EnumMap(const array<Pair<E, sgl::string_view<CharT>>, N>& map) : data(map) {}

    constexpr EnumMap(const array<Pair<E, const CharT*>, N>& map) {
      for (size_t i = 0; i < N; ++i) {
        data[i].first = map[i].first;
        data[i].second = sgl::string_view<CharT>(map[i].second);
      }
    }

    constexpr E operator[](sgl::string_view<CharT> str) const {
      for (const auto& elem : data) {
        if (elem.second == str)
          return elem.first;
      }
      return E{};
    }

    constexpr sgl::string_view<CharT> operator[](E value) const noexcept {
      for (const auto& elem : data) {
        if (elem.first == value)
          return elem.second;
      }
      return sgl::string_view<CharT>{};
    }
    constexpr bool contains(E value) const {
      for (const auto& elem : data) {
        if (elem.first == value)
          return true;
      }
      return false;
    }
    constexpr bool contains(sgl::string_view<CharT> string) const {
      for (const auto& elem : data) {
        if (elem.second == string)
          return true;
      }
      return false;
    }
    constexpr size_t index_of(E value) {
      size_t i{0};
      for (const auto& elem : data) {
        if (elem.first == value)
          return i;
        ++i;
      }
      return numeric_limits<size_t>::max();
    }
    constexpr size_t index_of(sgl::string_view<CharT> string) {
      size_t i{0};
      for (const auto& elem : data) {
        if (elem.second == string)
          return i;
        ++i;
      }
      return numeric_limits<size_t>::max();
    }
    constexpr sgl::string_view<CharT> get_view(size_t i) const noexcept { return data[i].second; }
    constexpr sgl::string_view<CharT> get_value(size_t i) const noexcept { return data[i].first; }

  private:
    array<Pair<E, sgl::string_view<CharT>>, N> data{};
  };

  template <typename E, size_t N, typename CharT>
  constexpr auto Map(const array<Pair<E, sgl::string_view<CharT>>, N>& data) {
    return EnumMap<E, N, CharT>(data);
  }
#define SGL_MAP(elem, ...) EnumMap(array{{Pair elem, ##__VA_ARGS__}})

} // namespace sgl
#endif