#ifndef SGL_STRING_VIEW_IMPL_HPP
#define SGL_STRING_VIEW_IMPL_HPP
#include "sgl/string_view.hpp"

namespace sgl {

  template <typename CharT>
  constexpr string_view<CharT>::string_view(string_view&& other) noexcept
      : data_(other.data_), size_(other.size_) {}

  template <typename CharT>
  constexpr string_view<CharT>::string_view(const string_view& other) noexcept
      : data_(other.data_), size_(other.size_){};

  template <typename CharT>
  constexpr string_view<CharT>::string_view(const CharT* str, size_t size) noexcept
      : data_(str), size_(size) {}

  template <typename CharT>
  constexpr string_view<CharT>::string_view(const CharT* str) noexcept : data_(str) {
    for (size_t i = 0;; ++i) {
      if ((str[i] == static_cast<CharT>('\0')) || (i == sgl::numeric_limits<size_t>::max())) {
        size_ = i;
        break;
      }
    }
  }

  template <typename CharT>
  template <size_t N>
  constexpr string_view<CharT>::string_view(const CharT (&str)[N]) noexcept
      : data_(str), size_(N - 1) {}

  template <typename CharT>
  template <typename StrLike>
  constexpr string_view<CharT>::string_view(const StrLike& str) noexcept
      : data_(str.data()), size_(str.size()) {}

  template <typename CharT>
  [[nodiscard]] constexpr const CharT* string_view<CharT>::data() const noexcept {
    return data_;
  }

  template <typename CharT>
  [[nodiscard]] constexpr const CharT* string_view<CharT>::begin() noexcept {
    return data_;
  }

  template <typename CharT>
  [[nodiscard]] constexpr const CharT* string_view<CharT>::begin() const noexcept {
    return data_;
  }

  template <typename CharT>
  [[nodiscard]] constexpr const CharT* string_view<CharT>::end() noexcept {
    return data_ + size_;
  }

  template <typename CharT>
  [[nodiscard]] constexpr const CharT* string_view<CharT>::end() const noexcept {
    return data_ + size_;
  }

  template <typename CharT>
  [[nodiscard]] constexpr size_t string_view<CharT>::size() const noexcept {
    return size_;
  }

  template <typename CharT>
  [[nodiscard]] constexpr const CharT& string_view<CharT>::operator[](size_t i) const noexcept {
    return data_[i];
  }

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

  namespace string_view_literals {
    constexpr sgl::string_view<char> operator"" _sv(const char* str, size_t n) noexcept {
      return {str, n};
    }

    constexpr sgl::string_view<char16_t> operator"" _sv16(const char16_t* str, size_t n) noexcept {
      return {str, n};
    }

    constexpr sgl::string_view<char32_t> operator"" _sv32(const char32_t* str, size_t n) noexcept {
      return {str, n};
    }
  } // namespace string_view_literals
} // namespace sgl

#endif
