#ifndef SGL_ENUM_HPP
#define SGL_ENUM_HPP
#include "sgl_item.hpp"
namespace sgl {
  template <typename T, typename CharT>
  struct Pair {
    T                  val{};
    string_view<CharT> str{};
  };
  /**
   * @brief
   *
   * @tparam EnumT
   * @tparam NumEnumerators
   * @tparam LineWidth
   * @tparam CharT
   */
  template <typename EnumT,
            size_t NumEnumerators,
            size_t LineWidth,
            typename CharT>
  class Enum_t : public sgl::Item_t<LineWidth, CharT> {
  public:
    Enum_t(string_view<CharT> item_name,
           const Pair<EnumT, char> (&array)[NumEnumerators],
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

    string_view<CharT> current_string() const noexcept {
      return map[index_].str;
    }
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

    Pair<EnumT, CharT> map[NumEnumerators]{};
    size_t             index_{0};
  };

  template <typename EnumT,
            size_t NumEnumerators,
            size_t LineWidth,
            typename CharT>
  struct is_item<Enum_t<EnumT, NumEnumerators, LineWidth, CharT>>
      : std::true_type {};
} // namespace sgl
#endif