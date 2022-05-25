#ifndef CALLABLE_HPP
#define CALLABLE_HPP
#include <cstring>
#include <new>
#include <type_traits>

template <typename>
class Callable;
struct Test {};
template <typename Ret, typename... Args>
class Callable<Ret(Args...)> {
private:
  template <typename T>
  struct mfn {
    T& t;
    Ret (T::*member)(Args...);
    Ret operator()(Args... args) { return (t.(*member))(args...); }
  };
  template <typename T>
  struct cmfn {
    T& t;
    Ret (T::*member)(Args...) const;
    Ret operator()(Args... args) { return (t.(*member))(args...); }
  };

  static Ret null_invoke(void*, Args...) {
    static Ret r{};
    return r;
  }
  static Ret free_function_invoke(void* buf, Args... args) {
    using type = Ret (*)(Args...);
    return (*static_cast<type*>(buf))(args...);
  }
  template <typename T>
  static Ret inline_invoke(void* buf, Args... args) {
    return static_cast<Ret>((*static_cast<T*>(buf))(args...));
  }
  // data members
  Ret (*invoke)(void*, Args...){null_invoke};
  char buffer[16]{0};

public:
  constexpr Callable() {}
  Callable(Ret (*f)(Args...)) { bind(f); }
  Callable(Ret (&f)(Args...)) { bind(f); }
  template <typename T>
  Callable(T& obj, Ret (T::*member_function)(Args...)) {
    bind(obj, member_function);
  }
  template <typename T>
  Callable(T& obj, Ret (T::*member_function)(Args...) const) {
    bind(obj, member_function);
  }
  template <typename F>
  Callable(F&& f) {
    bind(std::forward<F>(f));
  }
  Ret operator()(Args... args) { return invoke(buffer, args...); }

  void bind(Ret (*free_function)(Args...)) {
    memcpy_s(buffer, sizeof(buffer), &free_function, sizeof(free_function));
    invoke = &free_function_invoke;
  }
  void bind(Ret (&free_function)(Args...)) { bind(&free_function); }
  template <typename T>
  void bind(T& obj, Ret (T::*member_function)(Args...)) {
    static_assert(sizeof(mfn<T>) <= sizeof(buffer), "");
    new (buffer) mfn<T>(obj, member_function);
    invoke = &inline_invoke<mfn<T>>;
  }
  template <typename T>
  void bind(T& obj, Ret (T::*member_function)(Args...) const) {
    static_assert(sizeof(cmfn<T>) <= sizeof(buffer), "");
    new (buffer) cmfn<T>(obj, member_function);
    invoke = &inline_invoke<cmfn<T>>;
  }
  template <typename F>
  void bind(F&& f) {
    static_assert(std::is_invocable_r_v<Ret, F, Args...>, "");
    static_assert(sizeof(F) <= sizeof(buffer), "");

    new (buffer) std::decay_t<F>(std::forward<std::decay_t<F>>(f));
    invoke = &inline_invoke<std::decay_t<F>>;
  }
};
#endif