#ifndef STATIC_STRING_HPP
#define STATIC_STRING_HPP
#include "string_view.hpp"

#include <type_traits>
namespace sgl {
  template <size_t N>
  using smallest_type_t = std::conditional_t < N < 256,
        uint8_t,
        std::conditional_t <
            N<65536,
              uint16_t,
              std::conditional_t<N<4294967296, uint32_t, uint64_t>>>;

  template <typename CharT, size_t Capacity>
  class StaticString {
  public:
    using size_type = smallest_type_t<Capacity>;

    constexpr StaticString() = default;
    constexpr StaticString(const StaticString& other) : size_(other.size_) {
      overwrite(other.data_, size_);
    }
    constexpr StaticString(const CharT (&string)[Capacity + 1])
        : size_(Capacity) {
      overwrite(string, size_);
    }

    template <size_t N, typename = std::enable_if_t<(N <= (Capacity + 1))>>
    constexpr StaticString(const CharT (&string)[N]) : size_(N - 1) {
      overwrite(string, size_);
    }
    constexpr StaticString(const basic_string_view<CharT> sv)
        : StaticString(sv.data(), sv.size()) {}
    constexpr StaticString(const CharT* str, size_t size) {
      overwrite(str, size);
    }
    constexpr operator basic_string_view<CharT>() {
      return basic_string_view<CharT>{data_, size_};
    }
    constexpr size_t       size() const noexcept { return size_; }
    constexpr size_t       capacity() const noexcept { return Capacity; }
    constexpr CharT*       data() noexcept { return data_; }
    constexpr const CharT* data() const noexcept { return data_; }
    constexpr CharT*       begin() noexcept { return data_; }
    constexpr const CharT* begin() const noexcept { return data_; }
    constexpr const CharT* end() const noexcept { return data_ + size_; }
    constexpr void         append(const CharT* str, size_t n) {
      for (size_t i = size_; (i < Capacity) and (n != 0); ++i, --n) {
        data_[i] = str[i];
        ++size_;
      }
    }
    constexpr void append(CharT c) {
      if ((size_ + 1) < Capacity) {
        data_[size_] = c;
        ++size_;
      }
    }

    constexpr bool is_empty() const noexcept { return size_ == 0; }

    constexpr StaticString& operator=(const StaticString& other) {
      size_ = 0;
      for (const auto& c : other.data_) {
        data_[size_] = c;
        ++size_;
      }
      for (int i = size_; i < Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }
    constexpr StaticString& operator=(const basic_string_view<CharT> str) {
      size_ = 0;
      for (const auto& c : str) {
        data_[size_] = c;
        ++size_;
      }
      for (int i = size_; i < Capacity; ++i) {
        data_[i] = 0;
      }
      return *this;
    }
    constexpr void reset() noexcept {
      for (auto& c : data_) {
        c = 0;
      }
      size_ = 0;
    }
    constexpr void resize(size_t new_size) {
      if (size_ < new_size)
        size_ = new_size;
      for (size_t i = size_ - 1; i < new_size; --i) {
        data_[i] = 0;
      }
    }
    constexpr CharT& operator[](size_t i) { return data_[i]; }

    constexpr const CharT& operator[](size_t i) const { return data_[i]; }

  private:
    constexpr void overwrite(const CharT* str, size_t n) {
      size_t i = 0;
      size_ = 0;
      for (; (i < Capacity) and (i < n); ++i) {
        data_[i] = str[i];
        ++size_;
      }
    }
    CharT     data_[Capacity + 1] = {0};
    size_type size_{0};
  };
  template <typename CharT, size_t N>
  constexpr bool operator==(const StaticString<CharT, N> s1,
                            const StaticString<CharT, N> s2) {
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