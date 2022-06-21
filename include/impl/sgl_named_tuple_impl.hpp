#ifndef SGL_NAMED_TUPLE_IMPL_HPP
#define SGL_NAMED_TUPLE_IMPL_HPP
#include "../sgl_value_list.hpp"
#include "../sgl_named_tuple.hpp"
namespace sgl {

  template <typename... Names, typename... Ts>
  constexpr named_tuple<type_list<Names...>, type_list<Ts...>>::named_tuple(
      const Ts&... elems) noexcept(nothrow_copy_constructible)
      : Arg<Names, Ts>{Names{}, elems}... {}

  template <typename... Names, typename... Ts>
  constexpr named_tuple<type_list<Names...>, type_list<Ts...>>::named_tuple(Ts&&... elems) noexcept(
      nothrow_move_constructible)
      : Arg<Names, Ts>{Names{}, std::move(elems)}... {}

  template <typename... Names, typename... Ts>
  constexpr named_tuple<type_list<Names...>, type_list<Ts...>>::named_tuple(
      const Arg<Names, Ts>&... elems) noexcept(nothrow_copy_constructible)
      : Arg<Names, Ts>{elems}... {}

  template <typename... Names, typename... Ts>
  constexpr named_tuple<type_list<Names...>, type_list<Ts...>>::named_tuple(
      Arg<Names, Ts>&&... elems) noexcept(nothrow_move_constructible)
      : Arg<Names, Ts>{std::move(elems)}... {}
  template <typename... Names, typename... Ts>
  template <typename Name>
  const type_for_t<Name, named_tuple<type_list<Names...>, type_list<Ts...>>>&
      named_tuple<type_list<Names...>, type_list<Ts...>>::get(Name name) const noexcept {
    static_assert(sgl::contains_v<Name, name_list_t>, "");
    return static_cast<const Arg<Name, type_for<Name>>*>(this)->get();
  }

  template <typename... Names, typename... Ts>
  template <typename Name>
  type_for_t<Name, named_tuple<type_list<Names...>, type_list<Ts...>>>&
      named_tuple<type_list<Names...>, type_list<Ts...>>::get(Name name) noexcept {
    static_assert(sgl::contains_v<Name, name_list_t>, "");
    return static_cast<Arg<Name, type_for<Name>>*>(this)->get();
  }

  template <typename... Names, typename... Ts>
  template <size_t I>
  auto& named_tuple<type_list<Names...>, type_list<Ts...>>::get() noexcept {
    if constexpr (I >= sizeof...(Ts)) {
      static_assert(value_list_impl::always_false<I>, "Index out of range.");
    } else {
      using Name = type_at_t<I, name_list_t>;
      using Type = type_at_t<I, type_list_t>;
      return static_cast<Arg<Name, Type>*>(this)->get();
    }
  }

  template <typename... Names, typename... Ts>
  template <size_t I>
  const auto& named_tuple<type_list<Names...>, type_list<Ts...>>::get() const noexcept {
    if constexpr (I >= sizeof...(Ts)) {
      static_assert(value_list_impl::always_false<I>, "Index out of range.");
    } else {

      using Name = type_at_t<I, name_list_t>;
      using Type = type_at_t<I, type_list_t>;
      return static_cast<const Arg<Name, Type>*>(this)->get();
    }
  }

} // namespace sgl
#endif