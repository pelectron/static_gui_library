//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#include "sgl/named_tuple.hpp"

#include <catch2/catch.hpp>

using namespace sgl::string_view_literals;

struct Value {
  bool visited = false;
};

TEST_CASE("NamedTuple") {
  constexpr auto n1 = NAME("n1");
  constexpr auto n2 = NAME("n2");
  constexpr auto n3 = NAME("n3");

  using Tuple =
      sgl::NamedTuple<sgl::type_list<sgl::Name<'n', '1'>, sgl::Name<'n', '2'>, sgl::Name<'n', '3'>>,
                      sgl::type_list<int, char, double>>;
  SECTION("Construction from named values") {
    auto tuple = sgl::NamedTuple(n1 <<= 1, n2 <<= 'x', n3 <<= 2.0);
    REQUIRE(tuple[n1] == 1);
    REQUIRE(tuple[n2] == 'x');
    REQUIRE(tuple[n3] == 2.0);
  }
  SECTION("construction from values") {
    Tuple tuple(1, 'x', 2.0);
    REQUIRE(tuple[n1] == 1);
    REQUIRE(tuple[n2] == 'x');
    REQUIRE(tuple[n3] == 2.0);
  }
  SECTION("all access methods return the same value") {
    auto tuple = sgl::NamedTuple(n1 <<= 1, n2 <<= 'x', n3 <<= 2.0);

    REQUIRE(tuple[n1] == tuple.get<0>());
    REQUIRE(tuple[n1] == sgl::get<0>(tuple));
    REQUIRE(tuple[n1] == sgl::get(n1, tuple));
    REQUIRE(&tuple[n1] == &tuple.get<0>());
    REQUIRE(&tuple[n1] == &sgl::get<0>(tuple));
    REQUIRE(&tuple[n1] == &sgl::get(n1, tuple));

    REQUIRE(tuple[n2] == tuple.get<1>());
    REQUIRE(tuple[n2] == sgl::get<1>(tuple));
    REQUIRE(tuple[n2] == sgl::get(n2, tuple));
    REQUIRE(&tuple[n2] == &tuple.get<1>());
    REQUIRE(&tuple[n2] == &sgl::get<1>(tuple));
    REQUIRE(&tuple[n2] == &sgl::get(n2, tuple));

    REQUIRE(tuple[n3] == tuple.get<2>());
    REQUIRE(tuple[n3] == sgl::get<2>(tuple));
    REQUIRE(tuple[n3] == sgl::get(n3, tuple));
    REQUIRE(&tuple[n3] == &tuple.get<2>());
    REQUIRE(&tuple[n3] == &sgl::get<2>(tuple));
    REQUIRE(&tuple[n3] == &sgl::get(n3, tuple));
  }
  SECTION("foreach") {
    // TODO: test for each
    using Tuple2 = sgl::NamedTuple<
        sgl::type_list<sgl::Name<'n', '1'>, sgl::Name<'n', '2'>, sgl::Name<'n', '3'>>,
        sgl::type_list<Value, Value, Value>>;

    Tuple2 t2{};

    t2.for_each([](Value& v) { v.visited = true; });
    REQUIRE(t2[n1].visited);
    REQUIRE(t2[n2].visited);
    REQUIRE(t2[n3].visited);
    
    t2.for_each([](Value& v) { v.visited = false; });
    REQUIRE_FALSE(t2[n1].visited);
    REQUIRE_FALSE(t2[n2].visited);
    REQUIRE_FALSE(t2[n3].visited);

    sgl::for_each(t2, [](Value& v) { v.visited = true; });
    REQUIRE(t2[n1].visited);
    REQUIRE(t2[n2].visited);
    REQUIRE(t2[n3].visited);
  }

  SECTION("for_each_with_name") {
    // TODO: test for each with name
  }
}
