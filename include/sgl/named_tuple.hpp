
/// \file sgl/named_tuple.hpp
/// \author Pelé Constam (you\domain.com)
/// \version 0.1
/// \date 2022-06-20
/// \brief This file defines the NamedTuple class and it's iteration functions, sgl::for_each() and
/// sgl::for_each_with_name().
/// \copyright Copyright (c) 2022
///

#ifndef SGL_NAMED_TUPLE_HPP
#define SGL_NAMED_TUPLE_HPP
#include "sgl/fwd.hpp"
#include "sgl/named_value.hpp"
#include "sgl/type_list.hpp"

namespace sgl {

  template <typename F, typename... Ts>
  inline constexpr bool
      nothrow_invocable_for_each = (std::is_nothrow_invocable_v<std::decay_t<F>, Ts> && ...);

  /// \headerfile named_tuple.hpp "sgl/named_tuple.hpp"
  /// \brief A named tuple is like a normal std::tuple, except that it can also be indexed by
  /// 'name'.
  /// \details
  /// Creating a NamedTuple is most easily done with the \ref sgl::operator<<=(). This way, CTAD
  /// will do all the magic for you.
  ///
  /// \code
  /// #include "sgl/named_tuple.hpp"
  /// using namespace sgl;
  /// constexpr auto  name1 = NAME(" name1");
  /// constexpr auto  name2 = NAME(" name2");
  /// constexpr auto  name3 = NAME(" name3");
  /// auto tuple = NamedTuple( name1 <<= 5, name2 <<= 1.15,  name3 <<= 5.3f);
  ///
  /// \endcode
  ///
  /// The named tuple can be accessed with the sgl::get() and NamedTuple::get() methods by index or
  /// name. There is also a operator[] for indexing by name.
  ///
  /// \code
  /// // prints '5, 1.15, 5.3', tuple indexed by different, but equivalent, methods
  /// std::cout << tuple[ name1] << ", " << sgl::get( name2,tuple) << tuple.get(name3) << std::endl;
  /// // same as this:
  /// std::cout << tuple.get<0>() ", " << sgl::get<1>(tuple) << tuple.get<2>() << std::endl;
  /// \endcode
  ///
  /// A variadic functor like a generic lambda can be invoked on all elements, again with free and
  /// member function overloads:
  ///
  /// \code
  /// sgl::for_each(tuple, [](auto& elem){/*doing something with element*/});
  /// tuple.for_each([](auto& elem){/*doing something with element*/});
  /// \endcode
  ///
  /// \tparam Names names of the elements. Must be of type sgl::Name<....>.
  /// \tparam Ts value types of the elements.
  template <typename... Names, typename... Ts>
  class NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>> : NamedValue<Names, Ts>... {
  public:
    static_assert((is_name_type_v<Names> && ...),
                  "Every type in pack Names... must be a name type.");
    /// true if all Ts are nothrow default constructible
    static constexpr bool nothrow_default_constructible =
        (std::is_nothrow_default_constructible_v<NamedValue<Names, Ts>> && ...);
    /// true if all Ts are nothrow destructible
    static constexpr bool nothrow_destructible =
        (std::is_nothrow_destructible_v<NamedValue<Names, Ts>> && ...);
    /// true if all Ts are nothrow copy constructible
    static constexpr bool nothrow_copy_constructible =
        (std::is_nothrow_copy_constructible_v<NamedValue<Names, Ts>> && ...);
    /// true if all Ts are nothrow move constructible
    static constexpr bool nothrow_move_constructible =
        (std::is_nothrow_move_constructible_v<NamedValue<Names, Ts>> && ...);
    /// type list containing all names
    using name_list_t = type_list<Names...>;
    /// type list of all value types
    using type_list_t = type_list<Ts...>;

    /// default ctor
    constexpr NamedTuple() = default;

    /// default copy ctor
    constexpr NamedTuple(const NamedTuple&) = default;

    /// default move ctor
    constexpr NamedTuple(NamedTuple&&) = default;

    /// construct from values
    /// \param elems unnamed values to construct tuple
    constexpr explicit NamedTuple(const Ts&... elems) noexcept(nothrow_copy_constructible);

