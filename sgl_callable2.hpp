#ifndef CALLABLE2_HPP
#define CALLABLE2_HPP
#include <cstring>
#include <new>
#include <type_traits>
namespace sgl {
  template <typename Ret, typename F, typename = void, typename... Args>
  struct is_const_invocable : std::false_type {};
  template <typename Ret, typename F, typename... Args>
  struct is_const_invocable<
      Ret,
      F,
      std::enable_if_t<std::is_invocable_r_v<Ret, const F, Args...>>,
      Args...> : std::true_type {};
  template <typename Ret, typename F, typename... Args>
  static constexpr bool is_const_invocable_v =
      is_const_invocable<Ret, F, void, Args...>::value;
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
    constexpr Callable() : buffer_{.func = nullptr} {}

    constexpr Callable(Ret (*f)(Args...))
        : invoke_(&free_function_invoke), buffer_{.func = f} {}

    constexpr Callable(Ret (&f)(Args...))
        : invoke_(&free_function_invoke), buffer_{.func = &f} {}

    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...)) {
      bind(obj, member_function);
    }

    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) const) {
      bind(obj, member_function);
    }

    template <typename F,
              std::enable_if_t<!std::is_convertible_v<F, Ret (*)(Args...)>>* =
                  nullptr>
    Callable(F&& f) {
      bind(std::forward<F>(f));
    }

    /// invoke delegate
    Ret operator()(Args... args) const { return invoke_(&buffer_, args...); }

    /// bind free function
    void bind(Ret (*free_function)(Args...)) {
      buffer_.func = free_function;
      invoke_ = &free_function_invoke;
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
      static_assert(sizeof(mfn<T>) <= sizeof(buffer_), "");
      new (buffer_.buffer) mfn<T>{&obj, member_function};
      invoke_ = &inline_invoke<mfn<T>>;
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
      static_assert(sizeof(cmfn<T>) <= sizeof(buffer_), "");
      new (buffer_.buffer) cmfn<T>{&obj, member_function};
      invoke_ = &inline_invoke<cmfn<T>>;
    }

    /**
     * @brief bind invocable
     *
     * @tparam F invocable type
     * @param f invocable instance
     */
    template <typename F,
              std::enable_if_t<!std::is_convertible_v<F, Ret (*)(Args...)>>* =
                  nullptr>
    void bind(F&& f) {
      static_assert(std::is_invocable_r_v<Ret, F, Args...>, "");
      static_assert(sizeof(F) <= sizeof(buffer_), "");
      static_assert(std::is_trivially_destructible_v<F>, "");
      if constexpr (is_const_invocable_v<Ret,
                                         std::remove_reference_t<F>,
                                         Args...>) {
        new (buffer_.buffer) std::decay_t<F>(std::forward<F>(f));
        invoke_ = &inline_invoke<std::decay_t<F>>;
      } else {
        new (buffer_.buffer) std::decay_t<F>(std::forward<F>(f));
        invoke_ = &const_inline_invoke<std::decay_t<F>>;
      }
    }

    void reset() { invoke_ = &null_invoke; }

  private:
    template <typename T>
    struct mfn {
      T* t;
      Ret (T::*member)(Args...);
      Ret operator()(Args... args) const {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    template <typename T>
    struct cmfn {
      T* t;
      Ret (T::*member)(Args...) const;
      Ret operator()(Args... args) const {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    union Storage {
      Ret (*func)(Args...);
      char buffer[16];
    };

    static Ret null_invoke(const Storage*, Args...) {
      static Ret r{};
      return r;
    }

    static Ret free_function_invoke(const Storage* storage, Args... args) {
      return static_cast<Ret>((*(storage->func))(args...));
    }

    template <typename T>
    static Ret inline_invoke(const Storage* storage, Args... args) {
      return static_cast<Ret>((*static_cast<const T*>(
          static_cast<const void*>(storage->buffer)))(args...));
    }

    template <typename F>
    static Ret const_inline_invoke(const Storage* storage, Args... args) {
      return static_cast<Ret>(
          (*reinterpret_cast<F*>(const_cast<char*>(storage->buffer)))(args...));
    }

    // data members
    Ret (*invoke_)(const Storage*, Args...){&null_invoke};
    Storage buffer_;
  };
} // namespace sgl
#endif