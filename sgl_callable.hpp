#ifndef CALLABLE_HPP
#define CALLABLE_HPP
#include <cstring>
#include <new>
#include <type_traits>
namespace sgl {
  /// @cond
  // intentional forward declaration only. specialization follows below.
  template <typename Signature>
  class Callable;
  /// @endcond

  /**
   * @brief This class implements a callable with the guarantee that it does not
   * heap allocate.
   * @details This class can bind and execute function objects/functors/lambdas,
   * free functions and member functions with a return type of Ret and arguments
   * Args... . See std::function, google search for delegates, etc. if the
   * concept is not clear.
   * @warning mutable lambdas have a quirk because they are mutable. When
   * invoking a Callable storing a mutable lambda, there is undefined behaviour
   * if the Callable's type is const! Below is a quick code example of what is
   * ok and what is not.
   * ```cpp
   * #include "sgl_callable2.hpp"
   * using Call_t = sgl::Callable<int(void)>;
   *
   * int main(){
   *   int a = 5,b=10;
   *   auto lambda = [=]() mutable { return (++a) + (--b);};
   *   Call_t callable(lambda);
   *   int ret = callable(); // ok, callable is not const.
   *
   *   const Call_t c2(lambda);
   *   ret = c2() // will result in undefined behaviour, since c2's type is
   *              // const.
   *  return 0;
   * }
   * ```
   * This also means, that invoking the callable in a const method of a
   * class/struct with a callable as a member or bound to a const reference in a
   * free function, will result in undefined behaviour. See
   * https://en.cppreference.com/w/cpp/language/const_cast for more info.
   * @tparam Ret return type of the callable
   * @tparam Args argument types
   */
  template <typename Ret, typename... Args>
  class Callable<Ret(Args...)> {
  public:
    /// default constructor
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

    /// invoke delegate
    Ret operator()(Args... args) { return invoke(buffer, args...); }

    /// bind free function
    void bind(Ret (*free_function)(Args...)) {
      memcpy_s(buffer, sizeof(buffer), &free_function, sizeof(free_function));
      invoke = &free_function_invoke;
    }

    /// bind free function
    void bind(Ret (&free_function)(Args...)) { bind(&free_function); }

    /**
     * @brief bind object and member function
     *
     * @tparam T object type
     * @param obj object instance
     * @param member_function member function pointer
     */
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...)) {
      static_assert(sizeof(mfn<T>) <= sizeof(buffer), "");
      new (buffer) mfn<T>{&obj, member_function};
      invoke = &inline_invoke<mfn<T>>;
    }

    /**
     * @brief bind object and const member function
     *
     * @tparam T object type
     * @param obj object instance
     * @param member_function pointer to const qualified member function
     */
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) const) {
      static_assert(sizeof(cmfn<T>) <= sizeof(buffer), "");
      new (buffer) cmfn<T>{&obj, member_function};
      invoke = &inline_invoke<cmfn<T>>;
    }

    /**
     * @brief bind functor
     *
     * @tparam F functor type
     * @param f functor instance
     */
    template <typename F,
              std::enable_if_t<!std::is_convertible_v<F, Ret (*)(Args...)>>* =
                  nullptr>
    void bind(F&& f) {
      static_assert(std::is_invocable_r_v<Ret, F, Args...>, "");
      static_assert(sizeof(F) <= sizeof(buffer), "");

      new (buffer) std::decay_t<F>(std::forward<F>(f));
      invoke = &inline_invoke<std::decay_t<F>>;
    }

  private:
    template <typename T>
    struct mfn {
      T* t;
      Ret (T::*member)(Args...);
      Ret operator()(Args... args) {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    template <typename T>
    struct cmfn {
      T* t;
      Ret (T::*member)(Args...) const;
      Ret operator()(Args... args) {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    static Ret null_invoke(void*, Args...) {
      static Ret r{};
      return r;
    }
    static Ret free_function_invoke(void* buf, Args... args) {
      using type = Ret (*)(Args...);
      return static_cast<Ret>((*static_cast<type*>(buf))(args...));
    }
    template <typename T>
    static Ret inline_invoke(void* buf, Args... args) {
      return static_cast<Ret>((*static_cast<T*>(buf))(args...));
    }

    // data members
    Ret (*invoke)(void*, Args...){null_invoke};
    char buffer[16]{0};
  };
} // namespace sgl
#endif