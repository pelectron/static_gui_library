#ifndef SGL_ENUM_HPP
#define SGL_ENUM_HPP
#include "sgl_item.hpp"
namespace sgl {

  /**
   * @addtogroup item_types Item Types
   * @{
   * @brief This class implements a enumerated item, i.e. a item with only a limited amount of values.
   *
   * @tparam EnumT the enum type
   * @tparam NumEnumerators number of enumerated values
   * @tparam LineWidth number of characters per line in the menu
   * @tparam CharT character type of the item
   */
  template <typename EnumT,
            size_t NumEnumerators,
            size_t LineWidth,
            typename CharT>
  class Enum_t : public sgl::Item_t<LineWidth, CharT> {
  public:
    using string_view_t = typename sgl::Item_t<LineWidth, CharT>::string_view_t;
    struct Pair {
      EnumT         val;
      string_view_t str;
    };

    /**
     * @brief Construct a Enum_t item. See the example of how to construct without naming types explicitly.
     *
     * @param item_name name of the item
     * @param array  array of pairs of EnumT values and corresponding
     * string_view_t
     * @param start_index
     */
    Enum_t(string_view_t item_name,
           const Pair (&array)[NumEnumerators],
           size_t start_index = 0)
        : sgl::Item_t<LineWidth, CharT>(
              item_name,
              array[start_index].str,
              &Enum_t<EnumT, NumEnumerators, LineWidth, CharT>::
                  default_handle_input),
          index_(start_index) {
      for (size_t i = 0; i < NumEnumerators; ++i) {
        map[i] = array[i];
      }
    }

    constexpr size_t size() const { return NumEnumerators; }

    string_view_t    current_string() const noexcept { return map[index_].str; }
    constexpr size_t index() const { return index_; }
    constexpr void set_index(size_t index) { index_ = index % NumEnumerators; }

  private:
    static error default_handle_input(sgl::Item_t<LineWidth, CharT>& elem,
                                      sgl::Input                     input) {
      auto& enum_elem =
          static_cast<Enum_t<EnumT, NumEnumerators, LineWidth, CharT>&>(elem);
      switch (input) {
        case sgl::Input::right:
          [[fallthrough]];
        case sgl::Input::up:
          enum_elem.set_index((enum_elem.index() + 1) % enum_elem.size());
          break;
        case sgl::Input::left:
          [[fallthrough]];
        case sgl::Input::down:
          enum_elem.set_index(enum_elem.index() == 0 ? enum_elem.size() - 1
                                                     : enum_elem.index() - 1);

          break;
        default:
          break;
      }
      elem.set_text(enum_elem.current_string());
      return error::no_error;
    }

    Pair   map[NumEnumerators]{};
    size_t index_{0};
  };
  /// @}
} // namespace sgl
#endif