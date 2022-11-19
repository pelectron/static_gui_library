//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_ARRAY_HPP
#define SGL_ARRAY_HPP
#include <cstddef>

namespace sgl {

  /// simple array type
  /// @tparam T value type
  /// @tparam N number of elements
  template <typename T, size_t N>
  struct Array {
  public:
    /// get pointer to first element.
    constexpr T* begin() { return data; }

    /// get const pointer to first element.
    constexpr const T* begin() const { return data; }

    /// get pointer to one past the last element.
    constexpr T* end() { return data + N; }

    /// get pointer to one past the last element.
    constexpr const T* end() const { return data + N; }

    /// number of elements in this array.
    [[nodiscard]] constexpr size_t size() const { return N; }

    /// get i-th element. Undefined behaviour if i >= size().
    constexpr T& operator[](size_t i) { return data[i]; }

    /// get i-th element. Undefined behaviour if i >= size().
    constexpr const T& operator[](size_t i) const { return data[i]; }

    T data[N];
  };

  /// @{
  template <typename T, size_t N>
  Array(const T (&)[N]) -> Array<T, N>;

  template <class T, class... U>
  Array(T, U...) -> Array<T, 1 + sizeof...(U)>;
  /// @}
} // namespace sgl
#endif /* SGL_ARRAY_HPP */
