//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_TYPE_LIST_HPP
#define SGL_TYPE_LIST_HPP
#include <type_traits>

namespace sgl {
  ///\cond
  template <typename... Ts>
  struct type_list {};

  template <typename>
  struct head;

  template <typename T, typename... Ts>
  struct head<type_list<T, Ts...>> {
    using type = T;
  };

  template <typename List>
  using head_t = typename head<List>::type;
  template <typename List>
  struct tail;

  template <typename T, typename... Ts>
  struct tail<type_list<T, Ts...>> {
    using type = type_list<Ts...>;
  };

  template <typename List>
  using tail_t = typename tail<List>::type;

  template <typename List>
  struct list_size {
    static constexpr size_t value = 0;
  };

  template <typename... Ts>
  struct list_size<type_list<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
  };

  template <typename List>
  inline constexpr size_t list_size_v = list_size<List>::value;

  template <typename T, typename T1, typename... Ts>
  constexpr size_t index_of_impl(size_t i = 0) {
    if constexpr (std::is_same_v<T, T1>) {
      return i;
    }
    if constexpr (sizeof...(Ts) == 0) {
      return static_cast<size_t>(-1);
    } else {
      return index_of_impl<T, Ts...>(i + 1);
    }
  }
  template <typename T, typename List>
  struct index_of;

  template <typename T, typename... Ts>
  struct index_of<T, type_list<Ts...>> {
    static constexpr size_t value = index_of_impl<T, Ts...>();
  };

  template <typename T, typename List>
  inline constexpr size_t index_of_v = index_of<T, List>::value;
  static_assert(index_of_v<int, type_list<int, double, char>> == 0);

  template <typename T, typename List>
  struct push_front;

  template <typename T, typename... Ts>
  struct push_front<T, type_list<Ts...>> {
    using type = type_list<T, Ts...>;
  };

  template <typename T, typename List>
  using push_front_t = typename push_front<T, List>::type;

  /// \brief pop the first type from the list and return the shortened list.
  /// \example pop_front<type_list<int,char>>::type == type_list<char>
  /// \tparam List list pop front of.
  template <typename List>
  struct pop_front;

  template <typename T, typename... Ts>
  struct pop_front<type_list<T, Ts...>> {
    using type = type_list<Ts...>;
  };

  template <>
  struct pop_front<type_list<>> {
    using type = type_list<>;
  };

  template <typename List>
  using pop_front_t = typename pop_front<List>::type;
  static_assert(std::is_same_v<type_list<char, double>, pop_front_t<type_list<int, char, double>>>);

  /// \brief get N-th type in type_list List
  ///
  /// \tparam N index of type
  /// \tparam List
  template <size_t N, typename List>
  struct type_at;

  template <size_t N>
  struct index_out_of_range {
    static_assert(N != 0, "index out of range");
  };
  template <size_t, typename...>
  struct type_at_impl;

  template <size_t N, typename T, typename... Ts>
  struct type_at_impl<N, T, Ts...> : type_at_impl<N - 1, Ts...> {};

  template <typename T, typename... Ts>
  struct type_at_impl<0, T, Ts...> {
    using type = T;
  };

  template <size_t N>
  struct type_at_impl<N> {
    static_assert(N < 0, "type index out of range");
  };

  template <size_t N, typename... Ts>
  struct type_at<N, type_list<Ts...>> {
    using type = typename type_at_impl<N, Ts...>::type;
  };

  template <size_t N, typename List>
  using type_at_t = typename type_at<N, List>::type;

  static_assert(std::is_same_v<type_at_t<0, type_list<int, double, char>>, int>);
  static_assert(std::is_same_v<type_at_t<1, type_list<int, double, char>>, double>);
  static_assert(std::is_same_v<type_at_t<2, type_list<int, double, char>>, char>);
  template <typename List>
  using first_t = head_t<List>;
  template <typename List>
  using last_t = type_at_t<list_size_v<List> - 1, List>;

  /// \brief add T to end of List
  /// \example push_back<double, type_list<int,char>>::type == type_list<int,
  /// char, double>
  /// \tparam List list pop front of.
  template <typename T, typename List>
  struct push_back;

  template <typename T, typename... Ts>
  struct push_back<T, type_list<Ts...>> {
    using type = type_list<Ts..., T>;
  };

  /// \brief append T to List, like vector::push_back
  /// \example push_back_t<double, type_list<int,char>> ==
  /// type_list<int,char,double>
  /// \tparam T
  /// \tparam List
  template <typename T, typename List>
  using push_back_t = typename push_back<T, List>::type;

  static_assert(
      std::is_same_v<push_back_t<double, type_list<int, char>>, type_list<int, char, double>>);

  template <typename List, size_t I>
  struct pop_back_impl;

  template <typename T, typename... Ts, size_t I>
  struct pop_back_impl<type_list<T, Ts...>, I> {
    using type = push_front_t<T, typename pop_back_impl<type_list<Ts...>, I - 1>::type>;
  };

  template <typename T>
  struct pop_back_impl<type_list<T>, 1> {
    using type = type_list<>;
  };

  template <typename List>
  struct pop_back;

  template <typename... Ts>
  struct pop_back<type_list<Ts...>> {
    using type = typename pop_back_impl<type_list<Ts...>, sizeof...(Ts)>::type;
  };

  /// \brief pop last element of list and return the new list
  ///
  /// \tparam List
  template <typename List>
  using pop_back_t = typename pop_back<List>::type;

  template <template <typename...> typename F, typename List>
  struct apply;

  template <template <typename...> typename F, typename... Ts>
  struct apply<F, type_list<Ts...>> {
    using type = F<Ts...>;
  };

  template <template <typename> typename F, typename T>
  struct apply<F, type_list<T>> {
    using type = F<T>;
  };

  template <template <typename...> typename F, typename List>
  using apply_t = typename apply<F, List>::type;

  template <typename>
  struct extract;

  template <template <typename...> typename Class, typename... Ts>
  struct extract<Class<Ts...>> {
    using type = type_list<Ts...>;
  };

  template <template <typename> typename F, typename List>
  struct for_each_in_list;

  template <template <typename> typename F, typename... Ts>
  struct for_each_in_list<F, type_list<Ts...>> {
    using type = type_list<apply_t<F, type_list<Ts>>...>;
  };

  template <template <typename> typename F, typename List>
  using for_each_t = typename for_each_in_list<F, List>::type;

  template <typename T, typename List>
  struct contains;

  template <typename T, typename... Ts>
  struct contains<T, type_list<Ts...>> {
    static constexpr bool value = (std::is_same_v<T, Ts> || ...);
  };

  /// @brief checks if T is contained in List
  /// @tparam T type to search
  /// @tparam List sgl::type_list
  template <typename T, typename List>
  inline constexpr bool contains_v = contains<T, List>::value;

  template <typename... Ts>
  struct all_unique;

  template <typename T>
  struct all_unique<T> : std::true_type {};

  template <typename T, typename... Rest>
  struct all_unique<T, Rest...> {
    static constexpr bool value = ((!std::is_same_v<T, Rest>)&&...) && all_unique<Rest...>::value;
  };

  /// @brief check if every type in \a Ts is unique
  /// \details For example all_unique_v<char,int,double> == true,
  /// but all_unique_v<char,int,char>==false.
  /// @tparam ...Ts
  template <typename... Ts>
  inline constexpr bool all_unique_v = all_unique<Ts...>::value;

  ///\endcond
} // namespace sgl
#endif /* SGL_TYPE_LIST_HPP */
