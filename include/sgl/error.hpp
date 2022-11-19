/**
 * @file error.hpp
 * @author Pelé Constam (pelectron1602@gmail.com)
 * @brief This file defines the sgl::error enum.
 * @version 0.1
 * @date 2022-09-26
 *
 *          Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef SGL_ERROR_HPP
#define SGL_ERROR_HPP

namespace sgl {

  /// @headerfile error.hpp "sgl/error.hpp"
  /// @brief enumeration for error values.
  enum class error : int {
    no_error = 0,  //< no error occurred
    invalid_input, ///< invalid input received
    out_of_range,
    buffer_too_small,
    null_page,
    null_element,
    string_conversion,
    not_editable,
    invalid_value,
    edit_finished, ///< item is finished being edited
    format_error,
    page_not_found,
    invalid_page_index,
    invalid_format,
    null_format
  };
} // namespace sgl
#endif /* SGL_ERROR_HPP */
