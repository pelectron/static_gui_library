#ifndef SGL_STRING_VIEW_HPP
#define SGL_STRING_VIEW_HPP
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
    /// default ctor
    constexpr string_view() noexcept = default;
    /// move ctor
    constexpr string_view(string_view&& other) noexcept : data_(other.data_), size_(other.size_){};
    /// copy ctor
    constexpr string_view(const string_view& other) noexcept
        : data_(other.data_), size_(other.size_){};
    /// construct from pointer to element an size
    constexpr string_view(const CharT* str, size_t size) noexcept : data_(str), size_(size) {}
    /// construct from array
    template <size_t N>
    constexpr string_view(const CharT (&str)[N]) noexcept : data_(str), size_(N - 1) {}
    /// construct from string like type, i.e. must have a data() and a size()
    /// function.
    template <typename StrLike>
    constexpr string_view(const StrLike& str) noexcept : data_(str.data()), size_(str.size()) {}
    /// use default constructor for empty string view
    string_view(std::nullptr_t, size_t) = delete;
    /// get raw pointer from view
    constexpr const CharT* data() const noexcept { return data_; }
    /// get pointer to beginning of the view
    constexpr const CharT* begin() noexcept { return data_; }
    /// get pointer to beginning of the view
    constexpr const CharT* begin() const noexcept { return data_; }
    /// get pointer to end of the view
    constexpr const CharT* end() noexcept { return data_ + size_; }
    /// get pointer to end of the view
    constexpr const CharT* end() const noexcept { return data_ + size_; }
    /// get number of elements in view
    constexpr size_t size() const noexcept { return size_; }
    /// assignment operator
    constexpr string_view& operator=(const string_view& other) noexcept = default;
    /// index operator, undefined behaviour if i >= size().
    constexpr const CharT& operator[](size_t i) const noexcept { return data_[i]; }

  private:
    const CharT* data_{nullptr};
    size_t       size_{0};
  };
  /// string_view comparison operator
  template <typename CharT>
  constexpr bool operator==(const string_view<CharT> s1, const string_view<CharT> s2) noexcept {
    if (s1.size() != s2.size())
      return false;
    for (size_t i = 0; i < s1.size(); ++i) {
      if (s1[i] != s2[i])
        return false;
    }
    return true;
  }
  namespace string_view_literals {
    constexpr sgl::string_view<char> operator"" _sv(const char* str, size_t n) noexcept {
      return string_view<char>(str, n);
    }
    constexpr sgl::string_view<char16_t> operator"" _sv16(const char16_t* str, size_t n) noexcept {
      return string_view<char16_t>(str, n);
    }
    constexpr sgl::string_view<char32_t> operator"" _sv32(const char32_t* str, size_t n) noexcept {
      return string_view<char32_t>(str, n);
    }
  } // namespace string_view_literals
} // namespace sgl
#endif