#include "catch2/catch.hpp"
#include "sgl_callable.hpp"

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
        c = Call(f);
        return c;
      };
      static_assert(Call{copy_assign(lambda)}() == lambda(), "");
    }
    SECTION("constexpr copy assignment from callable") {
      constexpr auto assign = [](const Call& f) noexcept -> Call {
        Call c;
        c = f;
        return c;
      };
      STATIC_REQUIRE(assign(Call{lambda})() == lambda());
    }
    SECTION("constexpr move assignment from callable") {
      constexpr auto move_assign = [](Call&& f) noexcept -> Call {
        Call c;
        c = std::move(f);
        return c;
      };
      STATIC_REQUIRE(move_assign(std::move(Call{lambda}))() == lambda());
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
  SECTION("construction") {
    SECTION("default") {
      Call c{};
      REQUIRE(c() == 0);
    }
    SECTION("free function") {
      Call c{+lambda};
      REQUIRE(c() == lambda());
    }
    SECTION("lambda") {
      Call c{lambda};
      REQUIRE(c() == lambda());
    }
    SECTION("capturing lambda") {
      Call c{capture_lambda};
      REQUIRE(c() == capture_lambda());
    }
  }
  SECTION("assignment") {
    Call a;
    Call b{lambda};
    SECTION("copy assignment") {
      a = b;
      REQUIRE(a() == b());
    }
    SECTION("move assignment") {
      a = std::move(b);
      REQUIRE(a() == b());
    }
  }

  SECTION("binding") {
    Call a;
    SECTION("lambda") {
      a.bind(lambda);
      REQUIRE(a() == lambda());
    }
    SECTION("free function") {
      a.bind(+lambda);
      REQUIRE(a() == lambda());
    }
    SECTION("capturing lambda") {
      a.bind(capture_lambda);
      REQUIRE(a() == capture_lambda());
    }
  }
  SECTION("reset") {
    Call a(lambda);
    REQUIRE(a() == lambda());
    a.reset();
    REQUIRE(a() != lambda());
    a.bind(capture_lambda);
    REQUIRE(a() == capture_lambda());
    a.reset();
    REQUIRE(a() != capture_lambda());
  }
}