    /// construct from values
    /// \param elems unnamed values to construct tuple
    constexpr explicit NamedTuple(Ts&&... elems) noexcept(nothrow_move_constructible);

    /// construct from named values
    /// \param elems named values to construct tuple
    constexpr explicit NamedTuple(const NamedValue<Names, Ts>&... elems) noexcept(
        nothrow_copy_constructible);

    /// construct from named values
    /// \param elems named values to construct tuple
    constexpr explicit NamedTuple(NamedValue<Names, Ts>&&... elems) noexcept(
        nothrow_move_constructible);

    /// \brief get element by name
    /// \tparam Name name type
    /// \param name name instance
    /// \return corresponding reference to element type
    template <typename Name>
    constexpr auto& get(Name name) noexcept;

    /// \brief get element by name
    /// \tparam Name name type
    /// \param name name instance
    /// \return corresponding const reference to element type
    template <typename Name>
    constexpr const auto& get(Name name) const noexcept;

    /// \brief get element by index
    /// \tparam I element index
    /// \return reference to I-th element
    template <size_t I>
    constexpr auto& get() noexcept;

    /// \brief get element by index
    /// \tparam I element index
    /// \return const reference to I-th element
    template <size_t I>
    constexpr const auto& get() const noexcept;

    /// \brief get element by name
    /// \tparam Name name type
    /// \param name name instance
    /// \return corresponding reference to element type
    template <typename Name>
    constexpr auto& operator[](Name name) noexcept;

    /// \brief get element by name
    /// \tparam Name name type
    /// \param name name instance
    /// \return corresponding const reference to element type
    template <typename Name>
    constexpr const auto& operator[](Name name) const noexcept;

    /// \brief apply a callable f on each element. f will be called with a reference to each
    /// element.
    /// \tparam F callable type
    /// \param f callable instance
    template <typename F>
    constexpr void for_each(F&& f) noexcept(nothrow_invocable_for_each<F, Ts&...>);

    /// \brief apply a callable f on each element. f will be called with a const reference to each
    /// element.
    /// \tparam F callable type
    /// \param f callable instance
    template <typename F>
    constexpr void for_each(F&& f) const noexcept(nothrow_invocable_for_each<F, const Ts&...>);

    /// \brief apply a callable f on each element. f will be called with the name of the element as
    /// a sgl::string_view<char> and a reference to each element.
    /// \tparam F callable type
    /// \param f callable instance
    template <typename F>
    constexpr void for_each_with_name(F&& f) noexcept(
        (std::is_nothrow_invocable_r_v<void, F, sgl::string_view<char>, Ts&> && ...));

    /// \brief apply a callable f on each element. f will be called with the name of the element as
    /// a sgl::string_view<char> and a const reference to each element.
    /// \tparam F callable type
    /// \param f callable instance
    template <typename F>
    constexpr void for_each_with_name(F&& f) const
        noexcept((std::is_nothrow_invocable_r_v<void, F, sgl::string_view<char>, const Ts&> &&
                  ...));

  private:
    using This_t = NamedTuple<type_list<Names...>, type_list<Ts...>>;

    static_assert(sgl::all_unique_v<Names...>, "sgl::NamedTuple can't have duplicate names!");
  };

  /// \cond
  // template deduction guides for NamedTuple
  template <typename... Names, typename... Ts>
  NamedTuple(NamedValue<Names, Ts>&&...)
      -> NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>;

  template <typename... Names, typename... Ts>
  NamedTuple(const NamedValue<Names, Ts>&...)
      -> NamedTuple<sgl::type_list<Names...>, sgl::type_list<Ts...>>;
  /// \endcond

  /// \brief get I-th element of NamedTuple
  /// \code
  /// NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::get<0>(tuple);
  /// sgl::get<1>(tuple);
  /// //sgl::get<2>(tuple); static assertion error with message 'index out of range'
  /// \endcode
  /// \tparam I index
  /// \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param t tuple
  /// \return reference to I-th element
  template <size_t I, typename NameList, typename TypeList>
  constexpr auto& get(NamedTuple<NameList, TypeList>& t) noexcept;

