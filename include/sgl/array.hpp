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
  /// @tparam Size number of elements
  template <typename T, size_t Size>
  struct Array {
  public:
    /// get pointer to first element.
    constexpr T* begin() { return data; }

    /// get const pointer to first element.
    constexpr const T* begin() const { return data; }

    /// get pointer to one past the last element.
    constexpr T* end() { return data + Size; }

    /// get pointer to one past the last element.
    constexpr const T* end() const { return data + Size; }

    /// number of elements in this array.
    [[nodiscard]] constexpr size_t size() const { return Size; }

    /// get i-th element. Undefined behaviour if i >= size().
    constexpr T& operator[](size_t i) { return data[i]; }

    /// get i-th element. Undefined behaviour if i >= size().
    constexpr const T& operator[](size_t i) const { return data[i]; }

    T data[Size];
  };

  /// @cond
  template <typename T, size_t Size>
  Array(const T (&)[Size]) -> Array<T, Size>;

  template <class T, class... U>
  Array(T, U...) -> Array<T, 1 + sizeof...(U)>;
  /// @endcond

} // namespace sgl
#endif /* SGL_ARRAY_HPP */
