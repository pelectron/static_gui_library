#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP
#include <cstddef>
namespace sgl {
  template <typename CharT>
  constexpr size_t str_len(const CharT* str) {
    size_t i = 0;
    while (str[i] != CharT{0}) {
      ++i;
    }
    return i;
  }
  template <typename CharT>
  class basic_string_view {
  public:
    constexpr basic_string_view() = default;

    constexpr basic_string_view(basic_string_view&& other)
        : data_(other.data_), size_(other.size_){};

    constexpr basic_string_view(const basic_string_view& other)
        : data_(other.data_), size_(other.size_){};

    constexpr basic_string_view(const CharT* str, size_t size)
        : data_(str), size_(size) {}
    constexpr basic_string_view(const CharT* str)
        : data_(str), size_(str_len(str)) {}
    template <size_t N>
    constexpr basic_string_view(const CharT (&str)[N])
        : data_(str), size_(N - 1) {}
    template <typename StrLike>
    constexpr basic_string_view(const StrLike& str)
        : data_(str.data()), size_(str.size()) {}
    constexpr const CharT*       data() const noexcept { return data_; }
    constexpr CharT*             begin() noexcept { return data_; }
    constexpr const CharT*       begin() const noexcept { return data_; }
    constexpr CharT*             end() noexcept { return data_ + size_; }
    constexpr const CharT*       end() const noexcept { return data_ + size_; }
    constexpr size_t             size() const noexcept { return size_; }
    constexpr basic_string_view& operator=(const basic_string_view& other) {
      data_ = other.data_;
      size_ = other.size_;
      return *this;
    }
    constexpr const CharT& operator[](size_t i) const { return data_[i]; }

  private:
    const CharT* data_{nullptr};
    size_t       size_{0};
  };
  template <typename CharT>
  constexpr bool operator==(const basic_string_view<CharT> s1,
                            const basic_string_view<CharT> s2) {
    if (s1.size() != s2.size())
      return false;
    for (size_t i = 0; i < s1.size(); ++i) {
      if (s1[i] != s2[i])
        return false;
    }
    return true;
  }

  constexpr sgl::basic_string_view<char> operator"" _sv(const char* str,
                                                        size_t      n) {
    return basic_string_view<char>(str, n);
  }
  constexpr sgl::basic_string_view<char16_t> operator"" _sv(const char16_t* str,
                                                            size_t          n) {
    return basic_string_view<char16_t>(str, n);
  }
  constexpr sgl::basic_string_view<char32_t> operator"" _sv(const char32_t* str,
                                                            size_t          n) {
    return basic_string_view<char32_t>(str, n);
  }
} // namespace sgl
#endif