  /// \brief get I-th element of NamedTuple
  /// \code
  /// const NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::get<0>(tuple);
  /// sgl::get<1>(tuple);
  /// //sgl::get<2>(tuple); static assertion error with message 'index out of range'
  /// \endcode
  ///
  /// \tparam I element index
  /// \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param t tuple
  /// \return const reference to I-th element
  template <size_t I, typename NameList, typename TypeList>
  constexpr const auto& get(const NamedTuple<NameList, TypeList>& t) noexcept;

  /// \brief get element of tuple by name
  ///
  /// \code
  /// const NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::get(name1, tuple);
  /// sgl::get(name2, tuple);
  /// //sgl::get(name3, tuple); static assertion error with message 'tuple doesn't contain such a
  /// name'
  /// \endcode
  ///
  /// \tparam Name type of the name
  /// \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param name name of the tuple element
  /// \param t tuple
  /// \return reference to element
  template <typename Name, typename NameList, typename TypeList>
  constexpr auto& get(Name name, NamedTuple<NameList, TypeList>& t) noexcept;

  /// \brief get element of tuple by name
  /// \code
  /// const NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::get(name1, tuple);
  /// sgl::get(name2, tuple);
  /// //sgl::get(name3, tuple); static assertion error with message 'tuple doesn't contain such a
  /// name' \endcode \tparam Name type of the name \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param name name of the tuple element
  /// \param t tuple
  /// \return const reference to element
  template <typename Name, typename NameList, typename TypeList>
  constexpr const auto& get(Name name, const NamedTuple<NameList, TypeList>& t) noexcept;

  /// \brief apply f on each element in the tuple. f must be invocable with every type in TypeList.
  /// \code
  /// NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::for_each([](auto& value){...});
  /// \endcode
  /// \tparam F functor type
  /// \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param tuple tuple to invoke f on
  /// \param f functor
  template <typename F, typename NameList, typename TypeList>
  constexpr void for_each(NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(
      noexcept(std::declval<NamedTuple<NameList, TypeList>>().for_each(std::forward<F>(f))));

  /// \brief apply f on each element in the const tuple. f must be invocable with every const type
  /// in TypeList.
  /// \code
  /// const NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::for_each([](const auto& value){...});
  /// \endcode
  /// \tparam F functor type
  /// \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param tuple tuple to invoke f on
  /// \param f functor
  template <typename F, typename NameList, typename TypeList>
  constexpr void for_each(const NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(
      noexcept(std::declval<const NamedTuple<NameList, TypeList>>().for_each(std::forward<F>(f))));

  /// \brief apply f on each element in the tuple with the elements name and value. f must be
  /// invocable with a sgl::string_view<char> as it's first argument and a reference to T as it's
  /// second argument, for every T in TypeList.
  /// \code
  /// NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::for_each_with_name([](sgl::string_view<char> name, auto& value){...});
  /// \endcode
  /// \tparam F functor type
  /// \tparam NameList list of
  /// names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param tuple tuple to invoke f on
  /// \param f functor
  template <typename F, typename NameList, typename TypeList>
  constexpr void for_each_with_name(NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(noexcept(
      std::declval<NamedTuple<NameList, TypeList>>().for_each_with_name(std::forward<F>(f))));

  /// \brief apply f on each element in the tuple with the elements name and value. f must be
  /// invocable with a sgl::string_view<char> as it's first argument and a const reference to T as
  /// it's second argument, for every T in TypeList.
  /// \code
  /// const NamedTuple tuple(name1<<= 1, name2 <<=5.0);
  /// sgl::for_each_with_name([](sgl::string_view<char> name, const auto& value){...});
  /// \endcode
  /// \tparam F functor type
  /// \tparam NameList list of names of the tuple
  /// \tparam TypeList list of value types of the tuple
  /// \param tuple tuple to invoke f on
  /// \param f functor
  template <typename F, typename NameList, typename TypeList>
  constexpr void
      for_each_with_name(const NamedTuple<NameList, TypeList>& tuple, F&& f) noexcept(noexcept(
          std::declval<NamedTuple<NameList, TypeList>>().for_each_with_name(std::forward<F>(f))));
} // namespace sgl

#include "sgl/impl/named_tuple_impl.hpp"
#endif /* SGL_NAMED_TUPLE_HPP */
