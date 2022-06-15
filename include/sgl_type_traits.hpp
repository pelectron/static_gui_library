/**
 * @file sgl_traits.hpp
 * @author Pelé Constam (you@domain.com)
 * \brief This file implements some of the type traits found in the <type_traits> header.
 * @version 0.1
 * @date 2022-06-14
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SGL_TRAITS_HPP
#define SGL_TRAITS_HPP
#include <cfloat>
#include <climits>
#include <cstdint>

namespace sgl {
  /// @cond
  struct true_type {
    static constexpr bool value = true;
  };

  struct false_type {
    static constexpr bool value = false;
  };

  template <typename...>
  using void_t = void;

  template <typename, typename>
  static constexpr bool is_same_v = false;

  template <typename T>
  static constexpr bool is_same_v<T, T> = true;

  template <typename T>
  struct add_rvalue_reference {
    using type = T&&;
  };

  template <typename T>
  struct remove_reference {
    using type = T;
  };
  template <typename T>
  struct remove_reference<T&> {
    using type = T;
  };
  template <typename T>
  struct remove_reference<T&&> {
    using type = T;
  };

  template <typename T>
  using remove_reference_t = typename remove_reference<T>::type;

  template <typename T>
  struct is_const : false_type {};

  template <typename T>
  struct is_const<const T> : true_type {};

  template <typename T>
  static constexpr bool is_const_v = is_const<T>::value;

  template <typename T>
  struct is_lvalue_reference : false_type {};

  template <typename T>
  struct is_lvalue_reference<T&> : true_type {};

  template <typename T>
  static constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

  template <typename T>
  constexpr bool always_false = false;

  template <typename T>
  typename add_rvalue_reference<T>::type declval() noexcept {
    static_assert(always_false<T>, "declval not allowed in an evaluated context");
  }

  template <typename Void, typename Ret, typename F, typename... Args>
  struct invocable_r : false_type {};

  template <typename Ret, typename F, typename... Args>
  struct invocable_r<void_t<decltype(declval<F>()(declval<Args>()...))>, Ret, F, Args...> {
    static constexpr bool value = is_same_v<Ret, decltype(declval<F>()(declval<Args>()...))>;
  };

  template <typename Ret, typename F, typename... Args>
  static constexpr bool is_invocable_r_v = invocable_r<void, Ret, F, Args...>::value;

  template <typename Ret, typename F, typename... Args>
  static constexpr bool is_const_invocable_v = invocable_r<Ret, const F, void, Args...>::value;

  template <typename Void, typename Ret, typename F, typename... Args>
  struct nothrow_invocable_r : false_type {};

  template <typename Ret, typename F, typename... Args>
  struct nothrow_invocable_r<void_t<decltype(declval<F>()(declval<Args>()...))>, Ret, F, Args...> {
    static constexpr bool value =
        is_same_v<Ret, decltype(declval<F>()(declval<Args>()...))>and noexcept(
            declval<F>()(declval<Args>()...));
  };

  template <typename Ret, typename F, typename... Args>
  static constexpr bool is_nothrow_invocable_r_v =
      nothrow_invocable_r<void, Ret, F, Args...>::value;

  template <bool Expr, typename T>
  struct enable_if;

  template <typename T>
  struct enable_if<true, T> {
    using type = T;
  };

  template <bool Expr, typename T = void>
  using enable_if_t = typename enable_if<Expr, T>::type;

  template <typename T, typename = void, typename... Args>
  struct is_constructible : false_type {};

  template <typename T, typename... Args>
  struct is_constructible<T, void_t<decltype(T(declval<Args>()...))>, Args...> : true_type {};

  template <typename T, typename... Args>
  static constexpr bool is_constructible_v = is_constructible<T, void, Args...>::value;

  template <typename T>
  static constexpr bool is_copy_constructible_v = is_constructible_v<T, const T&>;

  template <typename T>
  static constexpr bool is_move_constructible_v = is_constructible_v<T, T&&>;

  template <typename T, typename = void, typename... Args>
  struct is_nothrow_constructible : false_type {};

  template <typename T, typename... Args>
  struct is_nothrow_constructible<T, void_t<decltype(T(declval<Args>()...))>, Args...> {
    static constexpr bool value = noexcept(T(declval<Args>()...))and noexcept(declval<T>().~T());
  };

  template <typename T, typename... Args>
  static constexpr bool is_nothrow_constructible_v =
      is_nothrow_constructible<T, void, Args...>::value;

  template <typename T>
  static constexpr bool is_nothrow_copy_constructible_v = is_nothrow_constructible_v<T, const T&>;

  template <typename T>
  static constexpr bool is_nothrow_move_constructible_v = is_nothrow_constructible_v<T, T&&>;

  template <typename T, typename = void, typename... Args>
  struct is_trivially_constructible : false_type {};

  template <typename T, typename... Args>
  struct is_trivially_constructible<T, void_t<decltype(T{declval<Args>()...})>, Args...>
      : true_type {};

  template <typename T, typename... Args>
  static constexpr bool is_trivially_constructible_v =
      is_trivially_constructible<T, Args...>::value;
  template <typename T, typename = void>
  struct is_trivially_copy_constructible : false_type {};

  template <typename T>
  struct is_trivially_copy_constructible<T, void_t<decltype(T{declval<const T>()})>> : true_type {};

  template <typename T>
  static constexpr bool is_trivially_copy_constructible_v =
      is_trivially_copy_constructible<T>::value;

  template <typename T, typename = void>
  struct is_trivially_move_constructible : false_type {};

  template <typename T, typename = void>
  struct is_nothrow_default_constructible : false_type {};

  template <typename T>
  struct is_nothrow_default_constructible<T, void_t<decltype(T{})>> {
    static constexpr bool value = noexcept(T{});
  };

  template <typename T>
  static constexpr bool is_nothrow_default_constructible_v =
      is_nothrow_default_constructible<T>::value;
  template <typename T>
  constexpr remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
  }

  template <typename T>
  struct is_trivially_move_constructible<T, void_t<decltype(T{move(declval<T>())})>> : true_type {};

  template <typename T>
  static constexpr bool is_trivially_move_constructible_v =
      is_trivially_move_constructible<T>::value;

  namespace detail {
    template <typename From, typename To>
    To convertible_test() {
      return declval<From>();
    }

    template <typename From, typename To>
    To convertible_test_nothrow() noexcept {
      return declval<From>();
    }
  } // namespace detail

  template <typename From, typename To, typename = void>
  struct is_convertible : false_type {};

  template <typename From, typename To>
  struct is_convertible<From, To, void_t<decltype(detail::convertible_test<From, To>())>>
      : true_type {};

  template <typename From, typename To>
  static constexpr bool is_convertible_v = is_convertible<From, To>::value;

  template <typename From, typename To, typename = void>
  struct is_nothrow_convertible : false_type {};

  template <typename From, typename To>
  struct is_nothrow_convertible<To,
                                From,
                                void_t<decltype(detail::convertible_test_nothrow<From, To>())>>
      : true_type {};

  template <typename From, typename To>
  static constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

  template <typename T, typename = void>
  struct is_nothrow_destructible : false_type {};

  template <typename T>
  struct is_nothrow_destructible<T, void_t<decltype(declval<T>().~T())>> {
    static constexpr bool value = noexcept(declval<T>().~T());
  };

  template <typename T>
  static constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

  template <bool, typename TTrue, typename TFalse>
  struct conditional {
    using type = TTrue;
  };

  template <typename TTrue, typename TFalse>
  struct conditional<false, TTrue, TFalse> {
    using type = TFalse;
  };

  template <bool b, typename TTrue, typename TFalse>
  using conditional_t = typename conditional<b, TTrue, TFalse>::type;

  template <typename T>
  struct is_array : false_type {};

  template <typename T>
  struct is_array<T[]> : true_type {};

  template <typename T, size_t N>
  struct is_array<T[N]> : true_type {};

  template <typename T>
  static constexpr bool is_array_v = is_array<T>::value;

  template <typename T>
  struct remove_extent {
    using type = T;
  };

  template <typename T>
  struct remove_extent<T[]> {
    using type = T;
  };

  template <typename T, size_t N>
  struct remove_extent<T[N]> {
    using type = T;
  };

  template <typename T>
  using remove_extent_t = typename remove_extent<T>::type;
  // is function
  // primary template
  template <typename>
  struct is_function : false_type {};

  // specialization for regular functions
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...)> : true_type {};

  // specialization for variadic functions such as printf
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...)> : true_type {};

  // specialization for function types that have cv-qualifiers
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) volatile> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const volatile> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) volatile> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const volatile> : true_type {};

  // specialization for function types that have ref-qualifiers
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...)&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const &> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) volatile &> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const volatile &> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...)&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const &> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) volatile &> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const volatile &> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) volatile &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const volatile &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) volatile &&> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const volatile &&> : true_type {};

  // specializations for noexcept versions of all the above (C++17 and later)

  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) volatile noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const volatile noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) volatile noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const volatile noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...)& noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const & noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) volatile & noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const volatile & noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...)& noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const & noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) volatile & noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const volatile & noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...)&& noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const && noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) volatile && noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args...) const volatile && noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...)&& noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const && noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) volatile && noexcept> : true_type {};
  template <typename Ret, typename... Args>
  struct is_function<Ret(Args..., ...) const volatile && noexcept> : true_type {};
  template <typename Sig>
  static constexpr bool is_function_v = is_function<Sig>::value;
  // is function end

  // add pointer
  namespace detail {

    template <typename T>
    struct type_identity {
      using type = T;
    }; // or use type_identity (since C++20)

    template <typename T>
    auto try_add_pointer(int) -> type_identity<remove_reference_t<T>*>;
    template <typename T>
    auto try_add_pointer(...) -> type_identity<T>;

  } // namespace detail

  template <typename T>
  struct add_pointer : decltype(detail::try_add_pointer<T>(0)) {};
  template <typename T>
  using add_pointer_t = typename add_pointer<T>::type;
  // add pointer end

  // remove cv
  template <typename T>
  struct remove_cv {
    using type = T;
  };

  template <typename T>
  struct remove_cv<const T> {
    using type = T;
  };

  template <typename T>
  struct remove_cv<volatile T> {
    using type = T;
  };

  template <typename T>
  struct remove_cv<const volatile T> {
    using type = T;
  };

  template <typename T>
  struct remove_const {
    using type = T;
  };

  template <typename T>
  struct remove_const<const T> {
    using type = T;
  };

  template <typename T>
  struct remove_volatile {
    using type = T;
  };

  template <typename T>
  struct remove_volatile<volatile T> {
    using type = T;
  };

  template <typename T>
  using remove_cv_t = typename remove_cv<T>::type;

  template <typename T>
  using remove_const_t = typename remove_const<T>::type;

  template <typename T>
  using remove_volatile_t = typename remove_volatile<T>::type;
  // remove cv end

  template <typename T>
  struct decay {
  private:
    using U = remove_reference_t<T>;

  public:
    using type = conditional_t<is_array_v<U>,
                               remove_extent_t<U>*,
                               conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>>;
  };

  template <typename T>
  using decay_t = typename decay<T>::type;

  template <typename T>
  constexpr T&& forward(remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
  }

  template <typename T>
  constexpr T&& forward(remove_reference_t<T>&& t) noexcept {
    return static_cast<T&&>(t);
  }

  template <typename T>
  struct is_floating_point : false_type {};

  template <>
  struct is_floating_point<float> : true_type {};

  template <>
  struct is_floating_point<double> : true_type {};

  template <>
  struct is_floating_point<long double> : true_type {};

  template <typename T>
  static constexpr bool is_floating_point_v = is_floating_point<T>::value;

  template <typename T>
  struct is_integral : false_type {};
  template <>
  struct is_integral<uint8_t> : true_type {};
  template <>
  struct is_integral<int8_t> : true_type {};
  template <>
  struct is_integral<uint16_t> : true_type {};
  template <>
  struct is_integral<int16_t> : true_type {};
  template <>
  struct is_integral<uint32_t> : true_type {};
  template <>
  struct is_integral<int32_t> : true_type {};
  template <>
  struct is_integral<uint64_t> : true_type {};
  template <>
  struct is_integral<int64_t> : true_type {};
  template <>
  struct is_integral<bool> : true_type {};
  template <>
  struct is_integral<char> : true_type {};

  template <>
  struct is_integral<char16_t> : true_type {};

  template <>
  struct is_integral<char32_t> : true_type {};

  template <>
  struct is_integral<wchar_t> : true_type {};
  template <typename T>
  static constexpr bool is_integral_v = is_integral<T>::value;

  template <typename T>
  struct is_unsigned {
    static constexpr bool value = static_cast<T>(0) < static_cast<T>(-1);
  };

  template <typename T>
  static constexpr bool is_unsigned_v = is_unsigned<T>::value;
  template <typename T>
  static constexpr bool is_signed_v = !is_unsigned<T>::value;

  template <template <typename...> typename Pred, typename... Args>
  struct constraint {
    using type = sgl::enable_if_t<Pred<Args...>::value, bool>;
  };

  template <template <typename...> typename Pred, typename... Args>
  using constraint_t = typename constraint<Pred, Args...>::type;

  template <template <typename> typename Pred, typename... Args>
  struct constraint_for_all {
    using type = sgl::enable_if_t<(Pred<Args>::value && ...), bool>;
  };

  template <template <typename> typename Pred, typename... Args>
  using constraint_for_all_t = typename constraint_for_all<Pred, Args...>::type;

  template <template <typename> typename Pred, typename... Args>
  struct constraint_for_none {
    using type = sgl::enable_if_t<((!Pred<Args>::value) && ...), bool>;
  };

  template <template <typename> typename Pred, typename... Args>
  using constraint_for_none_t = typename constraint_for_none<Pred, Args...>::type;

  template <template <typename> typename Pred, typename... Args>
  struct constraint_for_some {
    using type = sgl::enable_if_t<(Pred<Args>::value || ...), bool>;
  };

  template <template <typename> typename Pred, typename... Args>
  using constraint_for_some_t = typename constraint_for_some<Pred, Args...>::type;

  template <class T, typename = void>
  struct is_destructible : false_type {};

  template <typename T>
  struct is_destructible<T&, void> : true_type {};

  template <typename T>
  struct is_destructible<T, enable_if_t<is_function_v<T>>> : false_type {};

  template <typename T>
  struct is_destructible<T[], void> : false_type {};

  template <typename T>
  struct is_destructible<T, void_t<decltype(declval<remove_extent_t<T>&>().~remove_extent_t<T>())>>
      : true_type {};

  template <typename T>
  static constexpr bool is_destructible_v = is_destructible<T>::value;

  template <typename T>
  struct is_trivially_destructible {
    static constexpr bool value = is_destructible_v<T> and is_destructible_v<remove_extent_t<T>>;
  };

  template <typename T>
  static constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
  /// @endcond
} // namespace sgl
#endif