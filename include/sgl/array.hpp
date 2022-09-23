#ifndef SGL_ARRAY_HPP
#define SGL_ARRAY_HPP
namespace sgl{
  
  /// simple Array type
  template <typename T, size_t N>
  struct Array {
  public:
    constexpr T*                   begin() { return data; }
    constexpr const T*             begin() const { return data; }
    constexpr T*                   end() { return data + N; }
    constexpr const T*             end() const { return data + N; }
    [[nodiscard]] constexpr size_t size() const { return N; }
    constexpr T&                   operator[](size_t i) { return data[i]; }
    constexpr const T&             operator[](size_t i) const { return data[i]; }

    T data[N];
  };

  /// deduction guide for Array
  /// \{
  template <typename T, size_t N>
  Array(const T (&)[N]) -> Array<T, N>;

  template <class T, class... U>
  Array(T, U...) -> Array<T, 1 + sizeof...(U)>;
  /// <\}
}
#endif