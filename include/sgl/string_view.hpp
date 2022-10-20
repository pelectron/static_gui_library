//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_STRING_VIEW_HPP
#define SGL_STRING_VIEW_HPP
#include "sgl/limits.hpp"

#include <cstddef>

namespace sgl {

  /// \headerfile string_view.hpp "sgl/string_view.hpp"
  /// \brief A non owning 'view' of CharT elements. See std::string_view for an explanation of the
  /// concept if it is unclear. \tparam CharT character type of the string_view
  template <typename CharT>
  class string_view {
  public:
    /// default ctor
    constexpr string_view() noexcept = default;

    /// move ctor
    /// \param other stirng_view to move from
    constexpr string_view(string_view&& other) noexcept : data_(other.data_), size_(other.size_) {}

    /// copy ctor
    /// \param other string_view to copy
    constexpr string_view(const string_view& other) noexcept
        : data_(other.data_), size_(other.size_){};

    /// construct from pointer to array an size
    /// \param str pointer to array of characters
    /// \param size number of characters in str
    constexpr string_view(const CharT* str, size_t size) noexcept : data_(str), size_(size) {}

    /// construct from pointer to array. str MUST be null terminated.
    /// \param str array to convert to view
    explicit constexpr string_view(const CharT* str) noexcept : data_(str) {
      for (size_t i = 0;; ++i) {
        if ((str[i] == '\0') || (i == sgl::numeric_limits<size_t>::max())) {
          size_ = i;
          break;
        }
      }
    }

    /// construct from array/literal. Assumes str is null terminated
    /// \param str array/literal to create view of
    template <size_t N>
    explicit constexpr string_view(const CharT (&str)[N]) noexcept : data_(str), size_(N - 1) {}

    /// construct from string like type, i.e. must have a data() and a size()
    /// function.
    /// \param str string to view
    template <typename StrLike>
    explicit constexpr string_view(const StrLike& str) noexcept
        : data_(str.data()), size_(str.size()) {}

    /// use default constructor for empty string view
    string_view(std::nullptr_t, size_t) = delete;

    /// get raw pointer from view
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* data() const noexcept { return data_; }

    /// get pointer to beginning of the view
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* begin() noexcept { return data_; }

    /// get pointer to beginning of the view
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* begin() const noexcept { return data_; }

    /// get pointer to end of the view, i.e begin() + size().
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* end() noexcept { return data_ + size_; }

    /// get pointer to end of the view, i.e begin() + size().
    /// \return const CharT*
    [[nodiscard]] constexpr const CharT* end() const noexcept { return data_ + size_; }

    /// get number of elements in view
    /// \return size_t
    [[nodiscard]] constexpr size_t size() const noexcept { return size_; }

    /// assignment operator
    /// \param other string view to copy
    /// \return string_view&
    constexpr string_view& operator=(const string_view& other) noexcept = default;

    /// index operator, undefined behaviour if i >= size().
    /// \param i index
    /// \return const CharT&
    [[nodiscard]] constexpr const CharT& operator[](size_t i) const noexcept { return data_[i]; }

  private:
    const CharT* data_{nullptr};
    size_t       size_{0};
  };

  /// \brief lexicographical comparison operator
  /// \tparam CharT character type
  /// \param s1 first view
  /// \param s2 second view
  /// \return true lexicographically equal
  /// \return false lexicographically not equal
  template <typename CharT>
  constexpr bool operator==(const string_view<CharT> s1, const string_view<CharT> s2) noexcept {
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

  /// \brief this namespace contains sgl::string_view literal operators
  namespace string_view_literals {
    /// string_view<char> literal operator
    /// \param str pointer to literal
    /// \param n literal size
    /// \return sgl::string_view<char>
    constexpr sgl::string_view<char> operator"" _sv(const char* str, size_t n) noexcept {
      return {str, n};
    }
    /// string_view<char16_t> literal operator
    /// \param str pointer to literal
    /// \param n literal size
    /// \return sgl::string_view<char16_t>
    constexpr sgl::string_view<char16_t> operator"" _sv16(const char16_t* str, size_t n) noexcept {
      return {str, n};
    }
    /// string_view<char32_t> literal operator
    /// \param str pointer to literal
    /// \param n literal size
    /// \return sgl::string_view<char32_t>
    constexpr sgl::string_view<char32_t> operator"" _sv32(const char32_t* str, size_t n) noexcept {
      return {str, n};
    }
  } // namespace string_view_literals
} // namespace sgl
#endif /* SGL_STRING_VIEW_HPP */
