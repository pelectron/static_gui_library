#ifndef CALLABLE2_HPP
#define CALLABLE2_HPP
#include <new>
#include <type_traits>
#include <utility>
namespace sgl {
  namespace detail {
    template <typename R, typename T, typename... A>
    static constexpr bool is_const_invocable_v =
        std::is_nothrow_invocable_v<R, std::add_const_t<T>, A...>;
  }
  /// \cond
  // intentional forward declaration only. specialization follows below.

  template <typename Signature>
  class Callable;
  /// \endcond

  /**
   * \brief This class implements a callable with the guarantee that it does not
   * heap allocate or throw. Additionally, constexpr fee functions and non capturing lambdas can be
   * executed at compile time, i.e. the following will successfully compile:
   * ```
   * static_assert(Callable<int()>([]() noexcept { return 0;})() == 0,"");
   * ```
   *
   * \details This class can bind and execute function objects/functors/lambdas,
   * free functions and member functions with a return type of Ret and arguments
   * Args... . See function, google search for delegates, etc. if the
   * concept is not clear.
   * \warning mutable lambdas have a issues because they are mutable. When
   * invoking a Callable storing a mutable lambda, there is undefined behaviour
   * if the Callable's type is const! Below is a quick code example of what is
   * ok and what is not.
   * ```cpp
   * #include "sgl_callable2.hpp"
   * using Call_t = sgl::Callable<int(void)>;
   *
   * int main(){
   *   int a = 5,b=10;
   *   auto lambda = [=]() mutable noexcept{ return (++a) + (--b);};
   *   Call_t callable(lambda);
   *   int ret = callable(); // ok, callable's type is not const.
   *
   *   const Call_t c2(lambda);
   *   ret = c2(); // will result in undefined behaviour, since c2's type is
   *              // const.
   *  return 0;
   * }
   * ```
   * https://en.cppreference.com/w/cpp/language/const_cast for more info.
   * \tparam Ret return type of the callable
   * \tparam Args argument types
   */
  template <typename Ret, typename... Args>
  class Callable<Ret(Args...)> {
  public:
    /// Default constructor
    constexpr Callable() noexcept = default;
    constexpr Callable(Callable&& other) noexcept : invoke_(other.invoke_), buffer_(other.buffer_) {
      other.reset();
    }
    constexpr Callable(const Callable& other) noexcept
        : invoke_(other.invoke_), buffer_(other.buffer_) {}

    /// Construct callable from free function pointer
    constexpr Callable(Ret (*f)(Args...) noexcept) noexcept
        : invoke_(&free_function_invoke), buffer_{f} {}

    /**
     * \brief Construct a new callable from object and member function
     * \tparam T object type
     * \param obj object instance
     * \param member_function pointer to member function
     */
    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept {
      bind(obj, member_function);
    }

