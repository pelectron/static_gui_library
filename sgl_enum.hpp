#ifndef SGL_ENUM_HPP
#define SGL_ENUM_HPP
#include "sgl_item.hpp"
namespace sgl {
  template <typename T, typename CharT>
  struct Pair {
    T                  val;
    string_view<CharT> str;
  };

  /**
   * @addtogroup item_types Item Types
   * @{
   * @brief This class implements a enumerated item, i.e. a item with only a
   * limited amount of possible values.
   *
   * @tparam EnumT the enumeration type
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
    template <typename T>
    using enum_handler_check = std::enable_if_t<
        std::is_invocable_r_v<sgl::error,
                              T,
                              Enum_t<EnumT, NumEnumerators, LineWidth, CharT>&,
                              sgl::Input>>;
    using Pair = sgl::Pair<EnumT, CharT>;

    using Map_t = Pair[NumEnumerators];

    /**
     * @brief Construct a Enum_t item. See the example of how to construct
     * without naming types explicitly.
     *
     * @param item_name name of the item
     * @param map  map EnumT values and corresponding string_views. See nested
     * Pair class.
     * @param start_index
     */
    Enum_t(string_view_t item_name,
           const Pair (&map)[NumEnumerators],
           size_t start_index = 0)
        : Enum_t(item_name, map, default_handle_input, start_index) {}

    /**
     * @brief Construct a new Enum_t object
     *
     * @tparam EnumInputHandler
     * @param item_name
     * @param handler
     * @param start_index
     */
    template <typename EnumInputHandler,
              enum_handler_check<EnumInputHandler>* = nullptr>
    Enum_t(string_view_t item_name,
           const Pair (&map)[NumEnumerators],
           EnumInputHandler&& handler,
           size_t             start_index = 0)
        : sgl::Item_t<LineWidth, CharT>(
              item_name,
              map[start_index % NumEnumerators].str,
              [handle = std::move(handler)](sgl::Item_t<LineWidth, CharT>& item,
                                            sgl::Input                     i) {
                using Type = Enum_t<EnumT, NumEnumerators, LineWidth, CharT>;
                return handle(static_cast<Type&>(item), i);
              }),
          index_(start_index % NumEnumerators) {
      for (size_t i = 0; i < NumEnumerators; ++i) {
        map_[i] = map[i];
      }
    }

    /// get number of enumerated values
    constexpr size_t num_values() const { return NumEnumerators; }
    /// get string of current value
    string_view_t current_string() const noexcept { return map_[index_].str; }
    /// get index of current value
    constexpr size_t index() const { return index_; }
    /// set current value by index
    constexpr void set_index(size_t index) { index_ = index % NumEnumerators; }
    /// get current value
    constexpr EnumT get_value() const { return map_[index_].val; }
    /// set value. returns sgl::error::invalid_value if value is not found in
    /// it's map_.
    constexpr sgl::error set_value(EnumT value) {
      size_t i = 0;
      for (const auto& elem : map_) {
        if (elem.val == value) {
          index_ = i;
          return sgl::error::no_error;
        }
        ++i;
      }
      return sgl::error::invalid_value;
    }

    /// get access to the array of pairs storing to enum values and
    /// string_views.
    const Map_t& get_map() const { return map_; }

  private:
    static error default_handle_input(
        Enum_t<EnumT, NumEnumerators, LineWidth, CharT>& item,
        sgl::Input                                       input) {
      switch (input) {
        case sgl::Input::right:
          [[fallthrough]];
        case sgl::Input::up:
          item.set_index((item.index() + 1) % item.num_values());
          break;
        case sgl::Input::left:
          [[fallthrough]];
        case sgl::Input::down:
          item.set_index(item.index() == 0 ? item.num_values() - 1
                                           : item.index() - 1);
          break;
        default:
          break;
      }
      item.set_text(item.current_string());
      return error::no_error;
    }

    Map_t  map_{};
    size_t index_{0};
  };
  /// @}

  /**
   * @brief helper function to create an Enum_t instance. Despite the
   * complicated looking template arguments, they are all be deduced
   * @example 
   * @code 
   * #include "sgl.hpp"
   * using namespace sgl::string_view_literals;
   * enum class test_enum { _0, _1, _2, _3, _4 };
   * auto e = sgl::make_enum<test_enum,40>("item name"_sv,
   *                       {{test_enum::_1, "one"},
   *                        {test_enum::_0, "zero"},
   *                        {test_enum::_2, "two"},
   *                        {test_enum::_3, "three"},
   *                        {test_enum::_4, "four"}});
   * @endcode 
   * @tparam EnumT enumeration type
   * @tparam NumEnumerators number of enumerated values
   * @tparam LineWidth line width of the menu
   * @tparam CharT character type
   * @tparam sgl::Enum_t<EnumT, NumEnumerators, LineWidth, CharT>::Map_t deduced
   * map type, i.e. array of struct with EnumT value and string_view.
   * @param item_name name of the item
   * @param start_index index of the value in map to first show.
   * @return sgl::Enum_t<EnumT, NumEnumerators, LineWidth, CharT>
   */
  template <typename EnumT,
            size_t LineWidth,
            typename CharT,
            size_t NumEnumerators>
  sgl::Enum_t<EnumT, NumEnumerators, LineWidth, CharT>
      make_enum(sgl::string_view<CharT> item_name,
                const sgl::Pair<EnumT, CharT> (&enum_map)[NumEnumerators],
                size_t start_index = 0) {
    return sgl::Enum_t<EnumT, NumEnumerators, LineWidth, CharT>(item_name,
                                                                enum_map,
                                                                start_index);
  }

} // namespace sgl
#endif