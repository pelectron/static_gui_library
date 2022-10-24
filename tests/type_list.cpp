#include "sgl/type_list.hpp"

#include <catch2/catch.hpp>
#include <limits>

template <typename... Ts>
struct apply_test {};

TEST_CASE("type_list") {
  SECTION("head") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    STATIC_REQUIRE(std::is_same_v<int, sgl::head_t<list>>);
  }
  SECTION("tail") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    STATIC_REQUIRE(
        std::is_same_v<sgl::type_list<char, double, float, unsigned>, sgl::tail_t<list>>);
  }
  SECTION("list_size") {
    using l1 = sgl::type_list<int, char, double, float, unsigned>;
    using l2 = sgl::type_list<int, double>;
    using l3 = sgl::type_list<int>;
    using l4 = sgl::type_list<>;
    STATIC_REQUIRE(sgl::list_size_v<l1> == 5);
    STATIC_REQUIRE(sgl::list_size_v<l2> == 2);
    STATIC_REQUIRE(sgl::list_size_v<l3> == 1);
    STATIC_REQUIRE(sgl::list_size_v<l4> == 0);
  }
  SECTION("index_of") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    STATIC_REQUIRE(sgl::index_of_v<int, list> == 0);
    STATIC_REQUIRE(sgl::index_of_v<char, list> == 1);
    STATIC_REQUIRE(sgl::index_of_v<double, list> == 2);
    STATIC_REQUIRE(sgl::index_of_v<float, list> == 3);
    STATIC_REQUIRE(sgl::index_of_v<unsigned, list> == 4);
    STATIC_REQUIRE(sgl::index_of_v<short, list> == std::numeric_limits<size_t>::max());
  }
  SECTION("push_front") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    using pushed_list = sgl::push_front_t<short, list>;
    using expected_list = sgl::type_list<short, int, char, double, float, unsigned>;
    STATIC_REQUIRE(std::is_same_v<expected_list, pushed_list>);
  }
  SECTION("pop_front") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    using expected = sgl::type_list<char, double, float, unsigned>;
    using result = sgl::pop_front_t<list>;
    STATIC_REQUIRE(std::is_same_v<result, expected>);
  }
  SECTION("push_back") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    using pushed_list = sgl::push_back_t<short, list>;
    using expected_list = sgl::type_list<int, char, double, float, unsigned, short>;
    STATIC_REQUIRE(std::is_same_v<expected_list, pushed_list>);
  }
  SECTION("pop_back") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    using expected = sgl::type_list<int, char, double, float>;
    using result = sgl::pop_back_t<list>;
    STATIC_REQUIRE(std::is_same_v<expected, result>);
  }
  SECTION("type_at") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    STATIC_REQUIRE(std::is_same_v<int, sgl::type_at_t<0, list>>);
    STATIC_REQUIRE(std::is_same_v<char, sgl::type_at_t<1, list>>);
    STATIC_REQUIRE(std::is_same_v<double, sgl::type_at_t<2, list>>);
    STATIC_REQUIRE(std::is_same_v<float, sgl::type_at_t<3, list>>);
    STATIC_REQUIRE(std::is_same_v<unsigned, sgl::type_at_t<4, list>>);
  }
  SECTION("apply") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    using expected = apply_test<int, char, double, float, unsigned>;
    using result = sgl::apply_t<apply_test, list>;
    STATIC_REQUIRE(std::is_same_v<expected, result>);
  }
  SECTION("contains") {
    using list = sgl::type_list<int, char, double, float, unsigned>;
    STATIC_REQUIRE(sgl::contains_v<int, list>);
    STATIC_REQUIRE(sgl::contains_v<char, list>);
    STATIC_REQUIRE(sgl::contains_v<double, list>);
    STATIC_REQUIRE(sgl::contains_v<float, list>);
    STATIC_REQUIRE(sgl::contains_v<unsigned, list>);
    STATIC_REQUIRE_FALSE(sgl::contains_v<short, list>);
    STATIC_REQUIRE_FALSE(sgl::contains_v<long, list>);
  }
  SECTION("all_unique") {
    STATIC_REQUIRE(sgl::all_unique_v<int, char, double, float, unsigned>);
    STATIC_REQUIRE_FALSE(sgl::all_unique_v<int, char, double, float, int, unsigned>);
  }
  // #TODO: for_each_t
}