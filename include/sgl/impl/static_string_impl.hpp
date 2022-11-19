#ifndef SGL_STATIC_STRING_IMPL_HPP
#define SGL_STATIC_STRING_IMPL_HPP
#include "sgl/static_string.hpp"

namespace sgl{

  template <typename CharT, size_t Capacity>
    constexpr  static_string<CharT,Capacity>::static_string(size_t size, CharT val) noexcept
        : size_(size > Capacity ? Capacity : size) {
      for (size_t i = 0; (i < size); ++i) {
        data_[i] = val;
      }
    }

  template <typename CharT, size_t Capacity>
    template <size_t ArraySize>
     constexpr static_string<CharT,Capacity>::static_string(const CharT (&string)[ArraySize]) noexcept
        : size_((ArraySize - 1) < Capacity ? (ArraySize - 1) : Capacity) {
      overwrite(string, size_);
    }

  template <typename CharT, size_t Capacity>
     constexpr static_string<CharT,Capacity>::static_string(string_view<CharT> sv) noexcept
        : static_string(sv.data(), sv.size()) {}

  template <typename CharT, size_t Capacity>
    constexpr static_string<CharT,Capacity>::static_string(const CharT* str, size_t size) noexcept { overwrite(str, size); }

  template <typename CharT, size_t Capacity>
    constexpr static_string<CharT,Capacity>::operator string_view<CharT>() noexcept {
      return string_view<CharT>{data_, size_};
    }

  template <typename CharT, size_t Capacity>
    constexpr size_t static_string<CharT,Capacity>::size() const noexcept { return size_; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr size_t static_string<CharT,Capacity>::capacity() const noexcept { return Capacity; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr CharT* static_string<CharT,Capacity>::c_str() noexcept { return data_; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr const CharT* static_string<CharT,Capacity>::c_str() const noexcept { return data_; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr CharT* static_string<CharT,Capacity>::data() noexcept { return data_; }

  template <typename CharT, size_t Capacity>
  [[nodiscard]] constexpr const CharT* static_string<CharT,Capacity>::data() const noexcept { return data_; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr CharT* static_string<CharT,Capacity>::begin() noexcept { return data_; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr const CharT* static_string<CharT,Capacity>::begin() const noexcept { return data_; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr const CharT* static_string<CharT,Capacity>::end() const noexcept { return data_ + size_; }

  template <typename CharT, size_t Capacity>
    constexpr size_t static_string<CharT,Capacity>::append(const CharT* str, size_t n) noexcept {
      const auto max = (size_ + n) < Capacity ? (size_ + n) : Capacity;
      const auto old_size = size_;
      for (size_t i = size_; i < max; ++i) {
        data_[i] = str[i - old_size];
        ++size_;
      }
      return size_ - old_size;
    }

  template <typename CharT, size_t Capacity>
    constexpr void static_string<CharT,Capacity>::append(string_view<CharT> str) noexcept { append(str.data(), str.size()); }

  template <typename CharT, size_t Capacity>
    constexpr void static_string<CharT,Capacity>::append(CharT c) noexcept {
      if ((size_ + 1u) < Capacity) {
        data_[size_] = c;
        ++size_;
      }
    }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr bool static_string<CharT,Capacity>::is_empty() const noexcept { return size_ == 0; }

  template <typename CharT, size_t Capacity>
    constexpr static_string<CharT,Capacity>& static_string<CharT,Capacity>::operator=(const static_string& other) noexcept {
      if (this == &other) {
        return *this;
      }
      this->overwrite(other.data_, other.size_);
      for (size_t i = size_; i <= Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }

  template <typename CharT, size_t Capacity>
    constexpr static_string<CharT,Capacity>& static_string<CharT,Capacity>::static_string<CharT,Capacity>::operator=(const string_view<CharT> str) noexcept {
      this->overwrite(str.data(), str.size());
      for (size_t i = size_; i <= Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }

  template <typename CharT, size_t Capacity>
    constexpr void static_string<CharT,Capacity>::reset() noexcept {
      for (auto& c : data_) {
        c = 0;
      }
      size_ = 0;
    }

  template <typename CharT, size_t Capacity>
    constexpr void static_string<CharT,Capacity>::resize(size_t new_size) noexcept {
      if (size_ == 0) {
        size_ =
            static_cast<sgl::smallest_type_t<Capacity>>(new_size > Capacity ? Capacity : new_size);
        return;
      }
      // when new_size < size_, then fill with the newly unused characters to 0
      for (size_t i = size_ - 1; i >= new_size; --i) {
        data_[i] = 0;
      }
      size_ =
          static_cast<sgl::smallest_type_t<Capacity>>(new_size > Capacity ? Capacity : new_size);
    }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr CharT& static_string<CharT,Capacity>::operator[](size_t i) noexcept { return data_[i]; }

  template <typename CharT, size_t Capacity>
    [[nodiscard]] constexpr const CharT& static_string<CharT,Capacity>::operator[](size_t i) const noexcept { return data_[i]; }

  template <typename CharT, size_t Capacity>
    constexpr void static_string<CharT,Capacity>::overwrite(const CharT* str, size_t n) noexcept {
      const auto max_index = n < Capacity ? n : Capacity;
      size_ = 0;
      for (size_t i = 0; i < max_index; ++i, ++size_) {
        data_[i] = str[i];
      }
      for (size_t i = max_index; i < Capacity; ++i) {
        data_[i] = 0;
      }
    }

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
}
#endif
