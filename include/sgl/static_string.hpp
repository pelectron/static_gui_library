//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_STATIC_STRING_HPP
#define SGL_STATIC_STRING_HPP
#include "sgl/smallest_type.hpp"
#include "sgl/string_view.hpp"

#include <type_traits>

namespace sgl {

  /// @headerfile static_string.hpp "sgl/static_string.hpp"

  /**
    A simple, non allocating string class. Internally, it holds the size of the string and
    an array of CharT with size Capacity + 1. This way, the string is always null terminated.

    @tparam CharT character type
    @tparam Capacity maximum size of the string.
   */
  template <typename CharT, size_t Capacity>
  class static_string {
  public:
    /// create empty static string
    constexpr static_string() noexcept = default;

    /**
      construct with size times the character val
      @param size number of times to repeat val
      @param val value to repeat
     */
    constexpr explicit static_string(size_t size, CharT val) noexcept;
    /**
      copy ctor
      @param other string to copy
     */
    constexpr static_string(const static_string& other) = default;

    /**
      move ctor
      @param other string to move
     */
    constexpr static_string(static_string&& other) = default;

    /**
      construct from array/string literal
      @param string string literal to construct from
     */
    template <size_t ArraySize = Capacity + 1>
    explicit constexpr static_string(const CharT (&string)[ArraySize]) noexcept;

    /**
      construct from sgl::string_view
      @param sv string view to copy
     */
    explicit constexpr static_string(string_view<CharT> sv) noexcept;

    /**
      construct from pointer to CharT array and size
      @param str pointer to array
      @param size size of string
     */
    constexpr static_string(const CharT* str, size_t size) noexcept;
    /**
      conversion operator to string_view
      @return sgl::string_view<CharT>
     */
    constexpr explicit operator string_view<CharT>() const noexcept;

    /**
      get size of static string, i.e. number of characters.
      @return size_t
     */
    [[nodiscard]] constexpr size_t size() const noexcept;
    /**
      get maximum size of static string.
      @return size_t
     */
    [[nodiscard]] constexpr size_t capacity() const noexcept;
    /**
      get pointer to beginning of static string. Same as data().
      @return CharT*
     */
    [[nodiscard]] constexpr CharT* c_str() noexcept;

    /**
      get const pointer to beginning of static string. Same as data()
      @return const CharT*
     */
    [[nodiscard]] constexpr const CharT* c_str() const noexcept;
    /**
      get pointer to beginning of static string.
      @return CharT*
     */
    [[nodiscard]] constexpr CharT* data() noexcept;
    /**
      get const pointer to beginning of static string.
      @return const CharT*
     */
    [[nodiscard]] constexpr const CharT* data() const noexcept;

    /**
      get pointer to beginning of static string.
      @return CharT*
     */
    [[nodiscard]] constexpr CharT* begin() noexcept;

    /**
      get pointer to beginning of static string.
      @return const CharT*
     */
    [[nodiscard]] constexpr const CharT* begin() const noexcept;

    /**
      get pointer to end of static string.
      @return const CharT*
     */
    [[nodiscard]] constexpr const CharT* end() const noexcept;

    /**
      append str to this. Characters from str which do not fit are discarded.
      @param str pointer to array to append
      @param n length of str to append
      @return numer of characters appended
     */
    constexpr size_t append(const CharT* str, size_t Size) noexcept;
    /**
      append string_view to this. Characters from str which do not fit are
      discarded.
      @param str string to append
     */
    constexpr void append(string_view<CharT> str) noexcept;

    /**
      append character, if there is capacity left.
      @param c character to append
     */
    constexpr void append(CharT c) noexcept;

    /**
      check if string is empty
      @return bool
     */
    [[nodiscard]] constexpr bool is_empty() const noexcept;

    /**
      copy assignment operator
      @param other string to copy
      @return reference to this
     */
    constexpr static_string& operator=(const static_string& other) noexcept;

    /**
      assign from string_view
      @param str string to copy
      @return reference to this
     */
    constexpr static_string& operator=(const string_view<CharT> str) noexcept;

    /**
      reset/clear the string.
     */
    constexpr void reset() noexcept;

    /**
      resize the string.
      @param new_size new size of string. Sets all new characters to zero.
     */
    constexpr void resize(size_t Sizeew_size) noexcept;

    /**
      access the i-th character. Undefined behaviour if i > Capacity.
      @param i index
      @return CharT&
     */
    [[nodiscard]] constexpr CharT& operator[](size_t i) noexcept;

    /**
      const access to i-th character. Undefined behaviour if i > Capacity.
      @param i index
      @return const CharT&
     */
    [[nodiscard]] constexpr const CharT& operator[](size_t i) const noexcept;

  private:
    constexpr void overwrite(const CharT* str, size_t Size) noexcept;

    CharT                          data_[Capacity + 1]{0}; ///< holds the characters.
    sgl::smallest_type_t<Capacity> size_{0};               ///< size of the string.
  };

  /// @cond
  template <typename CharT, size_t Size>
  static_string(const CharT (&string)[Size]) -> static_string<CharT, Size - 1>;

  /// @endcond

  /**
    compares s1 and s2 lexicographically and returns true if they are equal
    @tparam CharT character type
    @tparam N1 capacity of s1
    @tparam N2 capacity of s2
    @param s1 string 1
    @param s2 string 2
    @return bool
   */
  template <typename CharT, size_t Size1, size_t Size2>
  constexpr bool operator==(const static_string<CharT, Size1> s1,
                            const static_string<CharT, Size2> s2) noexcept;
} // namespace sgl

#include "sgl/impl/static_string_impl.hpp"
#endif /* SGL_STATIC_STRING_HPP */
