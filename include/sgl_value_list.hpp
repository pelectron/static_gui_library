#ifndef SGL_VALUE_LIST_HPP
#define SGL_VALUE_LIST_HPP
#include "sgl_limits.hpp"

namespace sgl {
  template <const auto&... values>
  struct value_list {};

  namespace value_list_impl {
    template <auto... vs>
    static constexpr bool always_false = false;
    template <size_t I, const auto& v, const auto&... vs>
    constexpr const auto& get_value(value_list<v, vs...>) {
      if constexpr (I == 0) {
        return v;
      } else {
        return value_list_impl::get_value<I - 1>(value_list<vs...>{});
      }
    }
  }; // namespace value_list_impl

  template <size_t I, const auto&... vs>
  constexpr const auto& value_at(value_list<vs...> list) noexcept {
    if constexpr (I < sizeof...(vs)) {
      value_list_impl::get_value<I>(list);
    } else {
      static_assert(value_list_impl::always_false<vs...>, "index out of range");
    }
  }

  template <const auto& Value, size_t I = 0, const auto& First, const auto&... Rest>
  constexpr size_t index_of_value_impl(value_list<First, Rest...>) {
    if constexpr (Value == First)
      return I;
    else {
      if constexpr (sizeof...(Rest) == 0) {
        return numeric_limits<size_t>::max();
      } else {
        return index_of_value_impl<Value, I + 1>(value_list<Rest...>{});
      }
    }
  };

  template <const auto& Value, typename List>
  struct index_of_value;
  template <const auto& Value, const auto&... vs>
  struct index_of_value<Value, value_list<vs...>> {
    static constexpr size_t value = index_of_value_impl<Value>(value_list<vs...>{});
  };

  template <const auto& Value, typename List>
  static constexpr size_t index_of_value_v = index_of_value<Value, List>::value;
} // namespace sgl
#endif