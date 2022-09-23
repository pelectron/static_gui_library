#ifndef SGL_FORMAT_HPP
#define SGL_FORMAT_HPP
#include "sgl/error.hpp"

#include <cstdint>

namespace sgl {

  /// \brief format type enum
  enum class format_t { integer, floating, exponential, fixed, hex };

  /// \brief This class represent a format result. It contains an sgl::error value and the number of
  /// characters formatted.
  struct format_result {
    sgl::error ec;   ///< conversion error result
    size_t     size; ///< number of characters written
  };

  /// \brief format value into str. Leaves str unchanged if there is a format error.
  /// \tparam CharT character type
  /// \tparam T value type
  /// \param str character buffer to format into
  /// \param len size of character buffer
  /// \param value value to format
  /// \param precision formatting precision
  /// \param format formatting type.
  /// \return sgl::format_result
  template <typename CharT, typename T>
  sgl::format_result
      format(CharT* str, size_t len, T value, uint32_t precision, format_t format) noexcept;

} // namespace sgl

#include "sgl/impl/format_impl.hpp"
#endif