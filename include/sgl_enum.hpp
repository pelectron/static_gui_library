#ifndef SGL_BASIC_ENUM_HPP
#define SGL_BASIC_ENUM_HPP
#include "sgl_item_base.hpp"
namespace sgl {

  /// A pair of T and sgl::string_view<CharT>
  template <typename T, typename CharT>
  struct Pair {
    constexpr Pair() noexcept = default;
    constexpr Pair& operator=(const Pair&) noexcept = default;
    constexpr Pair& operator=(Pair&&) noexcept = default;

    template <size_t N>
    constexpr Pair(const T& t, const CharT (&str)[N]) : value(t), string(str) {}
    constexpr Pair(const Pair&) noexcept = default;
    constexpr Pair(Pair&&) noexcept = default;

    T                       value;
    sgl::string_view<CharT> string;
  };

  /**
   * @brief This class implements a enumerated enum_item, i.e. a enum_item with
   * only a limited amount of possible values.
   *  @details see sgl::make_enum or an easy way to instantiate a Enum
   * enum_item.
   *
   * @tparam T the enumeration type
   * @tparam NumEnumerators number of enumerated values
   * @tparam TextSize number of characters per line in the menu
   * @tparam CharT character type of the enum_item
   */
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  class Enum : public sgl::ItemBase<traits<Enum<T, NumEnumerators, TextSize, CharT>>> {
  public:
    using value_type = T;
    using item_type = sgl::Enum<T, NumEnumerators, TextSize, CharT>;
    using Base = sgl::ItemBase<traits<Enum<T, NumEnumerators, TextSize, CharT>>>;
    using StringView = typename Base::StringView;
    using Pair = sgl::Pair<T, CharT>;
    using Map_t = Pair[NumEnumerators];

    /**
     * @brief Construct a Enum enum_item. See the example of how to
     * construct without naming types explicitly.
     * @param name name of the enum_item
     * @param map  maps T's to corresponding string_views. See
     * sgl::Pair and sgl::make_enum for an example.
     * @param start_index
     */
    constexpr Enum(StringView name,
                   const Pair (&map)[NumEnumerators],
                   size_t start_index = 0) noexcept;

    /**
     * @brief Construct a new Enum object
     * @tparam InputHandler
     * @param name name of enum_item
     * @param map maps T's to corresponding string_views. See
     * sgl::make_enum for an example
     * @param handler enum input handler
     * @param start_index which value to show first
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    Enum(StringView name,
         const Pair (&map)[NumEnumerators],
         InputHandler&& handler,
         size_t         start_index = 0) noexcept;

    constexpr Enum(StringView name,
                   const Pair (&map)[NumEnumerators],
                   sgl::error (*handler)(item_type&, sgl::Input) noexcept,
                   size_t start_index = 0) noexcept;

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
    /// it's map_.
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

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>::Enum(StringView name,
                                                           const Pair (&map)[NumEnumerators],
                                                           size_t start_index) noexcept
      : Enum<T, NumEnumerators, TextSize, CharT>(name, map, &default_handle_input, start_index) {}

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, Enum<T, NumEnumerators, TextSize, CharT>>>
  Enum<T, NumEnumerators, TextSize, CharT>::Enum(StringView name,
                                                 const Pair (&map)[NumEnumerators],
                                                 InputHandler&& handler,
                                                 size_t         start_index) noexcept
      : Base(name, map[start_index % NumEnumerators].string, forward<InputHandler>(handler)),
        map_{}, index_(start_index % NumEnumerators) {
    for (size_t i = 0; i < NumEnumerators; ++i) {
      map_[i] = map[i];
    }
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr Enum<T, NumEnumerators, TextSize, CharT>::Enum(
      StringView name,
      const Pair (&map)[NumEnumerators],
      sgl::error (*handler)(item_type&, sgl::Input) noexcept,
      size_t start_index) noexcept
      : Base(name, map[start_index % NumEnumerators].string, handler),
        index_(start_index % NumEnumerators) {
    for (size_t i = 0; i < NumEnumerators; ++i) {
      map_[i] = map[i];
    }
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr size_t Enum<T, NumEnumerators, TextSize, CharT>::num_values() const noexcept {
    return NumEnumerators;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr typename Enum<T, NumEnumerators, TextSize, CharT>::StringView
      Enum<T, NumEnumerators, TextSize, CharT>::current_string() const noexcept {
    return map_[index_].string;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr size_t Enum<T, NumEnumerators, TextSize, CharT>::index() const noexcept {
    return index_;
  }
  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr void Enum<T, NumEnumerators, TextSize, CharT>::set_index(size_t index) noexcept {
    index_ = index % NumEnumerators;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr T Enum<T, NumEnumerators, TextSize, CharT>::get_value() const noexcept {
    return map_[index_].value;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr sgl::error Enum<T, NumEnumerators, TextSize, CharT>::set_value(T value) noexcept {
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

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  const typename Enum<T, NumEnumerators, TextSize, CharT>::Map_t&
      Enum<T, NumEnumerators, TextSize, CharT>::get_map() const noexcept {
    return map_;
  }

  template <typename T, size_t NumEnumerators, size_t TextSize, typename CharT>
  constexpr sgl::error
      Enum<T, NumEnumerators, TextSize, CharT>::default_handle_input(item_type& enum_item,
                                                                     sgl::Input input) noexcept {
    switch (input) {
      case sgl::Input::right:
        [[fallthrough]];
      case sgl::Input::up:
        enum_item.set_index((enum_item.index() + 1) % enum_item.num_values());
        break;
      case sgl::Input::left:
        [[fallthrough]];
      case sgl::Input::down:
        enum_item.set_index(enum_item.index() == 0 ? enum_item.num_values() - 1
                                                   : enum_item.index() - 1);
        break;
      default:
        break;
    }
    enum_item.set_text(enum_item.current_string());
    return error::no_error;
  }
} // namespace sgl
#endif