#include "catch2/catch.hpp"
#include "sgl_callable.hpp"

template <typename T>
static constexpr T the_answer = 42;
template <typename T>
constexpr T get_answer() noexcept {
  return the_answer<T>;
}

TEMPLATE_TEST_CASE("Testing sgl::callable",
                   "[callable][template]",
                   int,
                   char,
                   double,
                   float,
                   unsigned) {
  using Call = sgl::Callable<TestType(void)>;
  constexpr auto lambda = []() noexcept -> TestType { return 42; };
  auto           capture_lambda = [lambda]() noexcept -> TestType { return lambda(); };
  SECTION("constexpr test") {
    SECTION("constexpr construction") {
      STATIC_REQUIRE(Call{lambda}() == lambda());
      STATIC_REQUIRE(Call{}() == 0);
    }
    SECTION("constexpr copy assignment") {
      constexpr auto copy_assign = [](auto&& f) noexcept {
        Call c;
        c = f;
        return c;
      };
      static_assert(Call{copy_assign(lambda)}() == lambda(),"");
    }
    SECTION("constexpr copy assignment from callable") {
      constexpr auto assign = [](const Call& f) noexcept -> Call {
        Call c;
        c = f;
        return c;
      };
      STATIC_REQUIRE(Call{assign(Call{lambda})}() == lambda());
    }
    SECTION("constexpr move assignment from callable") {
      constexpr auto move_assign = [](Call&& f) noexcept -> Call {
        Call c;
        c = std::move(f);
        return c;
      };
      STATIC_REQUIRE(Call{std::move(move_assign(std::move(Call{lambda})))}() == lambda());
    }
    SECTION("constexpr binding") {
      constexpr auto bind = [](auto&& f) {
        Call c;
        c.bind(f);
        return c;
      };
      STATIC_REQUIRE(bind(lambda)() == lambda());
      STATIC_REQUIRE(bind(Call{lambda})() == lambda());
    }
  }
  REQUIRE(Call{capture_lambda}() == capture_lambda());
  GIVEN("two callables") {
    Call call1(lambda);
    Call call2;
    WHEN("copy assigning") {
      call2 = call1;
      REQUIRE(call2() == call1());
    }
    WHEN("copy constructing") {
      Call call3(call1);
      REQUIRE(call3() == call1());
    }
  }
}
SCENARIO("Testing sgl::callable") {
  STATIC_REQUIRE(sgl::Callable<int(void)>([]() noexcept { return the_answer<int>; })() ==
                 the_answer<int>);
}