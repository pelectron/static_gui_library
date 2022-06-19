#ifndef SGL_BASIC_ENUM_HPP
#define SGL_BASIC_ENUM_HPP
#include "sgl_enum_map.hpp"
#include "sgl_item_base.hpp"

namespace sgl {

  /**
   * \brief This class implements a enumerated enum_item, i.e. a enum_item with
   * only a limited amount of possible values.
   *  \details see sgl::make_enum or an easy way to instantiate a Enum
   * enum_item.
   *
   * \tparam T the enumeration type
   * \tparam NumEnumerators number of enumerated values
   * \tparam TextSize number of characters per line in the menu
   * \tparam CharT character type of the enum_item
   */
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  class Enum : public sgl::ItemBase<Enum<T, NumEnumerators, TextSize, CharT>> {
  public:
    using value_type = T;
    using item_type = sgl::Enum<T, NumEnumerators, TextSize, CharT>;
    using Base = sgl::ItemBase<item_type>;
    using StringView = typename Base::StringView;
    using Pair = sgl::Pair<T, string_view<CharT>>;
    using Map_t = EnumMap<T, NumEnumerators, CharT>;
    using EnumMap = EnumMap<T, NumEnumerators, CharT>;
    constexpr Enum(Enum&&) = default;
    constexpr Enum(const Enum&) = default;
    /**
     * \brief Construct a Enum enum_item. See the example of how to
     * construct without naming types explicitly.
     * \param name name of the enum_item
     * \param map  maps T's to corresponding string_views. See
     * sgl::Pair and sgl::make_enum for an example.
     * \param start_index
     */

    constexpr Enum(StringView name, const EnumMap& map, size_t start_index = 0) noexcept
        : Base(name, map.get_view(start_index % NumEnumerators), &default_handle_input), map_(map),
          index_(start_index % NumEnumerators) {}
    /**
     * \brief Construct a new Enum object
     * \tparam InputHandler
     * \param name name of enum_item
     * \param map maps T's to corresponding string_views. See
     * sgl::make_enum for an example
     * \param handler enum input handler
     * \param start_index which value to show first
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr Enum(StringView     name,
                   const EnumMap& map,
                   InputHandler&& handler,
                   size_t         start_index = 0) noexcept;

    /// get number of enumerated values
    constexpr size_t num_values() const noexcept;

    /// get string of current value
    constexpr StringView current_string() const noexcept;

    /// get index of current value
    constexpr size_t index() const noexcept;

    /// set current value by index
    constexpr void set_index(size_t index) noexcept;

    /// get current value
    constexpr T get_value() const noexcept;

    /// set value. returns sgl::error::invalid_value if value is not found in
    /// it's map.
    constexpr sgl::error set_value(T value) noexcept;

    /// get access to the array of pairs storing to enum values and
    /// string_views.
    const Map_t& get_map() const noexcept;

  private:
    constexpr static sgl::error default_handle_input(item_type& enum_item,
                                                     sgl::Input input) noexcept;
    Map_t                       map_;
    size_t                      index_{0};
  };

  template <size_t TextSize, typename CharT, typename T, size_t NumEnumerators>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>
      make_enum(sgl::string_view<CharT>                  name,
                const EnumMap<T, NumEnumerators, CharT>& map,
                size_t                                   start_index = 0);

  template <size_t TextSize,
            typename CharT,
            typename T,
            size_t NumEnumerators,
            typename InputHandler,
            enable_if_is_input_handler<InputHandler, Enum<T, NumEnumerators, TextSize, CharT>>>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>
      make_enum(sgl::string_view<CharT>                  name,
                const EnumMap<T, NumEnumerators, CharT>& map,
                InputHandler&&                           handler,
                size_t                                   start_index = 0);

  template <size_t TextSize,
            typename CharT,
            typename T,
            size_t NumEnumerators,
            size_t N,
            typename InputHandler,
            enable_if_is_input_handler<InputHandler, Enum<T, NumEnumerators, TextSize, CharT>>>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>
      make_enum(const CharT (&name)[N],
                const EnumMap<T, NumEnumerators, CharT>& map,
                InputHandler&&                           handler,
                size_t                                   start_index = 0);

} // namespace sgl
#include "impl/sgl_enum_impl.hpp"
#endif