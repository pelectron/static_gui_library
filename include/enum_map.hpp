#ifndef SGL_ENUM_MAP_HPP
#define SGL_ENUM_MAP_HPP
#include "sgl_limits.hpp"
#include "sgl_string_view.hpp"
#include <type_traits>

namespace sgl {
  template <typename T1, typename T2>
  struct Pair {
    using first_type = T1;
    using second_type = T2;
    constexpr Pair() = default;
    constexpr Pair(const Pair& other) : first(other.first), second(other.second) {}
    constexpr Pair(Pair&& other) : first(std::move(other.first)), second(std::move(other.second)) {}
    constexpr Pair(const T1& t1, const T2& t2) : first(t1), second(t2) {}
    constexpr Pair(T1&& t1, T2&& t2) : first(std::move(t1)), second(std::move(t2)) {}
    constexpr Pair& operator=(const Pair& other) {
      first = other.first;
      second = other.second;
      return *this;
    }
    constexpr Pair& operator=(Pair&& other) {
      first = move(other.first);
      second = move(other.second);
      return *this;
    }
    T1 first{};
    T2 second{};
  };
  template <typename T1, typename T2>
  Pair(T1&&, T2&&) -> Pair<std::decay_t<T1>, std::decay_t<T2>>;

  template <typename T, size_t N>
  struct array {
  public:
    constexpr T*       begin() { return data; }
    constexpr const T* begin() const { return data; }
    constexpr T*       end() { return data + N; }
    constexpr const T* end() const { return data + N; }
    constexpr size_t   size() const { return N; }
    constexpr T&       operator[](size_t i) { return data[i]; }
    constexpr const T& operator[](size_t i) const { return data[i]; }

    T data[N];
  };
  /// deduction guide for array
  /// \{
  template <typename T, size_t N>
  array(const T (&)[N]) -> array<T, N>;

  template <class T, class... U>
  array(T, U...) -> array<T, 1 + sizeof...(U)>;
  /// \}

  /**
   * \brief This class maps values of type T to string_view<CharT>.
   * \details Internally, it just contains an sgl::array of Pair<E,sgl::String_view>charT>> and
   * provides a simple interface for it. To construct a EnumMap, either use the SGL_ENUM_MAP macro
   * or do it like this:
   *
   * \code
   *  enum class myEnum{
   *   val1,
   *   val2,
   *   // ...
   *  };
   *  static constexpr auto m = EnumMap(array{{Pair{myEnum::val1, "val1"},
   *                                               {myEnum::val2,"val2"},...}});
   * 
   *  \endcode
   *
   *  The curly braces and named types are necessary to deduce the enum type
   * correctly, as braced initializer lists have no type. Alternatively, use the SGL_ENUM_MAP macro
   * like below:
   *
   * \code
   *
   * static constexpr auto m = SGL_ENUM_MAP({myEnum::val1, "val1"}, {myEnum::val2, "val2"}, ...);
   *
   * \endcode
   *
   * \tparam E enum type
   * \tparam N number of elements
   * \tparam CharT character type
   */
  template <typename E, size_t N, typename CharT>
  struct EnumMap {
    constexpr EnumMap(const EnumMap& other) noexcept;

    constexpr EnumMap(EnumMap&& other) noexcept;

    constexpr EnumMap(const array<Pair<E, sgl::string_view<CharT>>, N>& map) noexcept;

    constexpr EnumMap(const array<Pair<E, const CharT*>, N>& map) noexcept;

    constexpr E                       operator[](sgl::string_view<CharT> str) const noexcept;
    constexpr sgl::string_view<CharT> operator[](E value) const noexcept;
    constexpr bool                    contains(E value) const noexcept;
    constexpr bool                    contains(sgl::string_view<CharT> string) const noexcept;
    constexpr size_t                  index_of(E value) const noexcept;
    constexpr size_t                  index_of(sgl::string_view<CharT> string) const noexcept;
    constexpr sgl::string_view<CharT> get_view(size_t i) const noexcept;
    constexpr E                       get_value(size_t i) const noexcept;

  private:
    array<Pair<E, sgl::string_view<CharT>>, N> data{};
  };

  /**
   * \brief This macro can be used to construct an EnumMap declaratively.
   * \details You can use this macro with at least one element, i.e a pair in curly braces. If your
   * compiler does not support the GNU-extension of pasting a zero length variadic macro without
   * printing a ',', use SGL_ENUM_MAP1 for defining ENumMaps with one element.
   *
   * \code
   * enum class myEnum{
   *  val1,
   *  val2,
   *  // ...
   * };
   * static constexpr auto m = SGL_ENUM_MAP({myEnum::val1, "val1"}, {myEnum::val2, "val2"}, ...);
   *
   * \endcode
   * \note The curly braces must be place as above, else it will not work.
   */

#define SGL_ENUM_MAP(value, string, ...) \
  sgl::EnumMap(sgl::array{{sgl::Pair value, string, ##__VA_ARGS__}})

/**
 * \brief This macro can be used to construct an EnumMap with one element.
 *
 * \details An example of it is below:
 *
 * \code
 *    enum class myEnum{
 *  val1,
 *  val2,
 *  // ...
 * };
 * static constexpr auto m = SGL_ENUM_MAP1({myEnum::val2, "val2"});
 * \endcode
 * \note The curly braces MUST be placed like above, i.e. '{value' and '"string"}'.
 */
#define SGL_ENUM_MAP1(value, string) sgl::EnumMap(sgl::array{sgl::Pair{value, string}})
} // namespace sgl
#include "impl/sgl_enum_map_impl.hpp"
#endif