    /**
     * \brief Construct a new callable from object and const member function
     * \tparam T object type
     * \param obj object instance
     * \param member_function pointer to member function
     */
    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) const noexcept) noexcept {
      bind(obj, member_function);
    }
    /**
     * \brief Construct a callable with a function object/functor
     * \tparam F function object type
     * \param f function object instance
     */
    template <
        typename F,
        std::enable_if_t<!std::is_constructible_v<Ret (*)(Args...) noexcept, std::decay_t<F>> and
                         !std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>>* = nullptr>
    Callable(F&& f) noexcept {
      bind(std::forward<F>(f));
    }

    Callable& operator=(Ret (*func)(Args...) noexcept) noexcept {
      buffer_.func = func;
      invoke_ = &free_function_invoke;
      return *this;
    }

    template <typename F,
              std::enable_if_t<!std::is_constructible_v<Ret (*)(Args...) noexcept, F>>* = nullptr>
    constexpr Callable& operator=(F&& f) noexcept {
      if constexpr (std::is_same_v<std::decay_t<F>, Callable<Ret(Args...)>>) {
        buffer_ = f.buffer_;
        invoke_ = f.invoke_;
      } else {
        this->bind(std::forward<F>(f));
      }
      return *this;
    }

    /// invoke delegate
    constexpr Ret operator()(Args... args) const noexcept { return invoke_(&buffer_, args...); }

    /// bind free function
    constexpr void bind(Ret (*free_function)(Args...) noexcept) noexcept {
      buffer_.func = free_function;
      invoke_ = &free_function_invoke;
    }

    /// bind free function
    constexpr void bind(Ret (&free_function)(Args...) noexcept) noexcept { bind(&free_function); }

    /**
     * \brief bind object and member function
     *
     * \tparam T object type
     * \param obj object instance
     * \param member_function member function pointer
     */
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept {
      static_assert(sizeof(mfn<T>) <= sizeof(buffer_), "");
      new (buffer_.buffer) mfn<T>{&obj, member_function};
      invoke_ = &inline_invoke<mfn<T>>;
    }

    /**
     * \brief bind object and const member function
     *
     * \tparam T object type
     * \param obj object instance
     * \param member_function pointer to const qualified member function
     */
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) const noexcept) noexcept {
      static_assert(sizeof(cmfn<T>) <= sizeof(buffer_), "");
      new (buffer_.buffer) cmfn<T>{&obj, member_function};
      invoke_ = &inline_invoke<cmfn<T>>;
    }

    /**
     * \brief bind invocable
     *
     * \tparam F invocable type
     * \param f invocable instance
     */
    template <typename F>
    void bind(F&& f) noexcept {
      if constexpr (std::is_constructible_v<Ret (*)(Args...) noexcept, F>) {
        using type = Ret (*)(Args...) noexcept;
        bind(type{f});
      } else {

        static_assert(std::is_nothrow_invocable_r_v<Ret, F, Args...>,
                      "f must be noexcept invocable.");
        static_assert(sizeof(std::decay_t<F>) <= sizeof(buffer_),
                      "sizeof(f) must be smaller than the buffer size.");
        static_assert(std::is_trivially_destructible_v<std::decay_t<F>>,
                      "F must be trivially destructible.");
        static_assert(std::is_trivially_move_constructible_v<std::decay_t<F>>,
                      "F must be trivially move constructible.");
        static_assert(std::is_trivially_copy_constructible_v<std::decay_t<F>>,
                      "F must be trivially copy constructible.");
        if constexpr (detail::is_const_invocable_v<Ret, std::remove_reference_t<F>, Args...>) {
          new (buffer_.buffer) std::decay_t<F>(std::forward<F>(f));
          invoke_ = &inline_invoke<std::decay_t<F>>;
        } else {
          new (buffer_.buffer) std::decay_t<F>(std::forward<F>(f));
          invoke_ = &const_inline_invoke<std::decay_t<F>>;
        }
      }
    }

    constexpr void reset() noexcept { invoke_ = &null_invoke; }

  private:
    template <typename T>
    struct mfn {
      T* t;
      Ret (T::*member)(Args...) noexcept;
      constexpr Ret operator()(Args... args) const {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    template <typename T>
    struct cmfn {
      T* t;
      Ret (T::*member)(Args...) const noexcept;
      constexpr Ret operator()(Args... args) const {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    union Storage {
      Ret (*func)(Args...) noexcept;
      char buffer[2 * sizeof(void*)];
    };

    static Ret null_invoke(const Storage*, Args...) noexcept {
      static Ret r{};
      return r;
    }

    constexpr static Ret free_function_invoke(const Storage* storage, Args... args) noexcept {
      return static_cast<Ret>((*(storage->func))(args...));
    }

    template <typename T>
    static Ret inline_invoke(const Storage* storage, Args... args) noexcept {
      return static_cast<Ret>(
          (*static_cast<const T*>(static_cast<const void*>(storage->buffer)))(args...));
    }

    template <typename F>
    static Ret const_inline_invoke(const Storage* storage, Args... args) noexcept {
      return static_cast<Ret>((*reinterpret_cast<F*>(const_cast<char*>(storage->buffer)))(args...));
    }

    // data members
    Ret (*invoke_)(const Storage*, Args...) noexcept {&null_invoke};
    Storage buffer_{nullptr};
  };
} // namespace sgl
#endif