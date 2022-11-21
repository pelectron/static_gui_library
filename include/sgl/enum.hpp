//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_ENUM_HPP
#define SGL_ENUM_HPP
#include "sgl/enum_map.hpp"
#include "sgl/item_base.hpp"
#include "sgl/smallest_type.hpp"

namespace sgl {
  /**
    @headerfile enum.hpp "sgl/enum.hpp"
    @ingroup item_types
   */

  /**
    This class implements a enumerated enum_item, that is a enum_item with
    only a limited amount of possible values.
    see sgl::make_enum() or an easy way to instantiate a Enum
    enum_item without having to define the template parameters.

    @tparam T the enumeration type
    @tparam NumEnumerators number of enumerated values
    @tparam TextSize number of characters per line in the menu
    @tparam CharT character type of the enum_item
   */
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  class Enum : public sgl::ItemBase<Enum<T, NumEnumerators, TextSize, CharT>> {
  public:
    /// value type of this item
    using value_type = T;

    /// base class of this item
    using Base = sgl::ItemBase<Enum<T, NumEnumerators, TextSize, CharT>>;

    /// this item type
    using item_type = typename Base::item_type;

    /// string_view type of this item
    using StringView = typename Base::StringView;

    /// default move constructor
    constexpr Enum(Enum&&) = default;

    /// default copy constructor
    constexpr Enum(const Enum&) = default;

    /**
      Construct a Enum enum_item. See the example of how to
      construct without naming types explicitly.

      @param map  maps T's to corresponding string_views. See
      sgl::Pair and sgl::make_enum for an example.
      @param start_index
     */
    constexpr Enum(const sgl::EnumMap<T, NumEnumerators, CharT>& map,
                   size_t                                        start_index = 0) noexcept
        : Base(map.get_view(start_index % NumEnumerators), &default_handle_input), map_(map),
          index_(start_index % NumEnumerators) {}

    /**
      Construct a new Enum object
      @tparam InputHandler See [here](markdown/concepts.md#input-handler)
      @param map maps T's to corresponding string_views. See
      sgl::make_enum for an example
      @param handler enum input handler
      @param start_index which value to show first
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr Enum(const sgl::EnumMap<T, NumEnumerators, CharT>& map,
                   InputHandler&&                                handler,
                   size_t                                        start_index = 0) noexcept;

    /**
      get number of enumerated values
      @return size_t
     */
    [[nodiscard]] constexpr size_t num_values() const noexcept;

    /**
      get string of current value
      @return StringView
     */
    [[nodiscard]] constexpr StringView current_string() const noexcept;

    /**
      get index of current value
      @return size_t
     */
    [[nodiscard]] constexpr size_t index() const noexcept;

    /**
      set current value by index
      @param index zero based index
     */
    constexpr void set_index(size_t index) noexcept;

    /**
      get current value
      @return T
     */
    [[nodiscard]] constexpr T get_value() const noexcept;

    /**
      set value. returns sgl::error::invalid_value if value is not found in
      its map.
      @param value value to set
      @return sgl::error
     */
    [[nodiscard]] constexpr sgl::error set_value(T value) noexcept;

  private:
    /**
      default input handler
      @return sgl::error
     */
    constexpr static sgl::error default_handle_input(item_type& enum_item,
                                                     sgl::input input) noexcept;

    sgl::EnumMap<T, NumEnumerators, CharT> map_; ///< map containing array of enum string pairs.
    sgl::smallest_type_t<NumEnumerators>   index_{0}; ///< map index of current value
  };

  /**
    @ingroup item_factories
    @defgroup enum_factories Enum Factory Functions
    @{
   */

  /**
    create an enum item with every template parameter deduced. The signature is a bit
    complex on first sight, but becomes ver clear in a simple example: The idea is that by
    providing the arguments in the following way : Enum::val1, "string literal val1", Enum::val2,
    "string literal val2", ... etc.,  the enumeration type, number of enumerated values, the
    maximum text size required and the character type can all be deduced from the arguments.
    ```cpp
     enum class Setting { opt1, opt2, opt3 };
     // the type of setting_item is sgl::Enum<Setting,3,8,char>
     constexpr auto setting_item = sgl::make_enum(Setting::opt1, "Option 1",
                                                  Setting::opt2, "Option 2",
                                                  Setting::opt3, "Option 3");
    ```
    @tparam E the enum type
    @tparam Str string type
    @tparam EnumStringArgs rest of enum value/string pairs
    @param enum_val first enum value
    @param enum_string string literal of enum_val
    @param args rest of enum value and string pairs in the same order as the first
    @return sgl::Enum<E, NumEnumerators, max_string_size(enum_string,args),
    character_type(enum_string)>
   */
  template <typename E, typename Str, typename... EnumStringArgs>
  constexpr auto
      make_enum(const E& enum_val, const Str& enum_string, const EnumStringArgs&... args);

  /**
    create an enum item with every template parameter deduced and custom start index. See
    sgl::make_enum(const E& enum_val, const Str& enum_string, const EnumStringArgs&... args) for
    an explanation of the parameters after the start index.

    ```cpp
    enum class Setting { opt1, opt2, opt3 };
    // the type of setting_item is sgl::Enum<Setting,3,8,char>, get_value() would return
    // Setting::opt3.
    constexpr auto setting_item = sgl::make_enum(2, Setting::opt1, "Option 1",
                                                  Setting::opt2, "Option 2",
                                                  Setting::opt3, "Option 3");
    ```

    @tparam E the enum type
    @tparam Str string type
    @tparam EnumStringArgs rest of enum value/string pairs
    @param start_index which value to show first. Zero based.
    @param enum_val first enum value
    @param enum_string string literal of enum_val
    @param args rest of enum value and string pairs in the same order as the first
    @return sgl::Enum<E, NumEnumerators, max_string_size(enum_string,args),
    character_type(enum_string)>
   */
  template <typename E, typename Str, typename... EnumStringArgs>
  constexpr auto make_enum(size_t     start_index,
                           const E&   enum_val,
                           const Str& enum_string,
                           const EnumStringArgs&... args);

  /**
    create an enum item with every template parameter deduced and custom start index and
    input handler.See sgl::make_enum(const E& enum_val, const Str& enum_string, const
    EnumStringArgs&... args) for an explanation of the parameters after the start index.

    ```cpp
    enum class Setting { opt1, opt2, opt3 };
    // the type of setting_item is sgl::Enum<Setting,3,8,char>, get_value() would return
    // Setting::opt2.
    constexpr auto setting_item = sgl::make_enum([](auto&item,sgl::input i){...}, 1,
    Setting::opt1, "Option 1", Setting::opt2, "Option 2", Setting::opt3, "Option 3");
    ```

    @tparam InputHandler enum item input handler
    @tparam E the enum type
    @tparam Str string type
    @tparam EnumStringArgs rest of enum value/string pairs
    @param handler custom enum item input handler
    @param start_index which value to show first. Zero based.
    @param enum_val first enum value
    @param enum_string string literal of enum_val
    @param args rest of enum value and string pairs in the same order as the first
    @return sgl::Enum<E, NumEnumerators, max_string_size(enum_string,args),
    character_type(enum_string)>
   */
  template <typename InputHandler, typename E, typename Str, typename... EnumStringArgs>
  constexpr auto make_enum(InputHandler&& handler,
                           size_t         start_index,
                           const E&       enum_val,
                           const Str&     enum_string,
                           const EnumStringArgs&... args);
  /** @}*/
} // namespace sgl

#include "sgl/impl/enum_impl.hpp"
#endif /* SGL_ENUM_HPP */
