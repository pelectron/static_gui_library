#ifndef SGL_TUPLE_HPP
#define SGL_TUPLE_HPP
#include "sgl_tuple_impl.hpp"

namespace sgl {
  /**
   * @brief This class is a tuple, analogous to std::tuple.
   * @details 
   * @tparam Ts types of the tuple.
   */
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
    /// type of the I'th element
    template <size_t I>
    using type_at_t = sgl::type_at_t<I, sgl::type_list<Ts...>>;

    constexpr tuple() noexcept(nothrow_default_constructible) = default;
    constexpr tuple(tuple&&) noexcept(nothrow_move_constructible) = default;
    constexpr tuple(const tuple&) noexcept(nothrow_copy_constructible) = default;
    constexpr tuple(const Ts&... ts) noexcept(nothrow_copy_constructible) : Base(ts...) {}
    constexpr tuple(Ts&&... ts) noexcept(nothrow_move_constructible) : Base(sgl::move(ts)...) {}

    template <size_t I>
    constexpr type_at_t<I>& get() noexcept {
      return this->template get_elem<I>();
    };

    template <size_t I>
    constexpr const type_at_t<I>& get() const noexcept {
      return this->template get_elem<I>();
    };

    template <typename F>
    constexpr void for_each(F&& f) noexcept(noexcept(f)) {
      for_each_impl(sgl::forward<F>(f), indices_t{});
    }

    template <typename F>
    constexpr void for_each(F&& f) const noexcept(noexcept(f)) {
      for_each_impl(sgl::forward<F>(f), indices_t{});
    }

  private:
    template <typename F, size_t... I>
    constexpr void for_each_impl(F&& f, sgl::index_seq_t<I...>) noexcept(noexcept(f)) {
      (forward<F>(f)(this->get<I>()), ...);
    }

    template <typename F, size_t... I>
    constexpr void for_each_impl(F&& f, sgl::index_seq_t<I...>) const noexcept(noexcept(f)) {
      (forward<F>(f)(this->get<I>()), ...);
    }
  };

  /// deduction guide for tuple
  template <typename... Ts>
  tuple(Ts&&...) -> tuple<sgl::decay_t<Ts>...>;

  /// @cond
  template <size_t I, typename tuple>
  struct tuple_element_type;

  template <size_t I, typename... Ts>
  struct tuple_element_type<I, sgl::tuple<Ts...>> {
    using type = sgl::type_at_t<I, sgl::type_list<Ts...>>;
  };
  /// @endcond

  /// get i-th type of tuple
  template <size_t I, typename tuple>
  using tuple_element_t = typename tuple_element_type<I, tuple>::type;

  /// get a reference to the tuple's I'th element. Analogous to std::get.
  template <size_t I, typename... Ts>
  constexpr tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t) noexcept {
    return t.template get<I>();
  }

  /// get a reference to the tuple's I'th element. Analogous to std::get.
  template <size_t I, typename... Ts>
  constexpr const tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t) noexcept {
    return t.template get<I>();
  }

  /// Apply f on each element in t
  template <typename F, typename... Ts>
  constexpr void for_each(tuple<Ts...>& t, F&& f) noexcept {
    t.for_each(sgl::forward<F>(f));
  }

  /// Apply f on each element in t
  template <typename F, typename... Ts>
  constexpr void for_each(const tuple<Ts...>& t, F&& f) noexcept {
    t.for_each(sgl::forward<F>(f));
  }
} // namespace sgl
#endif