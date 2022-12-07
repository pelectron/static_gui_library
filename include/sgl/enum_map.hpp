//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_ENUM_MAP_HPP
#define SGL_ENUM_MAP_HPP
#include "sgl/array.hpp"
#include "sgl/limits.hpp"
#include "sgl/pair.hpp"
#include "sgl/string_view.hpp"

#include <type_traits>

namespace sgl {

  /// @headerfile enum_map.hpp "sgl/enum_map.hpp"

  /**
    This class maps values of type T to string_view<CharT>.

    Internally, it just contains an sgl::Array of sgl::Pair<E,sgl::String_view<CharT>>
    and provides a simple interface for it. To construct a EnumMap, use the sgl::enum_map() free
    function. That function will deduced all template parameters for you.

    @tparam E enum/value type
    @tparam Size number of values this map contains.
    @tparam CharT character type
   */
  template <typename E, size_t Size, typename CharT>
  struct EnumMap {

    /// copy ctor
    /// @param other map to copy
    constexpr EnumMap(const EnumMap& other) noexcept;

    /// move ctor
    /// @param other map to move from
    constexpr EnumMap(EnumMap&& other) noexcept;

    /// construct EnumMap from Array of pairs
    /// @param map Array of pairs of E and sgl::string_view<CharT>
    constexpr explicit EnumMap(const Array<Pair<E, sgl::string_view<CharT>>, Size>& map) noexcept;

    /// construct EnumMap from Array
    /// @param map Array of pairs of E and const CharT*
    constexpr explicit EnumMap(const Array<Pair<E, const CharT*>, Size>& map) noexcept;

    /// get value from string_view. If str is not mapped to any value, any of the contained
    /// values of E might be returned.
    /// @param str stringified value
    /// @return constexpr E
    [[nodiscard]] constexpr E operator[](sgl::string_view<CharT> str) const noexcept;

    /// get string_view of value. Returns empty view if value is not mapped to any view.
    /// @param value value to get view of
    /// @return constexpr sgl::string_view<CharT>
    [[nodiscard]] constexpr sgl::string_view<CharT> operator[](E value) const noexcept;

    /// check if map contains value
    /// @param value value to check
    /// @return true contains value
    /// @return false doesn't contain value
    [[nodiscard]] constexpr bool contains(E value) const noexcept;

    /// query if map contains a value
    /// @param string stringified value
    /// @return true if this contains string.
    /// @return false if this doesn't contain string.
    [[nodiscard]] constexpr bool contains(sgl::string_view<CharT> string) const noexcept;

    /// get number of map entries
    /// @return constexpr size_t
    [[nodiscard]] constexpr size_t size() const noexcept { return Size; }

    /// get string_view of value by index
    /// @param i index
    /// @return constexpr sgl::string_view<CharT>
    [[nodiscard]] constexpr sgl::string_view<CharT> get_view(size_t i) const noexcept;

  private:
    /// get index of value in string form
    /// @param string stringified value
    /// @return constexpr size_t
    [[nodiscard]] constexpr size_t index_of(sgl::string_view<CharT> string) const noexcept;

    /// get index of value
    /// @param value value to get index of
    /// @return constexpr size_t
    [[nodiscard]] constexpr size_t index_of(E value) const noexcept;

    /// Get the value by index
    /// @param i index
    /// @return constexpr E
    [[nodiscard]] constexpr E get_value(size_t i) const noexcept;

    sgl::Array<sgl::Pair<E, sgl::string_view<CharT>>, Size> data{};
  };

  /**
    helper function to create an enum map with all template parameters deduced.

    @note args must be of the following form: MyEnum::val1, "string of val1", MyEnum::Val2,
    "string of val2", ... etc. Below is a code snippet showing how to use it:

    ```cpp
    #include "sgl/enum_map.hpp"
    enum class E{
      val1,
      val2,
      val3
     };
    constexpr auto map = sgl::enum_map(E::val1, "value 1", E::val2, "value 2", E::val3, "value3");
    ```

    @tparam EnumStringArgs should be viewed as pairs of enum values and character literals
    @param args must be in the form mentioned above and in the example code snippet, i.e. enum
    value, enum string, ...
    @return enum map with deduced parameters
   */
  template <typename... EnumStringArgs>
  constexpr auto enum_map(const EnumStringArgs&... args);

} // namespace sgl

#include "sgl/impl/enum_map_impl.hpp"
#endif /* SGL_ENUM_MAP_HPP */
