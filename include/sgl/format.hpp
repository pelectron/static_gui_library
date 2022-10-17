#ifndef SGL_FORMAT_HPP
#define SGL_FORMAT_HPP
#include "sgl/callable.hpp"
#include "sgl/error.hpp"
#include "sgl/fix_point.hpp"

#include <cstdint>
#include <type_traits>

namespace sgl {

  /// \brief format type enum
  enum class format { integer, floating, exponential, fixed, hex };

  /// \brief This class represent a format result. It contains an sgl::error value and the number of
  /// characters formatted.
  struct format_result {
    sgl::error ec;   ///< conversion error result
    size_t     size; ///< number of characters written
  };

  template <typename CharT, typename T>
  constexpr sgl::format_result to_chars(CharT* str, size_t len, T value) noexcept;

  template <typename CharT>
  sgl::format_result to_chars(CharT*      str,
                              size_t      len,
                              float       value,
                              uint32_t    precision,
                              sgl::format format) noexcept;

  template <typename CharT>
  sgl::format_result to_chars(CharT*      str,
                              size_t      len,
                              double      value,
                              uint32_t    precision,
                              sgl::format format) noexcept;

  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*          str,
                                        size_t          len,
                                        sgl::unsigned_fixed<I, F> value,
                                        uint32_t        precision,
                                        sgl::format     format) noexcept;

  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*          str,
                                        size_t          len,
                                        sgl::signed_fixed<I, F> value,
                                        uint32_t        precision,
                                        sgl::format     format) noexcept;

  namespace cx {

    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          float       value,
                                          uint32_t    precision,
                                          sgl::format format) noexcept;

    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          double      value,
                                          uint32_t    precision,
                                          sgl::format format) noexcept;
  } // namespace cx

  template <typename T, typename CharT>
  class Formatter {
  public:
    using value_type = T;
    using char_type = CharT;
    constexpr Formatter() = default;
    constexpr Formatter(const Formatter&) = default;
    constexpr Formatter(Formatter&&) = default;
    constexpr Formatter(
        sgl::Callable<sgl::format_result(CharT*, size_t, T)> formatting_function) noexcept;

    constexpr sgl::format_result operator()(CharT* str, size_t len, T value) {
      return impl_(str, len, value);
    }

  private:
    sgl::Callable<sgl::format_result(CharT*, size_t, T)> impl_{&sgl::to_chars<CharT, T>};
  };

  template <typename T, typename CharT>
  class RealFormatter {
  public:
    using value_type = T;
    using char_type = CharT;

    constexpr RealFormatter() = default;
    constexpr RealFormatter(const RealFormatter&) = default;
    constexpr RealFormatter(RealFormatter&&) = default;
    constexpr RealFormatter(uint32_t precision, sgl::format format = sgl::format::fixed) noexcept;
    constexpr RealFormatter(
        sgl::Callable<sgl::format_result(CharT*, size_t, T)> formatting_function,
        uint32_t                                             precision = 6,
        sgl::format                                          format = sgl::format::fixed) noexcept;

    constexpr RealFormatter& set_precision(uint32_t p) noexcept { precision_ = p; }
    constexpr RealFormatter& set_format(sgl::format f) noexcept { format_ = f; }

    sgl::format_result operator()(CharT* str, size_t len, T value) {
      return impl_(str, len, value, precision, format);
    }

  private:
    uint32_t    precision_{6};
    sgl::format format_{sgl::format::fixed};
    sgl::Callable<sgl::format_result(CharT*, size_t, T, uint32_t, sgl::format)> impl_{
        &sgl::to_chars};
  };

} // namespace sgl

#include "sgl/impl/format_impl.hpp"
#endif /* SGL_FORMAT_HPP */
