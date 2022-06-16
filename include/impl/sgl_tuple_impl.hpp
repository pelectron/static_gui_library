#ifndef SGL_TUPLE_UTILS_HPP
#define SGL_TUPLE_UTILS_HPP

#include "../sgl_index_sequence.hpp"
#include "../sgl_type_list.hpp"
#include "../sgl_type_traits.hpp"


namespace sgl {
  /// \cond
  template <size_t I, typename T>
  class tuple_element {
  public:
    using value_type = T;
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
    constexpr typename elem_at_t<I>::value_type& get_elem() noexcept {
      return (*static_cast<elem_at_t<I>*>(this)).get();
    }

    template <size_t I>
    constexpr const typename elem_at_t<I>::value_type& get_elem() const noexcept {
      return (*static_cast<const elem_at_t<I>*>(this)).get();
    }
  };
  /// \endcond
} // namespace sgl
#endif