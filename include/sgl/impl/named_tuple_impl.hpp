//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_NAMED_TUPLE_IMPL_HPP
#define SGL_IMPL_NAMED_TUPLE_IMPL_HPP
#include "sgl/named_tuple.hpp"

namespace sgl {
  /// \cond
  namespace tuple_detail {
    template <auto...>
    static constexpr bool always_false = false;
    template <typename Name, typename Tuple>
    struct type_for;

    template <typename Name, typename... Names, typename... Ts>
    struct type_for<Name, NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>> {
      using name_list_t = sgl::type_list<Names...>;
      using type_list_t = sgl::type_list<Ts...>;
      static_assert(sgl::contains_v<Name, name_list_t>, "Name not found in tuple type");

      using type = type_at_t<index_of_v<Name, name_list_t>, type_list_t>;
    };

    template <typename Name, typename Tuple>
    using type_for_t = typename type_for<Name, Tuple>::type;
  } // namespace tuple_detail

  /// \endcond

  template <typename... Names, typename... Ts>
  constexpr NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::NamedTuple(
      const Ts&... elems) noexcept(nothrow_copy_constructible)
      : NamedValue<Names, Ts>{Names{}, elems}... {}

  template <typename... Names, typename... Ts>
  constexpr NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::NamedTuple(
      Ts&&... elems) noexcept(nothrow_move_constructible)
      : NamedValue<Names, Ts>{Names{}, std::move(elems)}... {}

  template <typename... Names, typename... Ts>
  constexpr NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::NamedTuple(
      const NamedValue<Names, Ts>&... elems) noexcept(nothrow_copy_constructible)
      : NamedValue<Names, Ts>{elems}... {}

  template <typename... Names, typename... Ts>
  constexpr NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::NamedTuple(
      NamedValue<Names, Ts>&&... elems) noexcept(nothrow_move_constructible)
      : NamedValue<Names, Ts>{std::move(elems)}... {}

  template <typename... Names, typename... Ts>
  template <typename Name>
  constexpr const auto&
      NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::get(Name name) const noexcept {
    if constexpr (!sgl::contains_v<Name, name_list_t>) {
      static_assert(sgl::contains_v<Name, name_list_t>, "No such name in tuple");
    } else {
      return static_cast<const NamedValue<Name, tuple_detail::type_for_t<Name, This_t>>*>(this)
          ->value();
      static_cast<void>(name);
    }
  }

  template <typename... Names, typename... Ts>
  template <typename Name>
  constexpr auto&
      NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::get(Name name) noexcept {
    if constexpr (!sgl::contains_v<Name, name_list_t>) {
      static_assert(sgl::contains_v<Name, name_list_t>, "No such name in tuple");
    } else {
      return static_cast<NamedValue<Name, tuple_detail::type_for_t<Name, This_t>>*>(this)->value();
      static_cast<void>(name);
    }
  }

  template <typename... Names, typename... Ts>
  template <size_t I>
  constexpr auto& NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::get() noexcept {
    if constexpr (I >= sizeof...(Ts)) {
      static_assert(tuple_detail::always_false<I>, "Index out of range.");
    } else {
      using Name = type_at_t<I, name_list_t>;
      using Type = type_at_t<I, type_list_t>;
      return static_cast<NamedValue<Name, Type>*>(this)->value();
    }
  }

  template <typename... Names, typename... Ts>
  template <size_t I>
  constexpr const auto&
      NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::get() const noexcept {
    if constexpr (I >= sizeof...(Ts)) {
      static_assert(tuple_detail::always_false<I>, "Index out of range.");
    } else {
      using Name = type_at_t<I, name_list_t>;
      using Type = type_at_t<I, type_list_t>;
      return static_cast<const NamedValue<Name, Type>*>(this)->value();
    }
  }

