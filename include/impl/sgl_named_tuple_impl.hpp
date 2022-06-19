#ifndef SGL_NAMED_TUPLE_IMPL_HPP
#define SGL_NAMED_TUPLE_IMPL_HPP
#include "../sgl_named_tuple.hpp"
namespace sgl {

  template <const auto&... Names, typename... Ts>
  constexpr named_tuple<value_list<Names...>, type_list<Ts...>>::named_tuple(
      const Ts&... elems) noexcept(nothrow_copy_constructible)
      : data(named_value<Names, Ts>{elems}...) {}

  template <const auto&... Names, typename... Ts>
  constexpr named_tuple<value_list<Names...>, type_list<Ts...>>::named_tuple(
      Ts&&... elems) noexcept(nothrow_move_constructible)
      : data(named_value<Names, Ts>{std::move(elems)}...) {}

  template <const auto&... Names, typename... Ts>
  template <const auto& Name>
  auto& named_tuple<value_list<Names...>, type_list<Ts...>>::get() noexcept {
    constexpr size_t Index = index_of_value_v<Name, values_t>;
    if constexpr (Index == numeric_limits<size_t>::max()) {
      static_assert(value_list_impl::always_false<Index>,
                    "There is no element with this name stored in this tuple.");
    } else {
      return sgl::get<Index>(data).value;
    }
  }

  template <const auto&... Names, typename... Ts>
  template <const auto& Name>
  const auto& named_tuple<value_list<Names...>, type_list<Ts...>>::get() const noexcept {
    constexpr size_t Index = index_of_value_v<Name, values_t>;
    if constexpr (Index == numeric_limits<size_t>::max()) {
      static_assert(value_list_impl::always_false<Index>,
                    "There is no element with this name stored in this tuple.");
    } else {
      return sgl::get<Index>(data).value;
    }
  }

  template <const auto&... Names, typename... Ts>
  template <size_t I>
  auto& named_tuple<value_list<Names...>, type_list<Ts...>>::get() noexcept {
    if constexpr (I >= sizeof...(Ts)) {
      static_assert(value_list_impl::always_false<I>, "Index out of range.");
    } else {
      return sgl::get<I>(data).value;
    }
  }

  template <const auto&... Names, typename... Ts>
  template <size_t I>
  const auto& named_tuple<value_list<Names...>, type_list<Ts...>>::get() const noexcept {
    if constexpr (I >= sizeof...(Ts)) {
      static_assert(value_list_impl::always_false<I>, "Index out of range.");

    } else {
      return sgl::get<I>(data).value;
    }
  }

} // namespace sgl
#endif