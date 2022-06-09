#include "sgl_tuple.hpp"
using namespace sgl;
int main() {
  tuple<int, char, double> t;
  constexpr int            a = 1, b = 2;
  constexpr double         c = 3.0;
  auto                     t2 = tuple(a, b, c);
  static_assert(tuple(a, b, c).get<2>() == 3.0, "");
  return 0;
}