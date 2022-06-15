#ifndef SGL_STATIC_STRING_HPP
#define SGL_STATIC_STRING_HPP
#include "sgl_smallest_type.hpp"
#include "sgl_string_view.hpp"
#include "sgl_type_traits.hpp"


namespace sgl {

  template <typename CharT, size_t Capacity>
  class static_string {
  public:
    /// create empty static string
    constexpr static_string() noexcept = default;
    constexpr explicit static_string(size_t size, CharT val) noexcept : size_(size) {
      for (size_t i = 0; (i < size) and (i < Capacity); ++i) {
        data_[i] = val;
      }
    }
    /// copy ctor
    constexpr static_string(const static_string& other) noexcept : size_(other.size_) {
      overwrite(other.data_, size_);
    }

    /// construct from array
    constexpr static_string(const CharT (&string)[Capacity + 1]) noexcept : size_(Capacity) {
      overwrite(string, size_);
    }

    /// construct from array which is smaller than declared types capacity
    template <size_t N, enable_if_t<(N <= (Capacity + 1))>* = nullptr>
    constexpr static_string(const CharT (&string)[N]) noexcept : size_(N - 1) {
      overwrite(string, size_);
    }

    /// construct from sgl::string_view
    constexpr static_string(const string_view<CharT> sv) noexcept
        : static_string(sv.data(), sv.size()) {}

    /// construct from pointer ans size
    constexpr static_string(const CharT* str, size_t size) noexcept { overwrite(str, size); }

    /// conversion operator to string_view
    constexpr operator string_view<CharT>() noexcept { return string_view<CharT>{data_, size_}; }

    /// get size of static string, i.e. number of characters.
    constexpr size_t size() const noexcept { return size_; }

    /// get maximum size of static string.
    constexpr size_t capacity() const noexcept { return Capacity; }

    /// get pointer to beginning of static string.
    constexpr CharT* data() noexcept { return data_; }

    /// get const pointer to beginning of static string.
    constexpr const CharT* data() const noexcept { return data_; }

    /// get pointer to beginning of static string.
    constexpr CharT* begin() noexcept { return data_; }

    /// get pointer to beginning of static string.
    constexpr const CharT* begin() const noexcept { return data_; }

    /// get pointer to end of static string.
    constexpr const CharT* end() const noexcept { return data_ + size_; }

    /// append str to this. Characters from str which do not fit are discarded.
    constexpr void append(const CharT* str, size_t n) noexcept {
      for (size_t i = size_; (i < Capacity) and (n != 0); ++i, --n) {
        data_[i] = str[i];
        ++size_;
      }
    }

    /// append string_view to this. Characters from str which do not fit are
    /// discarded.
    constexpr void append(string_view<CharT> str) noexcept { append(str.data(), str.size()); }

    /// append char to this, if there is capacity left.
    constexpr void append(CharT c) noexcept {
      if ((size_ + 1) < Capacity) {
        data_[size_] = c;
        ++size_;
      }
    }

    /// check if string is empty
    constexpr bool is_empty() const noexcept { return size_ == 0; }

    /// copy assignment operator
    constexpr static_string& operator=(const static_string& other) noexcept {
      this->overwrite(other.data_, other.size_);
      for (size_t i = size_; i <= Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }

    /// assign from string_view
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
    constexpr CharT& operator[](size_t i) noexcept { return data_[i]; }

    /// const access to i-th character. Undefined behaviour if i > Capacity.
    constexpr const CharT& operator[](size_t i) const noexcept { return data_[i]; }

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

  /// static string comparison operator.
  template <typename CharT, size_t N>
  constexpr bool operator==(const static_string<CharT, N> s1,
                            const static_string<CharT, N> s2) noexcept {
    if (s1.size() != s2.size())
      return false;
    for (size_t i = 0; i < s1.size(); ++i) {
      if (s1[i] != s2[i])
        return false;
    }
    return true;
  }
} // namespace sgl
#endif