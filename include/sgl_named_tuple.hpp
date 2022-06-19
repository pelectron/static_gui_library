#ifndef SGL_NAMED_TUPLE_HPP
#define SGL_NAMED_TUPLE_HPP
#include "sgl_string_view.hpp"
#include "sgl_tuple.hpp"
#include "sgl_value_list.hpp"

namespace sgl {

  template <const auto& Name, typename T>
  struct named_value {
    T value;
  };

  template <typename T>
  static constexpr bool is_named_value_wrapped_v = false;
  template <const auto& n, typename T>
  static constexpr bool is_named_value_wrapped_v<named_value<n, T>> = true;

  template <const auto& Name, typename T>
  constexpr named_value<Name, T> make_named(T&& t) {
    return named_value<Name, T>{std::forward<T>(t)};
  }

  template <const auto& Name, typename T, typename... Args>
  constexpr named_value<Name, T> make_named(Args&&... args) {
    return named_value<Name, T>{{std::forward<Args>(args)...}};
  }

  template <typename NameList, typename TypeList>
  class named_tuple;
  template <const auto& N1>
  constexpr bool name_check(value_list<N1>) {
    return true;
  }

  template <const auto& N1, const auto& N2, const auto&... Ns>
  constexpr bool name_check(value_list<N1, N2, Ns...>) {
    if constexpr (sizeof...(Ns) == 0) {
      return not(N1 == N2);
    } else {
      return (not(N1 == N2)) and name_check(value_list<N2, Ns...>{});
    }
  }

  template <const auto&... Names, typename... Ts>
  class named_tuple<value_list<Names...>, type_list<Ts...>> {
  public:
    /// true if all Ts are nothrow default constructible
    static constexpr bool nothrow_default_constructible =
        std::is_nothrow_default_constructible_v<tuple<named_value<Names, Ts>...>>;
    /// true if all Ts are nothrow destructible
    static constexpr bool nothrow_destructible =
        std::is_nothrow_destructible_v<tuple<named_value<Names, Ts>...>>;
    /// true if all Ts are nothrow copy constructible
    static constexpr bool nothrow_copy_constructible =
        std::is_nothrow_copy_constructible_v<tuple<named_value<Names, Ts>...>>;
    /// true if all Ts are nothrow move constructible
    static constexpr bool nothrow_move_constructible =
        std::is_nothrow_move_constructible_v<tuple<named_value<Names, Ts>...>>;
    using values_t = value_list<Names...>;
    using types_t = type_list<named_value<Names, Ts>...>;
    static_assert(name_check(values_t{}), "duplicate names found!");

    constexpr named_tuple() noexcept(nothrow_default_constructible) = default;

    constexpr named_tuple(const named_tuple&) noexcept(nothrow_copy_constructible) = default;

    constexpr named_tuple(named_tuple&&) noexcept(nothrow_move_constructible) = default;

    constexpr named_tuple(const Ts&... elems) noexcept(nothrow_copy_constructible);

    constexpr named_tuple(Ts&&... elems) noexcept(nothrow_move_constructible);

    template <const auto& Name>
    auto& get() noexcept;

    template <const auto& Name>
    const auto& get() const noexcept;

    template <size_t I>
    auto& get() noexcept;

    template <size_t I>
    const auto& get() const noexcept;

  private:
    tuple<named_value<Names, Ts>...> data;
  };

  template <const auto&... Names, typename... Ts>
  constexpr auto make_named_tuple(Ts&&... elements) {
    using value_list_t = value_list<Names...>;
    using type_list_t = type_list<std::decay_t<Ts>...>;
    using Tuple = named_tuple<value_list_t, type_list_t>;
    static_assert(sizeof...(Names) == sizeof...(Ts),
                  "number of names and number of elements must match");
    return Tuple(std::forward<Ts>(elements)...);
  }

  template <size_t I, const auto&... Names, typename... Ts>
  auto& get(named_tuple<value_list<Names...>, type_list<Ts...>>& t) {
    return t.template get<I>();
  }
  template <size_t I, const auto&... Names, typename... Ts>
  const auto& get(named_tuple<value_list<Names...>, type_list<Ts...>>& t) {
    return t.template get<I>();
  }
  template <const auto& Name, const auto&... Names, typename... Ts>
  auto& get(named_tuple<value_list<Names...>, type_list<Ts...>>& t) {
    return t.template get<Name>();
  }
  template <const auto& Name, const auto&... Names, typename... Ts>
  const auto& get(named_tuple<value_list<Names...>, type_list<Ts...>>& t) {
    return t.template get<Name>();
  }

} // namespace sgl

#include "impl/sgl_named_tuple_impl.hpp"
#endif