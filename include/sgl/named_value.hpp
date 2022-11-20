/**
 * @file named_value.hpp
 * @author Pelé Constam (you@domain.com)
 * @brief This file defines the NamedValue and Name types used by NamedTuple and operator<<=().
 * @version 0.1
 * @date 2022-07-19
 */

//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SGL_NAMED_VALUE_HPP
#define SGL_NAMED_VALUE_HPP
#include "sgl/name.hpp"
#include "sgl/string_view.hpp"

#include <type_traits>

namespace sgl {

  /**
   * @headerfile named_value.hpp "sgl/named_value.hpp"
   * @brief This type represents a pair of an sgl::Name and a value of type T. To create a
   * NamedValue, either use the constructor or the @ref sgl::operator <<=() "operator <<=". An
   * example of how to use:
   *
   * @code
   *
   *  auto named_val1 = NamedValue(NAME("N1"), 5.3f);
   *
   *  constexpr auto my_name = NAME("name");
   *
   *  auto named_val2 {my_name <<= 2.5f};
   *
   * @endcode
   *
   * @tparam T value type
   * @tparam Chars characters of the name
   */
  template <typename Name, typename T>
  class NamedValue {
  public:
    static_assert(sgl::is_name_type_v<Name>, "Name is not an instance of sgl::Name<Chars...>.");
    /// value type of the argument
    using value_type = T;
    /// name type of the argument
    using name_type = Name;

    /// @brief default ctor.
    constexpr NamedValue() = default;

    /// @brief copy ctor
    /// @param other object to copy
    constexpr NamedValue(const NamedValue& other) = default;

    /// @brief move ctor
    /// @param other object to move
    constexpr NamedValue(NamedValue&& other) = default;

    /// @brief construct from name and value
    /// @param name name of NamedValue
    /// @param value value of NamedValue
    constexpr NamedValue(name_type         name,
                         const value_type& value) noexcept(std::is_nothrow_copy_constructible_v<T>);

    /// @brief move construct from name and value
    /// @param name name of NamedValue
    /// @param value value to move into NamedValue
    constexpr NamedValue(name_type    name,
                         value_type&& value) noexcept(std::is_nothrow_move_constructible_v<T>);

    /// @brief get name as sgl::string_view<char>
    /// @return sgl::string_view<char>
    [[nodiscard]] constexpr sgl::string_view<char> name() const noexcept;

    /// @brief get reference to value
    /// @return value_type&
    [[nodiscard]] constexpr value_type& value() noexcept;

    /// @brief get const reference to value
    /// @return const value_type&
    [[nodiscard]] constexpr const value_type& value() const noexcept;

  private:
    value_type value_; ///< value of the argument
  };

  /// @cond
  template <typename T, char... Chars>
  NamedValue(Name<Chars...>, T&&) -> NamedValue<Name<Chars...>, std::decay_t<T>>;
  /// @endcond

  /**
   * @brief This operator exists for easy creation of named arguments, i.e.
   * sgl::NamedValue. The return type of this operator is an sgl::NamedValue<Name,T> "NamedValue",
   * if T is not another name type. The reasons for using this operator to create a NamedValue are
   * the following:
   *  - it is has the second lowest precedence.
   *  - it is not often used, i.e. no real danger of confusing it with anything else.
   *  - it can be implemented as a non member function.
   *
   * ```cpp
   * // the type of named_value is NamedValue<Name<'n','a','m','e'>, float>
   * auto named_value {NAME("name") <<= 2.5f};
   * ```
   *
   * @tparam T value type
   * @tparam Chars characters of the name
   * @param name name of the NamedValue
   * @param value value of the NamedValue
   * @return  NamedValue<Name<Chars...>, std::decay_t<T>>
   */
  template <typename T, char... Chars>
  constexpr NamedValue<sgl::Name<Chars...>, std::decay_t<T>> operator<<=(sgl::Name<Chars...> name,
                                                                         T&&                 value);

} // namespace sgl

#include "impl/named_value_impl.hpp"
#endif /* SGL_NAMED_VALUE_HPP */
