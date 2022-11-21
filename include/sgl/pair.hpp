//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_PAIR_HPP
#define SGL_PAIR_HPP
#include <type_traits>
#include <utility>

namespace sgl {
  /**
    simple pair type. It holds two public members, `first` and `second` of type T1 or T2
    respectively.
    @tparam T1 type of the first element
    @tparam T2 type of the second elment.
   */
  template <typename T1, typename T2>
  struct Pair {
    using first_type = T1;
    using second_type = T2;
    constexpr Pair() = default;

    constexpr Pair(const Pair& other) = default;

    constexpr Pair(Pair&& other) = default;

    constexpr Pair(const T1& t1, const T2& t2) : first(t1), second(t2) {}

    constexpr Pair(T1&& t1, T2&& t2) : first(std::move(t1)), second(std::move(t2)) {}

    constexpr Pair& operator=(const Pair& other) = default;

    constexpr Pair& operator=(Pair&& other) = default;

    T1 first{};
    T2 second{};
  };

  /// @cond
  template <typename T1, typename T2>
  Pair(T1&&, T2&&) -> Pair<std::decay_t<T1>, std::decay_t<T2>>;
  template <typename T1, typename T2>
  Pair(const T1&, const T2&) -> Pair<std::decay_t<const T1>, std::decay_t<const T2>>;
  /// @endcond
} // namespace sgl
#endif
