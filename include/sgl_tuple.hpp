#ifndef SGL_TUPLE_HPP
#define SGL_TUPLE_HPP
#include "sgl_index_sequence.hpp"
#include "sgl_traits.hpp"
#include "sgl_tuple_utils.hpp"
#include "sgl_type_list.hpp"

namespace sgl {
  template <size_t I, typename T>
  class tuple_element {
  public:
    constexpr tuple_element() noexcept(sgl::is_nothrow_default_constructible_v<T>) = default;
    constexpr tuple_element(const T& arg) noexcept(sgl::is_nothrow_copy_constructible_v<T>)
        : value(arg) {}
    constexpr tuple_element(T&& arg) noexcept(sgl::is_nothrow_move_constructible_v<T>)
        : value(sgl::move(arg)) {}
    constexpr tuple_element(const tuple_element&) noexcept(
        sgl::is_nothrow_copy_constructible_v<T>) = default;
    constexpr tuple_element(tuple_element&&) noexcept(sgl::is_nothrow_move_constructible_v<T>) =
        default;
    constexpr T&       get() noexcept { return value; }
    constexpr const T& get() const noexcept { return value; }

  private:
    T value;
  };

  template <typename TypeList, typename IndexSequence>
  class tuple_impl;
  template <typename... Ts, size_t... Ns>
  class tuple_impl<sgl::type_list<Ts...>, sgl::index_seq_t<Ns...>>
      : public tuple_element<Ns, Ts>... {
  public:
    /// true if all Ts are nothrow default constructible
    static constexpr bool nothrow_default_constructible =
        (sgl::is_nothrow_default_constructible_v<Ts> && ...);
    /// true if all Ts are nothrow destructible
    static constexpr bool nothrow_destructible = (sgl::is_nothrow_destructible_v<Ts> && ...);
    /// true if all Ts are nothrow copy constructible
    static constexpr bool nothrow_copy_constructible =
        (sgl::is_nothrow_copy_constructible_v<Ts> && ...);
    /// true if all Ts are nothrow move constructible
    static constexpr bool nothrow_move_constructible =
        (sgl::is_nothrow_move_constructible_v<Ts> && ...);

    template <size_t I>
    using elem_at_t = tuple_element<I, sgl::type_at_t<I, sgl::type_list<Ts...>>>;
    constexpr tuple_impl() noexcept(nothrow_default_constructible) = default;
    constexpr tuple_impl(const tuple_impl&) noexcept(nothrow_copy_constructible) = default;
    constexpr tuple_impl(tuple_impl&&) noexcept(nothrow_move_constructible) = default;

    constexpr tuple_impl(const Ts&... ts) noexcept(nothrow_copy_constructible)
        : tuple_element<Ns, Ts>(ts)... {}

    constexpr tuple_impl(Ts&&... ts) noexcept(nothrow_move_constructible)
        : tuple_element<Ns, Ts>(sgl::move(ts))... {}

    template <size_t I>
    constexpr elem_at_t<I>& get_elem() noexcept {
      return *static_cast<elem_at_t<I>*>(this);
    }

    template <size_t I>
    constexpr const elem_at_t<I>& get_elem() const noexcept {
      return *static_cast<const elem_at_t<I>*>(this);
    }
  };

  template <typename... Ts>
  class tuple : private tuple_impl<sgl::type_list<Ts...>, sgl::index_sequence_for<Ts...>> {
  private:
    using Base = tuple_impl<sgl::type_list<Ts...>, sgl::index_sequence_for<Ts...>>;
    using indices_t = sgl::index_sequence_for<Ts...>;

  public:
    /// true if all Ts are nothrow default constructible
    static constexpr bool nothrow_default_constructible =
        (sgl::is_nothrow_default_constructible_v<Ts> && ...);
    /// true if all Ts are nothrow destructible
    static constexpr bool nothrow_destructible = (sgl::is_nothrow_destructible_v<Ts> && ...);
    /// true if all Ts are nothrow copy constructible
    static constexpr bool nothrow_copy_constructible =
        (sgl::is_nothrow_copy_constructible_v<Ts> && ...);
    /// true if all Ts are nothrow move constructible
    static constexpr bool nothrow_move_constructible =
        (sgl::is_nothrow_move_constructible_v<Ts> && ...);
    template <size_t I>
    using type_at_t = sgl::type_at_t<I, sgl::type_list<Ts...>>;

    constexpr tuple() noexcept(nothrow_default_constructible) = default;
    constexpr tuple(tuple&&) noexcept(nothrow_move_constructible) = default;
    constexpr tuple(const tuple&) noexcept(nothrow_copy_constructible) = default;
    constexpr tuple(const Ts&... ts) noexcept(nothrow_copy_constructible) : Base(ts...) {}
    constexpr tuple(Ts&&... ts) noexcept(nothrow_move_constructible) : Base(sgl::move(ts)...) {}

    template <size_t I>
    constexpr type_at_t<I>& get() noexcept {
      return this->template get_elem<I>().get();
    };

    template <size_t I>
    constexpr const type_at_t<I>& get() const noexcept{
      return this->template get_elem<I>().get();
    };

    template <typename F>
    constexpr void for_each(F&& f)noexcept(noexcept(f)) {
      for_each_impl(sgl::forward<F>(f), indices_t{});
    }

    template <typename F>
    constexpr void for_each(F&& f) const noexcept(noexcept(f)){
      for_each_impl(sgl::forward<F>(f), indices_t{});
    }

  private:
    template <typename F, size_t... I>
    constexpr void for_each_impl(F&& f, sgl::index_seq_t<I...>) noexcept(noexcept(f)){
      (f(this->get<I>()), ...);
    }
    template <typename F, size_t... I>
    constexpr void for_each_impl(F&& f, sgl::index_seq_t<I...>) const noexcept(noexcept(f)){
      (f(this->get<I>()), ...);
    }
  };
  /// deduction guide for tuple
  template <typename... Ts>
  tuple(Ts&&...) -> tuple<sgl::decay_t<Ts>...>;

  template <size_t I, typename tuple>
  struct tuple_element_type;
  template <size_t I, typename... Ts>
  struct tuple_element_type<I, sgl::tuple<Ts...>> {
    using type = sgl::type_at_t<I, sgl::type_list<Ts...>>;
  };
  /// get i-th type of tuple
  template <size_t I, typename tuple>
  using tuple_element_t = typename tuple_element_type<I, tuple>::type;

  template <size_t I, typename... Ts>
  constexpr tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t) noexcept{
    return t.template get<I>();
  }

  template <size_t I, typename... Ts>
  constexpr const tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t) noexcept{
    return t.template get<I>();
  }

  template <typename F, typename... Ts>
  constexpr void for_each(tuple<Ts...>& t, F&& f) noexcept{
    t.template for_each(sgl::forward<F>(f));
  }

  template <typename F, typename... Ts>
  constexpr void for_each(const tuple<Ts...>& t, F&& f) noexcept{
    t.template for_each(sgl::forward<F>(f));
  }
} // namespace sgl
#endif