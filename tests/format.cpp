#include "sgl/format.hpp"

#include "sgl/impl/format_impl.hpp"

#include <catch2/catch.hpp>
using String = sgl::static_string<char, 25>;
using namespace sgl::string_view_literals;

template <typename T, size_t N>
auto format(const T& value, const char (&expected)[N]) {
  String str{25, '\0'};
  auto   result = sgl::to_chars(str.data(), str.size(), value);
  REQUIRE(result.ec == sgl::error::no_error);
  str.resize(result.size);
  REQUIRE(sgl::string_view<char>(expected) == sgl::string_view<char>(str));
}

template <typename T, size_t N>
auto format(const T& value, const char (&expected)[N], size_t precision, sgl::format fmt) {
  String str{25, '\0'};
  auto   result = sgl::to_chars(str.data(), str.size(), value, precision, fmt);
  REQUIRE(result.ec == sgl::error::no_error);
  str.resize(result.size);
  REQUIRE(sgl::string_view<char>(expected) == sgl::string_view<char>(str));
}

template <typename T>
auto format_too_small(const T& value, size_t precision, sgl::format fmt) {
  String str{1, '\0'};
  auto   result = sgl::to_chars(str.data(), str.size(), value, precision, fmt);
  REQUIRE(result.ec == sgl::error::buffer_too_small);
  REQUIRE(str[0] == '\0');
}

template <typename T>
auto format_too_small(const T& value) {
  String str{1, '\0'};
  auto   result = sgl::to_chars(str.data(), str.size(), value);
  REQUIRE(result.ec == sgl::error::buffer_too_small);
  REQUIRE(str[0] == '\0');
}

