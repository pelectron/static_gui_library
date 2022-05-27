#ifndef TUPLE_HPP
#define TUPLE_HPP
#include "sgl_index_sequence.hpp"
#include "sgl_tuple_utils.hpp"
#include "sgl_type_list.hpp"

namespace sgl {
  template <typename... Types>
  class tuple {
  public:
    /// typelist of the types in this tuple
    using types_t = type_list<Types...>;
    /// index sequence for types in this tuple
    using indices_t = index_sequence_for<Types...>;
    /// template alias for I-th type in this tuple. 0 based index.
    template <size_t I>
    using type_at = type_at_t<I, types_t>;

    /// true if all Types are nothrow default constructible
    static constexpr bool nothrow_default_constructible =
        (std::is_nothrow_default_constructible_v<Types> && ...);
    /// true if all Types are nothrow destructible
    static constexpr bool nothrow_destructible =
        (std::is_nothrow_destructible_v<Types> && ...);
    /// true if all types are nothrow copy constructible
    static constexpr bool nothrow_copy_constructible =
        (std::is_nothrow_copy_constructible_v<Types> && ...);
    /// true if all Types are nothrow move constructible
    static constexpr bool nothrow_move_constructible =
        (std::is_nothrow_move_constructible_v<Types> && ...);

    /// default construct a tuple
    tuple() noexcept(nothrow_default_constructible) {
      default_construct(indices_t{});
    }

    /**
     * @brief construct a new tuple from values
     * @param values values
     */
    tuple(Types&&... values) noexcept(
        ((std::is_rvalue_reference_v<decltype(values)>
              ? std::is_nothrow_move_constructible_v<Types>
              : std::is_nothrow_copy_constructible_v<Types>)&&...)) {
      emplace_all(indices_t{}, std::forward<Types>(values)...);
    }

    /// move constructor
    tuple(tuple&& other) noexcept(nothrow_move_constructible) {
      move_impl(std::move(other), indices_t{});
    }

    /// copy constructor
    tuple(const tuple& other) noexcept(nothrow_copy_constructible) {
      copy_impl(other, indices_t{});
    }

    /// destructor
    ~tuple() noexcept(nothrow_destructible) { destroy(); }

    /**
     * @brief get access to I-th element in tuple. 0 based index.
     * @tparam I index
     * @return reference to type at index I
     */
    template <size_t I>
    type_at<I>& get() noexcept {
      static_assert(I < sizeof...(Types), "index out of range");
      return *reinterpret_cast<type_at<I>*>(buffer +
                                            buffer_index<I, Types...>());
    }

    /**
     * @brief get const access to I-th element in tuple. 0 based index.
     * @tparam I index
     * @return const reference to type at index I
     */
    template <size_t I>
    const type_at<I>& get() const noexcept {
      static_assert(I < sizeof...(Types), "index out of range");
      return *reinterpret_cast<const type_at<I>*>(buffer +
                                                  buffer_index<I, Types...>());
    }

    /**
     * @brief emplace I-th element. Destroys current stored I-th element before
     * emplacing new value.
     * @tparam I element index
     * @tparam Args argument types of I-th element
     * @param args constructor arguments
     */
    template <size_t I, typename... Args>
    void emplace(Args&&... args) noexcept(
        noexcept(type_at<I>(std::forward<Args>(args)...))) {
      static_assert(I < sizeof...(Types), "index out of range");
      destroy_impl(sgl::index_seq_t<I>{});
      new (buffer + buffer_index<I, Types...>())
          type_at<I>(std::forward<Args>(args)...);
    }

    /**
     * @brief applies f for each element in the tuple.
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F>
    void for_each(F&& f) noexcept(noexcept(f)) {
      for_each_impl(std::forward<F>(f), sgl::index_sequence_for<Types...>{});
    }

    /**
     * @brief applies f for each element in the tuple.
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F>
    void for_each(F&& f) const noexcept(noexcept(f)) {
      for_each_impl(std::forward<F>(f), sgl::index_sequence_for<Types...>{});
    }

  private:
    template <typename F, size_t... I>
    void for_each_impl(F&& f, sgl::index_seq_t<I...>) noexcept(noexcept(f)) {
      static_assert(std::is_invocable_v < F, type_at<I>&);
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (f(this->get<I>()), ...);
    }

    template <typename F, size_t... I>
    void for_each_impl(F&& f, sgl::index_seq_t<I...>) const
        noexcept(noexcept(f)) {
      static_assert(std::is_invocable_v < F, const type_at<I>&);
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (f(this->get<I>()), ...);
    }

    template <size_t... I>
    void
        copy_impl(const tuple& t,
                  sgl::index_seq_t<I...>) noexcept(nothrow_copy_constructible) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (emplace<I>(t.get<I>()), ...);
    }

    template <size_t... I>
    void
        move_impl(tuple&& t,
                  sgl::index_seq_t<I...>) noexcept(nothrow_move_constructible) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (emplace<I>(std::move(t.get<I>())), ...);
    }

    template <size_t... I>
    void default_construct(sgl::index_seq_t<I...>) noexcept(
        nothrow_default_constructible) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (new (buffer + buffer_index<I, Types...>()) type_at<I>(), ...);
    }

    void destroy() noexcept(nothrow_destructible) { destroy_impl(indices_t{}); }

    template <size_t... I>
    void destroy_impl(sgl::index_seq_t<I...>) noexcept(nothrow_destructible) {
      (reinterpret_cast<type_at<I>*>(buffer + buffer_index<I, Types...>())
           ->~type_at<I>(),
       ...);
    }

    template <size_t... I>
    void emplace_all(sgl::index_seq_t<I...>, Types&&... values) noexcept(
        ((std::is_rvalue_reference_v<decltype(values)>
              ? std::is_nothrow_move_constructible_v<Types>
              : std::is_nothrow_copy_constructible_v<Types>)&&...)) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (new (buffer + buffer_index<I, Types...>())
           type_at<I>(std::forward<Types>(values)),
       ...);
    }

    alignas(max_align<Types...>()) char buffer[tuple_size<Types...>()]{0};
  };
} // namespace sgl
#endif