  template <typename... Names, typename... Ts>
  template <char... Cs>
  constexpr auto& NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::operator[](
      sgl::Name<Cs...> name) noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, name_list_t>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, name_list_t>, "No such name in tuple");
    } else {
      return static_cast<
                 NamedValue<sgl::Name<Cs...>, tuple_detail::type_for_t<sgl::Name<Cs...>, This_t>>*>(
                 this)
          ->value();
      static_cast<void>(name);
    }
  }

  template <typename... Names, typename... Ts>
  template <char... Cs>
  constexpr const auto& NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::operator[](
      sgl::Name<Cs...>) const noexcept {
    if constexpr (!sgl::contains_v<sgl::Name<Cs...>, name_list_t>) {
      static_assert(sgl::contains_v<sgl::Name<Cs...>, name_list_t>, "No such name in tuple");
    } else {
      return static_cast<const NamedValue<sgl::Name<Cs...>,
                                          tuple_detail::type_for_t<sgl::Name<Cs...>, This_t>>*>(
                 this)
          ->value();
    }
  }

  template <typename... Names, typename... Ts>
  template <typename F>
  constexpr void NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::for_each(
      F&& f) noexcept(nothrow_invocable_for_each<F, Ts&...>) {
    if constexpr (!(std::is_invocable_r_v<void, F, Ts&> && ...)) {
      static_assert((std::is_invocable_r_v<void, F, Ts&> && ...),
                    "f must be invocable with T& for each T in this "
                    "tuple.");
    } else {
      (std::forward<F>(f)(this->get(Names{})), ...);
    }
  }

  template <typename... Names, typename... Ts>
  template <typename F>
  constexpr void NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::for_each(F&& f) const
      noexcept(nothrow_invocable_for_each<F, const Ts&...>) {
    if constexpr (!(std::is_invocable_r_v<void, F, const Ts&> && ...)) {
      static_assert((std::is_invocable_r_v<void, F, const Ts&> && ...),
                    "f must be invocable with const T& for each T in this "
                    "tuple.");
    } else {
      (std::forward<F>(f)(this->get(Names{})), ...);
    }
  }

  template <typename... Names, typename... Ts>
  template <typename F>
  constexpr void NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::for_each_with_name(
      F&& f) noexcept((std::is_nothrow_invocable_r_v<void, F, sgl::string_view<char>, Ts&> &&
                       ...)) {
    if constexpr (!(std::is_invocable_r_v<void, F, sgl::string_view<char>, Ts&> && ...)) {
      static_assert((std::is_invocable_r_v<void, F, sgl::string_view<char>, Ts&> && ...),
                    "f must be invocable with (sgl::string_view<char>, T&) for each T in this "
                    "tuple.");
    } else {
      (std::forward<F>(f)(sgl::string_view<char>(Names{}), this->get(Names{})), ...);
    }
  }

  template <typename... Names, typename... Ts>
  template <typename F>
  constexpr void
      NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>::for_each_with_name(F&& f) const
      noexcept((std::is_nothrow_invocable_r_v<void, F, sgl::string_view<char>, const Ts&> && ...)) {
    if constexpr (!(std::is_invocable_r_v<void, F, sgl::string_view<char>, const Ts&> && ...)) {
      static_assert((std::is_invocable_r_v<void, F, sgl::string_view<char>, const Ts&> && ...),
                    "f must be invocable with (sgl::string_view<char>, const T&) for each T in "
                    "this tuple.");
    } else {
      (std::forward<F>(f)(sgl::string_view<char>(Names{}), this->get(Names{})), ...);
    }
  }

  template <size_t I, typename NameList, typename TypeList>
  constexpr auto& get(NamedTuple<NameList, TypeList>& t) noexcept {
    return t.template get<I>();
  }

  template <size_t I, typename NameList, typename TypeList>
  constexpr const auto& get(const NamedTuple<NameList, TypeList>& t) noexcept {
    return t.template get<I>();
  }

  template <typename Name, typename NameList, typename TypeList>
  constexpr auto& get(Name name, NamedTuple<NameList, TypeList>& t) noexcept {
    return t.template get(name);
  }

  template <typename Name, typename NameList, typename TypeList>
  constexpr const auto& get(Name name, const NamedTuple<NameList, TypeList>& t) noexcept {
    return t.template get(name);
  }

  template <typename F, typename NameList, typename TypeList>
  constexpr void for_each(NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(
      noexcept(std::declval<NamedTuple<NameList, TypeList>>().for_each(std::forward<F>(f)))) {
    tuple.for_each(std::forward<F>(f));
  }

  template <typename F, typename NameList, typename TypeList>
  constexpr void for_each(const NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(
      noexcept(std::declval<const NamedTuple<NameList, TypeList>>().for_each(std::forward<F>(f)))) {
    tuple.for_each(std::forward<F>(f));
  }

  template <typename F, typename NameList, typename TypeList>
  constexpr void for_each_with_name(NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(noexcept(
      std::declval<NamedTuple<NameList, TypeList>>().for_each_with_name(std::forward<F>(f)))) {
    tuple.for_each_with_name(std::forward<F>(f));
  }

  template <typename F, typename NameList, typename TypeList>
  constexpr void
      for_each_with_name(const NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(noexcept(
          std::declval<NamedTuple<NameList, TypeList>>().for_each_with_name(std::forward<F>(f)))) {
    tuple.for_each_with_name(std::forward<F>(f));
  }
} // namespace sgl
#endif /* SGL_IMPL_NAMED_TUPLE_IMPL_HPP */
