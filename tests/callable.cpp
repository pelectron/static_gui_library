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
  STATIC_REQUIRE(Call{[]() noexcept { return the_answer<TestType>; }}() == the_answer<TestType>);
  STATIC_REQUIRE(Call{get_answer<TestType>}() == the_answer<TestType>);
  STATIC_REQUIRE(Call{&get_answer<TestType>}() == the_answer<TestType>);
  TestType val{25};
  auto     lambda1 = [val]() noexcept -> TestType { return val + the_answer<TestType>; };
  auto     lambda2 = [&val]() noexcept -> TestType { return the_answer<TestType> - val; };
  static_assert(sgl::is_trivially_destructible_v<decltype(lambda2)>,"");
  Call     call(lambda1);
  REQUIRE(call() == lambda1());
  call.bind(lambda2);
  REQUIRE(call() == lambda2());
  call.reset();
  REQUIRE(call() == TestType{0});
  GIVEN("two callables"){
    Call call1(lambda1);
    Call call2;
    WHEN("copy assigning"){
      call2 = call1;
      REQUIRE(call2()==call1());
    }
    WHEN("copy constructing"){
      Call call3(call1);
      REQUIRE(call3()==call1());
    }
  }
}
SCENARIO("Testing sgl::callable") {
  STATIC_REQUIRE(sgl::Callable<int(void)>([]() noexcept { return the_answer<int>; })() ==
                 the_answer<int>);
}