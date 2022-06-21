/**
 * @file sgl_named_tuple.hpp
 * @author Pelé Constam (you@domain.com)
 * @brief code taken from
 * https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/.
 * all credits to the compile time wizard
 * @version 0.1
 * @date 2022-06-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SGL_NAMED_TUPLE_HPP
#define SGL_NAMED_TUPLE_HPP
#include "sgl_named_tuple_index.hpp"
#include "sgl_string_view.hpp"
#include "sgl_tuple.hpp"

namespace sgl {
  template <typename Name, typename T>
  struct Arg;

  template <typename T, char... chars>
  struct Arg<Name<chars...>, T> {
    using value_type = T;
    using name_type = Name<chars...>;
    static constexpr bool nothrow_copy_constructible = std::is_nothrow_copy_constructible_v<T>;
    static constexpr bool nothrow_move_constructible = std::is_nothrow_move_constructible_v<T>;
    static constexpr bool nothrow_default_constructible =
        std::is_nothrow_default_constructible_v<T>;

    constexpr Arg() noexcept(nothrow_default_constructible) = default;

    constexpr Arg(const Arg& other) noexcept(nothrow_copy_constructible) : value_(other.value_) {}

    constexpr Arg(Arg&& other) noexcept(nothrow_move_constructible)
        : value_(std::move(other.value_)) {}

    constexpr Arg(name_type, const value_type& value) noexcept(nothrow_copy_constructible)
        : value_(value) {}

    constexpr Arg(name_type, value_type&& value) noexcept(nothrow_move_constructible)
        : value_(std::move(value)) {}

    T value_;
  };
  template <typename T, char... chars>
  Arg(Name<chars...>, T&&) -> Arg<Name<chars...>, std::decay_t<T>>;
  template <typename NameList, typename TypeList>
  class named_tuple;

  template <typename Name, typename Tuple>
  struct type_for;

  template <typename Name, typename... Names, typename... Ts>
  struct type_for<Name, named_tuple<type_list<Names...>, type_list<Ts...>>> {
    using name_list_t = type_list<Names...>;
    using type_list_t = type_list<Ts...>;
    using type = type_at_t<index_of_v<Name, name_list_t>, type_list_t>;
  };

  template <typename Name, typename Tuple>
  using type_for_t = typename type_for<Name, Tuple>::type;

  template <typename... Names, typename... Ts>
  class named_tuple<type_list<Names...>, type_list<Ts...>> : Arg<Names, Ts>... {
  public:
    /// true if all Ts are nothrow default constructible
    static constexpr bool nothrow_default_constructible =
        (std::is_nothrow_default_constructible_v<Arg<Names, Ts>> && ...);
    /// true if all Ts are nothrow destructible
    static constexpr bool nothrow_destructible =
        (std::is_nothrow_destructible_v<Arg<Names, Ts>> && ...);
    /// true if all Ts are nothrow copy constructible
    static constexpr bool nothrow_copy_constructible =
        (std::is_nothrow_copy_constructible_v<Arg<Names, Ts>> && ...);
    /// true if all Ts are nothrow move constructible
    static constexpr bool nothrow_move_constructible =
        (std::is_nothrow_move_constructible_v<Arg<Names, Ts>> && ...);
    using name_list_t = type_list<Names...>;
    using type_list_t = type_list<Ts...>;
    using types_t = type_list<Arg<Names, Ts>...>;
    using This_t = named_tuple<type_list<Names...>, type_list<Ts...>>;

    constexpr named_tuple() noexcept(nothrow_default_constructible) = default;

    constexpr named_tuple(const named_tuple&) noexcept(nothrow_copy_constructible) = default;

    constexpr named_tuple(named_tuple&&) noexcept(nothrow_move_constructible) = default;

    constexpr named_tuple(const Ts&... elems) noexcept(nothrow_copy_constructible);

    constexpr named_tuple(Ts&&... elems) noexcept(nothrow_move_constructible);

    constexpr named_tuple(const Arg<Names, Ts>&... elems) noexcept(nothrow_copy_constructible);

    constexpr named_tuple(Arg<Names, Ts>&&... elems) noexcept(nothrow_move_constructible);

    template <typename Name>
    type_for_t<Name, This_t>& get(Name name) noexcept;

    template <typename Name>
    const type_for_t<Name, This_t>& get(Name name) const noexcept;

    template <size_t I>
    auto& get() noexcept;
    template <size_t I>
    const auto& get() const noexcept;

    template <typename Name>
    type_for_t<Name, This_t>& operator[](Name) {
      static_assert(sgl::contains_v<Name, name_list_t>, "");
      return static_cast<Arg<Name, type_for_t<Name, This_t>>*>(this)->value_;
    }

    template <typename Name>
    const type_for<Name, This_t>& operator[](Name) const {
      static_assert(sgl::contains_v<Name, name_list_t>, "");
      return static_cast<const Arg<Name, type_for_t<Name, This_t>>*>(this)->value_;
    }
  };
  
  template <typename... Names, typename... Ts>
  named_tuple(Arg<Names, Ts>&&... elems) -> named_tuple<type_list<Names...>, type_list<Ts...>>;

  template <typename... Names, typename... Ts>
  named_tuple(const Arg<Names, Ts>&... elems) -> named_tuple<type_list<Names...>, type_list<Ts...>>;

  template<size_t I, typename NameList,typename TypeList>
auto& get(named_tuple<NameList,TypeList>& t)noexcept {
  return t.template get<I>();
}
template<size_t I, typename NameList,typename TypeList>
const auto& get(const named_tuple<NameList,TypeList>& t)noexcept{
  return t.template get<I>();
}
} // namespace sgl

#include "impl/sgl_named_tuple_impl.hpp"
#endif