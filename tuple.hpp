#ifndef TUPLE_HPP
#define TUPLE_HPP
#include "index_sequence.hpp"
#include "tuple_utils.hpp"
#include "type_list.hpp"

namespace sgl {
  template <typename... Types>
  class tuple {
  public:
    using types_t = type_list<Types...>;
    using indices_t = index_sequence_for<Types...>;

    tuple() { default_construct(indices_t{}); }
    tuple(Types&&... values) {
      emplace_all(indices_t{}, std::forward<Types>(values)...);
    }
    tuple(tuple&& other) { move_impl(std::move(other), indices_t{}); }
    tuple(const tuple& other) { copy_impl(other, indices_t{}); }
    template <size_t I>
    using type_at = type_at_t<I, types_t>;

    template <size_t I>
    type_at<I>& get() {
      return *reinterpret_cast<type_at<I>*>(buffer +
                                            buffer_index<I, Types...>());
    }
    template <size_t I>
    const type_at<I>& get() const {
      return *reinterpret_cast<const type_at<I>*>(buffer +
                                                  buffer_index<I, Types...>());
    }

    template <size_t I, typename... Args>
    void emplace(Args&&... args) {
      new (buffer + buffer_index<I, Types...>())
          type_at<I>(std::forward<Args>(args)...);
    }

  private:
    template <size_t I, typename T, typename... Ts>
    void emplace_all(T&& t, Ts&&... ts) {
      emplace<I>(std::forward<T>(t));
      if constexpr (sizeof...(Ts) != 0)
        emplace_all<I + 1, Ts...>(std::forward<Ts>(ts)...);
    }

    template <size_t... I>
    void copy_impl(const tuple& t, sgl::index_seq_t<I...>) {
      (emplace<I>(t.get<I>()), ...);
    }

    template <size_t... I>
    void move_impl(tuple&& t, sgl::index_seq_t<I...>) {
      (emplace<I>(std::move(t.get<I>())), ...);
    }

    template <size_t... I>
    void default_construct(sgl::index_seq_t<I...>) {
      (new (buffer + buffer_index<I, Types...>()) type_at<I>(), ...);
    }
    template <size_t... I>
    void emplace_all(sgl::index_seq_t<I...>, Types&&... types) {
      (new (buffer + buffer_index<I, Types...>())
           type_at<I>(std::forward<Types>(types)),
       ...);
    }
    alignas(max_align<Types...>()) char buffer[tuple_size<Types...>()]{0};
  };
} // namespace sgl
#endif