#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP
#include <cstddef>
namespace sgl {
  /**
   * @brief A 'view' of CharT elements. See std::string_view
   *
   * @tparam CharT character type of the string_view
   */
  template <typename CharT>
  class string_view {
  public:
    constexpr string_view() = default;

    constexpr string_view(string_view&& other)
        : data_(other.data_), size_(other.size_){};

    constexpr string_view(const string_view& other)
        : data_(other.data_), size_(other.size_){};

    constexpr string_view(const CharT* str, size_t size)
        : data_(str), size_(size) {}
    template <size_t N>
    constexpr string_view(const CharT (&str)[N]) : data_(str), size_(N - 1) {}
    template <typename StrLike>
    constexpr string_view(const StrLike& str)
        : data_(str.data()), size_(str.size()) {}
    constexpr const CharT* data() const noexcept { return data_; }
    constexpr CharT*       begin() noexcept { return data_; }
    constexpr const CharT* begin() const noexcept { return data_; }
    constexpr CharT*       end() noexcept { return data_ + size_; }
    constexpr const CharT* end() const noexcept { return data_ + size_; }
    constexpr size_t       size() const noexcept { return size_; }
    constexpr string_view& operator=(const string_view& other) {
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
  constexpr bool operator==(const string_view<CharT> s1,
                            const string_view<CharT> s2) {
    if (s1.size() != s2.size())
      return false;
    for (size_t i = 0; i < s1.size(); ++i) {
      if (s1[i] != s2[i])
        return false;
    }
    return true;
  }
  namespace string_view_literals{
  constexpr sgl::string_view<char> operator"" _sv(const char* str, size_t n) {
    return string_view<char>(str, n);
  }
  constexpr sgl::string_view<char16_t> operator"" _sv16(const char16_t* str,
                                                      size_t          n) {
    return string_view<char16_t>(str, n);
  }
  constexpr sgl::string_view<char32_t> operator"" _sv32(const char32_t* str,
                                                      size_t          n) {
    return string_view<char32_t>(str, n);
  }
  }
} // namespace sgl
#endif