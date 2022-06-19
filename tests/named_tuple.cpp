#include "../include/sgl_named_tuple.hpp"
#include "../include/sgl_string_view.hpp"

static constexpr sgl::string_view<char> one{"one"};
static constexpr sgl::string_view<char> two{"two"};
static constexpr sgl::string_view<char> three{"three"};
static constexpr sgl::string_view<char> other_one{"one"};
using namespace sgl::string_view_literals;
template <const auto& name>
struct value {};
template <const auto& name, size_t N>
struct other_value {};
template <const auto& n1, const auto& n2>
constexpr bool operator==(value<n1>, value<n2>) {
  return n1 == n2;
}
using V1 = value<one>;
using V2 = value<two>;
using V3 = value<three>;
using VO1 = other_value<one, 0>;
using VO2 = other_value<two, 0>;
using VO3 = other_value<three, 5>;

struct S1 {};
struct S2 {};
struct S3 {};

int main() {
  auto t = sgl::make_named_tuple<one, two, three>(V1{}, V2{}, V3{});
  auto tuple2 = sgl::make_named_tuple<one, two, three>(S1{}, S2{}, S3{});
  static_assert(std::is_same_v<decltype(t.get<one>()), decltype(t.get<other_one>())>, "");
  auto t1 = t.get<one>();
  auto t2 = t.get<other_one>();
  return t1 == t2;
}