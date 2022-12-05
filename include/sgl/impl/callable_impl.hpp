//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef SGL_IMPL_CALLABLE_IMPL_HPP
#define SGL_IMPL_CALLABLE_IMPL_HPP

#include "sgl/callable.hpp"
#include <utility>

namespace sgl {
  template <typename Ret, typename... Args>
  constexpr Callable<Ret(Args...)>::Callable(Callable&& other) noexcept
      : invoke_(std::move(other.invoke_)), buffer_(std::move(other.buffer_)) {}

  template <typename Ret, typename... Args>
  constexpr Callable<Ret(Args...)>::Callable(const Callable& other) noexcept
      : invoke_(other.invoke_), buffer_(other.buffer_) {}

  /// Construct callable from free function pointer
  template <typename Ret, typename... Args>
  constexpr Callable<Ret(Args...)>::Callable(Ret (*f)(Args...) noexcept) noexcept
      : invoke_(&free_function_invoke), buffer_{f} {}

  template <typename Ret, typename... Args>
  template <typename T>
  Callable<Ret(Args...)>::Callable(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept {
    bind(obj, member_function);
  }

  template <typename Ret, typename... Args>
  template <typename T>
  Callable<Ret(Args...)>::Callable(T& obj,
                                   Ret (T::*member_function)(Args...) const noexcept) noexcept {
    bind(obj, member_function);
  }

  template <typename Ret, typename... Args>
  template <
      typename F,
      std::enable_if_t<(!std::is_constructible_v<Ret (*)(Args...) noexcept, std::decay_t<F>>)and(
          !std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>)>*>
  Callable<Ret(Args...)>::Callable(F&& f) noexcept(std::is_nothrow_constructible_v<F>) {
    static_assert(!std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>);
    bind(std::forward<F>(f));
  }

  template <typename Ret, typename... Args>
  constexpr Callable<Ret(Args...)>&
      Callable<Ret(Args...)>::operator=(Callable<Ret(Args...)>&& other) noexcept {
    buffer_ = other.buffer_;
    invoke_ = other.invoke_;
    return *this;
  }

  template <typename Ret, typename... Args>
  constexpr Callable<Ret(Args...)>&
      Callable<Ret(Args...)>::operator=(const Callable<Ret(Args...)>& other) noexcept {
    if (this == &other) {
      return *this;
    }
    buffer_ = other.buffer_;
    invoke_ = other.invoke_;
    return *this;
  }

  template <typename Ret, typename... Args>
  constexpr Ret Callable<Ret(Args...)>::operator()(Args... args) noexcept {
    return invoke_(&buffer_, args...);
  }

  template <typename Ret, typename... Args>
  constexpr void Callable<Ret(Args...)>::bind(Ret (*free_function)(Args...) noexcept) noexcept {
    buffer_.func = free_function;
    invoke_ = &free_function_invoke;
  }

  template <typename Ret, typename... Args>
  constexpr void Callable<Ret(Args...)>::bind(Ret (&free_function)(Args...) noexcept) noexcept {
    bind(&free_function);
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void Callable<Ret(Args...)>::bind(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept {
    static_assert(sizeof(mfn<T>) <= sizeof(buffer_), "");
    new (buffer_.buffer) mfn<T>{&obj, member_function};
    invoke_ = &inline_invoke<mfn<T>>;
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void Callable<Ret(Args...)>::bind(T& obj,
                                    Ret (T::*member_function)(Args...) const noexcept) noexcept {
    static_assert(sizeof(cmfn<T>) <= sizeof(buffer_), "");
    new (buffer_.buffer) cmfn<T>{&obj, member_function};
    invoke_ = &inline_invoke<cmfn<T>>;
  }

  template <typename Ret, typename... Args>
  constexpr void Callable<Ret(Args...)>::bind(const Callable& other) noexcept {
    buffer_ = other.buffer_;
    invoke_ = other.invoke_;
  }

  template <typename Ret, typename... Args>
  constexpr void Callable<Ret(Args...)>::bind(Callable&& other) noexcept {
    buffer_ = other.buffer_;
    invoke_ = other.invoke_;
  }

  template <typename Ret, typename... Args>
  template <
      typename F,
      std::enable_if_t<(!std::is_constructible_v<Ret (*)(Args...) noexcept, std::decay_t<F>>)and(
          !std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>)>*>
  constexpr void Callable<Ret(Args...)>::bind(F&& f) noexcept {
    using T = std::decay_t<F>;
    // static_assert(std::is_nothrow_invocable_r_v<Ret, F, Args...>, "f must be noexcept
    // invocable.");
    static_assert(sizeof(T) <= sizeof(buffer_), "sizeof(f) must be smaller than the buffer size.");
    static_assert(std::is_trivially_destructible_v<T>, "F must be trivially destructible.");
    static_assert(std::is_trivially_move_constructible_v<T>,
                  "F must be trivially move constructible.");
    static_assert(std::is_trivially_copyable_v<T>, "F must be trivially copy constructible.");
    new (buffer_.buffer) T(std::forward<F>(f));
    invoke_ = &inline_invoke<T>;
  }

} // namespace sgl
#endif /* SGL_IMPL_CALLABLE_IMPL_HPP */
