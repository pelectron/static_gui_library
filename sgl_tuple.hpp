#ifndef TUPLE_HPP
#define TUPLE_HPP
#include "sgl_index_sequence.hpp"
#include "sgl_tuple_utils.hpp"
#include "sgl_type_list.hpp"

namespace sgl {
  template <typename... Types>
  class tuple {
  public:
    using types_t = type_list<Types...>;
    using indices_t = index_sequence_for<Types...>;
    template <size_t I>
    using type_at = type_at_t<I, types_t>;

    tuple() { default_construct(indices_t{}); }

    tuple(Types&&... values) {
      emplace_all(indices_t{}, std::forward<Types>(values)...);
    }

    tuple(tuple&& other) { move_impl(std::move(other), indices_t{}); }

    tuple(const tuple& other) { copy_impl(other, indices_t{}); }

    ~tuple() { destroy(); }

    template <size_t I>
    type_at<I>& get() {
      static_assert(I < sizeof...(Types), "index out of range");
      return *reinterpret_cast<type_at<I>*>(buffer +
                                            buffer_index<I, Types...>());
    }

    template <size_t I>
    const type_at<I>& get() const {
      static_assert(I < sizeof...(Types), "index out of range");
      return *reinterpret_cast<const type_at<I>*>(buffer +
                                                  buffer_index<I, Types...>());
    }

    template <size_t I, typename... Args>
    void emplace(Args&&... args) {
      static_assert(I < sizeof...(Types), "index out of range");
      new (buffer + buffer_index<I, Types...>())
          type_at<I>(std::forward<Args>(args)...);
    }

    /**
     * @brief applies f for each element in the tuple.
     *
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F>
    void for_each(F&& f) {
      for_each_impl(std::forward<F>(f), sgl::index_sequence_for<Types...>{});
    }

    template <typename F>
    void for_each(F&& f) const {
      for_each_impl(std::forward<F>(f), sgl::index_sequence_for<Types...>{});
    }

  private:
    template <typename F, size_t... I>
    void for_each_impl(F&& f, sgl::index_seq_t<I...>) {
      static_assert(std::is_invocable_v < F, type_at<I>&);
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (f(this->get<I>()), ...);
    }

    template <typename F, size_t... I>
    void for_each_impl(F&& f, sgl::index_seq_t<I...>) const {
      static_assert(std::is_invocable_v < F, const type_at<I>&);
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (f(this->get<I>()), ...);
    }

    template <size_t... I>
    void copy_impl(const tuple& t, sgl::index_seq_t<I...>) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (emplace<I>(t.get<I>()), ...);
    }

    template <size_t... I>
    void move_impl(tuple&& t, sgl::index_seq_t<I...>) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (emplace<I>(std::move(t.get<I>())), ...);
    }

    template <size_t... I>
    void default_construct(sgl::index_seq_t<I...>) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (new (buffer + buffer_index<I, Types...>()) type_at<I>(), ...);
    }

    void destroy() { destroy_impl(indices_t{}); }

    template <size_t... I>
    void destroy_impl(sgl::index_seq_t<I...>) {
      (reinterpret_cast<type_at<I>*>(buffer + buffer_index<I, Types...>())
           ->~type_at<I>(),
       ...);
    }

    template <size_t... I>
    void emplace_all(sgl::index_seq_t<I...>, Types&&... types) {
      static_assert(((I < sizeof...(Types)) && ...), "index out of range");
      (new (buffer + buffer_index<I, Types...>())
           type_at<I>(std::forward<Types>(types)),
       ...);
    }

    alignas(max_align<Types...>()) char buffer[tuple_size<Types...>()]{0};
  };
} // namespace sgl
#endif