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
   * Args... . See std::function, google search for delegates, etc. if the
   * concept is not clear.
   * \warning mutable lambdas have issues because they are mutable. When
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
    /// move constructor
    constexpr Callable(Callable&& other) noexcept;
    /// copy constructor
    constexpr Callable(const Callable& other) noexcept;

    /// Construct callable from free function pointer
    constexpr Callable(Ret (*f)(Args...) noexcept) noexcept;

    /**
     * \brief Construct a new callable from object and member function
     * \tparam T object type
     * \param obj object instance
     * \param member_function pointer to member function
     */
    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept;

    /**
     * \brief Construct a new callable from object and const member function
     * \tparam T object type
     * \param obj object instance
     * \param member_function pointer to member function
     */
    template <typename T>
    Callable(T& obj, Ret (T::*member_function)(Args...) const noexcept) noexcept;

    /**
     * \brief Construct a callable with a function object/functor
     * \tparam F function object type
     * \param f function object instance
     */
    template <
        typename F,
        std::enable_if_t<(!std::is_constructible_v<Ret (*)(Args...) noexcept, std::decay_t<F>>)and(
            !std::is_same_v<Callable<Ret(Args...)>, std::decay_t<F>>)>* = nullptr>
    explicit Callable(F&& f) noexcept(std::is_nothrow_constructible_v<F>);

    /// copy assignment operator
    constexpr Callable& operator=(Callable&& other) noexcept;

    /// move assignment operator
    constexpr Callable& operator=(const Callable& other) noexcept;

    /// invoke delegate
    constexpr Ret operator()(Args... args) const noexcept;

    /// bind free function
    constexpr void bind(Ret (*free_function)(Args...) noexcept) noexcept;

    /// bind free function
    constexpr void bind(Ret (&free_function)(Args...) noexcept) noexcept;

    /// bind other callable, i.e. simply copy other into this.
    constexpr void bind(const Callable& other) noexcept;

    // bind other callable, i.e. simply move other into this.
    constexpr void bind(Callable&& other) noexcept;

    /**
     * \brief bind object and member function
     *
     * \tparam T object type
     * \param obj object instance
     * \param member_function member function pointer
     */
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) noexcept) noexcept;

    /**
     * \brief bind object and const member function
     *
     * \tparam T object type
     * \param obj object instance
     * \param member_function pointer to const qualified member function
     */
    template <typename T>
    void bind(T& obj, Ret (T::*member_function)(Args...) const noexcept) noexcept;

    /**
     * \brief bind capturing lambda/ custom functor
     *
     * \tparam F invocable type
     * \param f invocable instance
     */
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

      constexpr Ret operator()(Args... args) const noexcept {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    template <typename T>
    struct cmfn {
      T* t;
      Ret (T::*member)(Args...) const noexcept;

      constexpr Ret operator()(Args... args) const noexcept {
        return static_cast<Ret>((t->*member)(args...));
      }
    };

    union Storage {
      Ret (*func)(Args...) noexcept;
      char buffer[2 * sizeof(void*)];
    };

    static constexpr Ret null_invoke(const Storage*, Args...) noexcept {
      if constexpr (!std::is_same_v<Ret, void>) {
        return Ret{};
      }
    }

    constexpr static Ret free_function_invoke(const Storage* storage, Args... args) noexcept {
      return static_cast<Ret>((storage->func)(args...));
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
#include "impl/sgl_callable_impl.hpp"
#endif