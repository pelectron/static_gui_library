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

  /// \headerfile static_string.hpp "sgl/static_string.hpp"
  /// \brief A simple, non allocating string class. Internally, it holds the size of the string and
  /// a CharT array of size Capacity + 1. This way, the string is always null terminated. \tparam
  /// CharT character type \tparam Capacity capacity of the string, excluding the null character.
  template <typename CharT, size_t Capacity>
  class static_string {
  public:
    /// create empty static string
    constexpr static_string() noexcept = default;

    /// construct with size times the character val
    /// \param size number of times to repeat val
    /// \param val value to repeat
    constexpr explicit static_string(size_t size, CharT val) noexcept : size_(size) {
      for (size_t i = 0; (i < size) and (i < Capacity); ++i) {
        data_[i] = val;
      }
    }

    /// copy ctor
    /// \param other string to copy
    constexpr static_string(const static_string& other) noexcept : size_(other.size_) {
      overwrite(other.data_, size_);
    }

    /// construct from array/string literal
    /// \param string string literal to construct from
    explicit constexpr static_string(const CharT (&string)[Capacity + 1]) noexcept
        : size_(Capacity) {
      overwrite(string, size_);
    }

    /// construct from sgl::string_view
    /// \param sv string view to copy
    explicit constexpr static_string(string_view<CharT> sv) noexcept
        : static_string(sv.data(), sv.size()) {}

    /// construct from pointer to CharT array and size
    /// \param str pointer to array
    /// \param size size of string
    constexpr static_string(const CharT* str, size_t size) noexcept { overwrite(str, size); }

    /// conversion operator to string_view
    /// \return sgl::string_view<CharT>
    constexpr explicit operator string_view<CharT>() noexcept {
      return string_view<CharT>{data_, size_};
    }

    /// get size of static string, i.e. number of characters.
    /// \return size_t
    [[nodiscard]] constexpr size_t size() const noexcept { return size_; }

    /// get maximum size of static string.
    /// \return size_t
    [[nodiscard]] constexpr size_t capacity() const noexcept { return Capacity; }

    /// get pointer to beginning of static string. Same as data().
    /// \return CharT*
    [[nodiscard]] constexpr CharT* c_str() noexcept { return data_; }

    /// get const pointer to beginning of static string. Same as data()
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data_; }

    /// get pointer to beginning of static string.
    /// \return CharT*
    [[nodiscard]] constexpr CharT* data() noexcept { return data_; }

    /// get const pointer to beginning of static string.
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* data() const noexcept { return data_; }

    /// get pointer to beginning of static string.
    /// \return CharT*
    [[nodiscard]] constexpr CharT* begin() noexcept { return data_; }

    /// get pointer to beginning of static string.
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* begin() const noexcept { return data_; }

    /// get pointer to end of static string.
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* end() const noexcept { return data_ + size_; }

    /// append str to this. Characters from str which do not fit are discarded.
    /// \param str pointer to array to append
    /// \param n length of str to append
    constexpr void append(const CharT* str, size_t n) noexcept {
      for (size_t i = size_; (i < Capacity) and (n != 0); ++i, --n) {
        data_[i] = str[i];
        ++size_;
      }
    }

    /// append string_view to this. Characters from str which do not fit are
    /// discarded.
    /// \param str string to append
    constexpr void append(string_view<CharT> str) noexcept { append(str.data(), str.size()); }

    /// append character, if there is capacity left.
    /// \param c character to append
    constexpr void append(CharT c) noexcept {
      if ((size_ + 1) < Capacity) {
        data_[size_] = c;
        ++size_;
      }
    }

    /// check if string is empty
    /// \return bool
    [[nodiscard]] constexpr bool is_empty() const noexcept { return size_ == 0; }

    /// copy assignment operator
    /// \param other string to copy
    /// \return reference to this
    constexpr static_string& operator=(const static_string& other) noexcept {
      if (this == &other) {
        return *this;
      }
      this->overwrite(other.data_, other.size_);
      for (size_t i = size_; i <= Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }

    /// assign from string_view
    /// \param str string to copy
    /// \return reference to this
    constexpr static_string& operator=(const string_view<CharT> str) noexcept {
      this->overwrite(str.data(), str.size());
      for (size_t i = size_; i <= Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }

    /// reset/clear the string.
    constexpr void reset() noexcept {
      for (auto& c : data_) {
        c = 0;
      }
      size_ = 0;
    }

    /// resize the string.
    /// \param new_size new size of string. Sets all new characters to null.
    constexpr void resize(size_t new_size) noexcept {
      if (size_ == 0) {
        size_ = new_size;
        return;
      }
      for (size_t i = size_ - 1; i >= new_size; --i) {
        data_[i] = 0;
      }
      size_ = new_size;
    }

    /// access to i-th character. Undefined behaviour if i > Capacity.
    /// \param i index
    /// \return CharT&
    [[nodiscard]] constexpr CharT& operator[](size_t i) noexcept { return data_[i]; }

    /// const access to i-th character. Undefined behaviour if i > Capacity.
    /// \param i index
    /// \return const CharT&
    [[nodiscard]] constexpr const CharT& operator[](size_t i) const noexcept { return data_[i]; }

  private:
    constexpr void overwrite(const CharT* str, size_t n) noexcept {
      size_t i = 0;
      size_ = 0;
      for (; (i < Capacity) and (i < n); ++i) {
        data_[i] = str[i];
        ++size_;
      }
    }

    CharT                          data_[Capacity + 1]{0}; ///< holds the characters.
    sgl::smallest_type_t<Capacity> size_{0};               ///< size of the string.
  };

  /// \cond
  template <typename CharT, size_t N>
  static_string(const CharT (&string)[N]) -> static_string<CharT, N - 1>;

  /// \endcond

  ///\brief compares s1 and s2 lexicographically and returns true if they are equal
  ///\tparam CharT character type
  ///\tparam N1 capacity of s1
  ///\tparam N2 capacity of s2
  ///\param s1 string 1
  ///\param s2 string 2
  ///\return bool
  template <typename CharT, size_t N1, size_t N2>
  constexpr bool operator==(const static_string<CharT, N1> s1,
                            const static_string<CharT, N2> s2) noexcept {
    if (s1.size() != s2.size()) {
      return false;
    }
    for (size_t i = 0; i < s1.size(); ++i) {
      if (s1[i] != s2[i]) {
        return false;
      }
    }
    return true;
  }
} // namespace sgl
#endif /* SGL_STATIC_STRING_HPP */
