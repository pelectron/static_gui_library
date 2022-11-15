//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_CALLABLE_HPP
#define SGL_CALLABLE_HPP
#include <new>
#include <type_traits>

namespace sgl {
  /// \cond

  // intentional forward declaration only. specialization follows below.
  template <typename Signature>
  class Callable;

  /// \endcond

  /// \headerfile callable.hpp "sgl/callable.hpp"
  /// \anchor Callable
  /// \brief This class implements a callable with the guarantee that it does not
  /// heap allocate, throw, and doesn't have the constness bug of std::function. Additionally,
  /// constexpr fee functions and non capturing lambdas can be executed at compile time, i.e. the
  /// following will successfully compile:
  /// ```
  /// static_assert(Callable<int()>([]() noexcept { return 0;})() == 0,"");
  /// ```
  ///
  /// \details This class can bind and execute function objects/functors/lambdas,
  /// free functions and member functions with a return type of Ret and arguments
  /// Args... . See std::function, google search for delegates, etc. if the
  /// concept is not clear.
  /// \tparam Ret return type of the callable
  /// \tparam Args argument types
  template <typename Ret, typename... Args>
  class Callable<Ret(Args...)> {
  public:
    /// Default constructor
    constexpr Callable() noexcept = default;
    /// move constructor
    /// @param other callable to move from
    constexpr Callable(Callable<Ret(Args...)>&& other) noexcept;
    /// copy constructor
    /// \param other callable to copy
    constexpr Callable(const Callable<Ret(Args...)>& other) noexcept;

    /// Construct callable from free function pointer
    /// \param f pointer to free function
    constexpr explicit Callable(Ret (*f)(Args...) noexcept) noexcept;

    /// \brief Construct a new callable from object and member function
    /// \tparam T object type
    /// \param obj object instance
    /// \param member_function pointer to member function
    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept;

    /// \brief Construct a new callable from object and const member function
    /// \tparam T object type
    /// \param obj object instance
    /// \param member_function pointer to member function
    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) const noexcept) noexcept;

    /// \brief Construct a callable with a function object/functor
    /// \tparam F function object type
    /// \param f function object instance
    template <
        typename F,
        std::enable_if_t<(!std::is_constructible_v<Ret (*)(Args...) noexcept, std::decay_t<F>>)and(
            !std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>)>* = nullptr>
    explicit Callable(F&& f) noexcept(std::is_nothrow_constructible_v<F>);

    /// move assignment operator
    /// \param other callable to move assign from
    /// \return reference to this
    constexpr Callable& operator=(Callable<Ret(Args...)>&& other) noexcept;

    /// copy assignment operator
    /// \param other callable to copy assign from
    /// \return reference to this
    constexpr Callable& operator=(const Callable<Ret(Args...)>& other) noexcept;

    /// invoke delegate. Returns statically allocated value, i.e. 0, if the callable is not bound to
    /// anything.
    /// \param args arguments to call stored invocable with
    /// \return Ret
    constexpr Ret operator()(Args... args) noexcept;

    /// bind free function
    /// \param free_function pointer to free function
    constexpr void bind(Ret (*free_function)(Args...) noexcept) noexcept;

    /// bind free function
    /// \param free_function reference to free function
    constexpr void bind(Ret (&free_function)(Args...) noexcept) noexcept;

    /// bind other callable, i.e. simply copy other into this.
    /// \param other callable to copy
    constexpr void bind(const Callable& other) noexcept;

    /// bind other callable, i.e. simply move other into this.
    /// \param other callable to move
    constexpr void bind(Callable&& other) noexcept;

    /// \brief bind object and member function
    ///
    /// \tparam T object type
    /// \param obj object instance
    /// \param member_function member function pointer
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept;

    /// \brief bind object and const member function
    ///
    /// \tparam T object type
    /// \param obj object instance
    /// \param member_function pointer to const qualified member function
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) const noexcept) noexcept;

    /// \brief bind capturing lambda/ custom functor
    ///
    /// \tparam F invocable type
    /// \param f invocable instance
    template <
        typename F,
        std::enable_if_t<(!std::is_constructible_v<Ret (*)(Args...) noexcept, std::decay_t<F>>)and(
            !std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>)>* = nullptr>
    constexpr void bind(F&& f) noexcept;

    /// unbind stored callable
    constexpr void reset() noexcept { invoke_ = &null_invoke; }

  private:
    template <typename T>
    struct mfn {
      T* t;
      Ret (T::*member)(Args...) noexcept;

      constexpr Ret operator()(Args... args) noexcept {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    template <typename T>
    struct cmfn {
      T* t;
      Ret (T::*member)(Args...) const noexcept;

      constexpr Ret operator()(Args... args) noexcept {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    union Storage {
      Ret (*func)(Args...) noexcept;
      char buffer[2 * sizeof(void*)];
    };

    static constexpr Ret null_invoke(Storage* s, Args... a) noexcept {
      static_cast<void>(s);
      (void(a), ...);
      if constexpr (!std::is_same_v<Ret, void>) {
        return Ret{};
      }
    }

    constexpr static Ret free_function_invoke(Storage* storage, Args... args) noexcept {
      return static_cast<Ret>((storage->func)(args...));
    }

    template <typename T>
    static Ret inline_invoke(Storage* storage, Args... args) noexcept {
      return static_cast<Ret>((*static_cast<T*>(static_cast<void*>(storage->buffer)))(args...));
    }

    // data members
    Ret (*invoke_)(Storage*, Args...) noexcept {&null_invoke};
    Storage buffer_{nullptr};
  };
} // namespace sgl

#include "sgl/impl/callable_impl.hpp"
#endif /* SGL_CALLABLE_HPP */
