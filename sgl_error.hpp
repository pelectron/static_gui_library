#ifndef SGL_ERROR_HPP
#define SGL_ERROR_HPP
namespace sgl {
  enum class error {
    no_error = 0,
    invalid_input,
    null_page,
    null_element,
    string_conversion,
    not_editable,
    invalid_value,
    edit_finished,
    format_error,
    page_not_found,
    invalid_page_index,
  };
}
#endif