#ifndef SGL_FORMAT_HPP
#define SGL_FORMAT_HPP
#include "sgl_error.hpp"
#include <cstdint>




namespace sgl {

  enum class format_t { integer, floating, exponential, fixed, hex };
  
  template <typename CharT, typename T>
  sgl::error format(CharT* str, size_t len, T value, uint32_t precision, format_t format) noexcept;
  
} // namespace sgl

#include "impl/sgl_format_impl.hpp"
#endif