TEST_CASE("to_chars") {
  SECTION("integer format") {
    SECTION("simple") {
      format(2, "2");
      format(3u, "3");
      format(0, "0");
      format(12345u, "12345");
      format(-12345, "-12345");
    }
    SECTION("too_small") {
      format_too_small(20000);
      format_too_small(300u);
      format_too_small(10);
      format_too_small(12345u);
      format_too_small(-12345);
    }
  }
  SECTION("floating point format") {
    SECTION("double") {
      SECTION("integer") {
        format(2.0, "2", 10, sgl::format::integer);
        format(3.0, "3", 10, sgl::format::integer);
        format(52.4, "52", 10, sgl::format::integer);
        format(52.5, "53", 10, sgl::format::integer);
      }
      SECTION("floating") {
        format(52.5, "5.25E1", 10, sgl::format::floating);
        format(0.0, "0E0", 10, sgl::format::floating);
        format(52.52525252525252525, "5.2525252525252526E1", 10, sgl::format::floating);
      }
      SECTION("exponential") {
        format(52.5, "5.2500000000e+01", 10, sgl::format::exponential);
        format(0.0, "0.0000000000e+00", 10, sgl::format::exponential);
        format(52.52525252525252525, "5.2525252525e+01", 10, sgl::format::exponential);
      }
      SECTION("fixed") {
        format(52.5, "52.5000000000", 10, sgl::format::fixed);
        format(0.0, "0.0000000000", 10, sgl::format::fixed);
        format(52.52525252525252525, "52.5252525253", 10, sgl::format::fixed);
      }
      SECTION("hex") {
        format(2.0, "0x4000000000000000", 10, sgl::format::hex);
        format(12345.25, "0x40C81CA000000000", 10, sgl::format::hex);
      }
      SECTION("too_small") {

        format_too_small(52.5, 10, sgl::format::floating);
        format_too_small(0.0, 10, sgl::format::floating);
        format_too_small(52.52525252525252525, 10, sgl::format::floating);

        format_too_small(52.5, 10, sgl::format::exponential);
        format_too_small(0.0, 10, sgl::format::exponential);
        format_too_small(52.52525252525252525, 10, sgl::format::exponential);

        format_too_small(52.5, 10, sgl::format::fixed);
        format_too_small(0.0, 10, sgl::format::fixed);
        format_too_small(52.52525252525252525, 10, sgl::format::fixed);
      }
    }
    SECTION("float") {
      SECTION("integer") {
        format(2.0f, "2", 10, sgl::format::integer);
        format(3.0f, "3", 10, sgl::format::integer);
        format(52.4f, "52", 10, sgl::format::integer);
        format(52.5f, "53", 10, sgl::format::integer);
      }
      SECTION("floating") {
        format(52.5f, "5.25E1", 10, sgl::format::floating);
        format(0.0f, "0E0", 10, sgl::format::floating);
        format(52.52525252525252525f, "5.2525253E1", 10, sgl::format::floating);
      }
      SECTION("exponential") {
        format(52.5f, "5.2500000000e+01", 10, sgl::format::exponential);
        format(0.0f, "0.0000000000e+00", 10, sgl::format::exponential);
        format(52.5252525f, "5.2525253e+01", 7, sgl::format::exponential);
      }
      SECTION("fixed") {

        format(52.5f, "52.5000000000", 10, sgl::format::fixed);
        format(0.0f, "0.0000000000", 10, sgl::format::fixed);
        format(52.5252525f, "52.525253", 6, sgl::format::fixed);
      }
      SECTION("hex") {
        format(2.0f, "0x40000000", 10, sgl::format::hex);
        format(12345.25f, "0x4640E500", 10, sgl::format::hex);
      }
      SECTION("too_small") {

        format_too_small(52.5f, 10, sgl::format::floating);
        format_too_small(0.0f, 10, sgl::format::floating);
        format_too_small(52.52525252525252525f, 10, sgl::format::floating);

        format_too_small(52.5f, 10, sgl::format::exponential);
        format_too_small(0.0f, 10, sgl::format::exponential);
        format_too_small(52.52525252525252525f, 10, sgl::format::exponential);

        format_too_small(52.5f, 10, sgl::format::fixed);
        format_too_small(0.0f, 10, sgl::format::fixed);
        format_too_small(52.52525252525252525f, 10, sgl::format::fixed);
      }
    }
  }
  SECTION("unsigned_fixed format") {
    SECTION("integer") {
      format(sgl::unsigned_fixed<16, 16>(0u), "0", 1, sgl::format::integer);
      format(sgl::unsigned_fixed<16, 16>(52.5), "53", 1, sgl::format::integer);
      format(sgl::unsigned_fixed<16, 16>(52.4), "52", 10, sgl::format::integer);
    }
    SECTION("floating") {
      format(sgl::unsigned_fixed<16, 16>(52.5), "5.25E1", 10, sgl::format::floating);
    }
    SECTION("exponential") {
      format(sgl::unsigned_fixed<16, 16>(52.5), "5.2500000000e+01", 10, sgl::format::exponential);
    }
    SECTION("fixed") {
      format(sgl::unsigned_fixed<16, 16>(0.5), "0.5000000000", 10, sgl::format::fixed);
      format(sgl::unsigned_fixed<16, 16>(52.5), "52.5000000000", 10, sgl::format::fixed);
    }
    SECTION("too small") {
      format_too_small(sgl::unsigned_fixed<16, 16>(52.5), 1, sgl::format::integer);
      format_too_small(sgl::unsigned_fixed<16, 16>(52.4), 10, sgl::format::integer);
      format_too_small(sgl::unsigned_fixed<16, 16>(52.5), 10, sgl::format::floating);
      format_too_small(sgl::unsigned_fixed<16, 16>(52.5), 10, sgl::format::exponential);
      format_too_small(sgl::unsigned_fixed<16, 16>(0.5), 10, sgl::format::fixed);
      format_too_small(sgl::unsigned_fixed<16, 16>(52.5), 10, sgl::format::fixed);
    }
  }
  SECTION("signed_fixed format") {
    SECTION("integer") {
      format(sgl::signed_fixed<16, 16>(0u), "0", 1, sgl::format::integer);
      format(sgl::signed_fixed<16, 16>(52.5), "53", 1, sgl::format::integer);
      format(sgl::signed_fixed<16, 16>(52.4), "52", 10, sgl::format::integer);
      format(sgl::signed_fixed<16, 16>(-52.4), "-52", 10, sgl::format::integer);
      format(sgl::signed_fixed<16, 16>(-52.5), "-53", 10, sgl::format::integer);
    }
    SECTION("floating") {
      format(sgl::signed_fixed<16, 16>(52.5), "5.25E1", 10, sgl::format::floating);
      format(sgl::signed_fixed<16, 16>(-52.5), "-5.25E1", 10, sgl::format::floating);
      format(sgl::signed_fixed<16, 16>(-0.5), "-5E-1", 10, sgl::format::floating);
    }
    SECTION("exponential") {
      format(sgl::signed_fixed<16, 16>(52.5), "5.2500000000e+01", 10, sgl::format::exponential);
      format(sgl::signed_fixed<16, 16>(-0.5), "-5.0000000000e-01", 10, sgl::format::exponential);
    }
    SECTION("fixed") {
      format(sgl::signed_fixed<16, 16>(0.5), "0.5000000000", 10, sgl::format::fixed);
      format(sgl::signed_fixed<16, 16>(-0.5), "-0.5000000000", 10, sgl::format::fixed);
      format(sgl::signed_fixed<16, 16>(52.5), "52.5000000000", 10, sgl::format::fixed);
      format(sgl::signed_fixed<16, 16>(52.5), "52.5000000000000000000", 19, sgl::format::fixed);
    }
    SECTION("too small") {
      format_too_small(sgl::signed_fixed<16, 16>(52.5), 1, sgl::format::integer);
      format_too_small(sgl::signed_fixed<16, 16>(52.4), 10, sgl::format::integer);
      format_too_small(sgl::signed_fixed<16, 16>(-52.5), 10, sgl::format::floating);
      format_too_small(sgl::signed_fixed<16, 16>(-0.5), 10, sgl::format::floating);
      format_too_small(sgl::signed_fixed<16, 16>(52.5), 10, sgl::format::floating);
      format_too_small(sgl::signed_fixed<16, 16>(52.5), 10, sgl::format::exponential);
      format_too_small(sgl::signed_fixed<16, 16>(0.5), 10, sgl::format::fixed);
      format_too_small(sgl::signed_fixed<16, 16>(52.5), 10, sgl::format::fixed);
      format_too_small(sgl::signed_fixed<16, 16>(52.5), 19, sgl::format::fixed);
    }
  }
}
