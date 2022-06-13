#ifndef SGL_TUPLE_UTILS_HPP
#define SGL_TUPLE_UTILS_HPP
namespace sgl {
  ///@cond
  constexpr size_t sgt_mod(size_t v, size_t mod) {
    size_t m = v % mod;

    return (m == 0) ? v : v + (mod - m);
  }

  static_assert(sgt_mod(8, 2) == 8, "");
  static_assert(sgt_mod(8, 3) == 9, "");

  template <typename T>
  constexpr T max(T a, T b) {
    return a < b ? b : a;
  }

  template <typename T, typename... Ts>
  constexpr size_t max_align_impl(size_t a) {
    if constexpr (sizeof...(Ts) == 0)
      return max(a, alignof(T));
    else
      return max_align_impl<Ts...>(max(a, alignof(T)));
  }
  template <typename... Ts>
  constexpr size_t max_align() {
    return max_align_impl<Ts...>(0);
  }
  template <typename T>
  constexpr size_t tuple_size_impl(size_t, size_t) {
    return sizeof(T);
  }
  template <typename T1, typename T2, typename... Ts>
  constexpr size_t tuple_size_impl(size_t s, size_t max_align) {
    if constexpr (sizeof...(Ts) == 0) {
      return sgt_mod(sgt_mod(s + sizeof(T1), alignof(T2)) + sizeof(T2), max_align);
    } else
      return tuple_size_impl<T2, Ts...>(sgt_mod(s + sizeof(T1), alignof(T2)), max_align);
  }

  template <typename... Ts>
  constexpr size_t tuple_size() {
    return tuple_size_impl<Ts...>(0, max_align<Ts...>());
  }

  template <size_t, typename T>
  constexpr size_t buffer_index_impl(size_t) {
    return 0;
  }

  template <size_t I, typename T1, typename T2, typename... Ts>
  constexpr size_t buffer_index_impl(size_t i) {
    if constexpr (I == 0) {
      return i;
    } else if constexpr (I == 1) {
      return sgt_mod(i + sizeof(T1), alignof(T2));
    } else {
      i = sgt_mod(i + sizeof(T1), alignof(T2));
      return buffer_index_impl<I - 1, T2, Ts...>(i);
    }
  }
  template <size_t I, typename... Ts>
  constexpr size_t buffer_index() {
    return buffer_index_impl<I, Ts...>(0);
  }
  static_assert(buffer_index<0, int, char, int>() == 0, "");
  ///@endcond
} // namespace sgl
#endif