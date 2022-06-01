#include "sgl_callable2.hpp"
using Call = sgl::Callable2<int(int)>;
int free_func(int a) { return 4 * a; }
struct Struct {
  int mfn(int a) { return 8 * a + b; }
  int cmfn(int a) const { return 8 * a + b; }
  int b{5};
};
const auto lambda = [](int a) { return a; };
int        main() {
  Struct a,c;
  auto   capture_lambda = [&a, &c](int k) { return a.b + c.b + k; };
  Struct s{10};
  Call   call1([](int a) { return 2 * a; });
  Call   call2(&free_func);
  Call   call3(s, &Struct::mfn);
  Call   call4(s, &Struct::cmfn);
  Call   call5(lambda);
  Call call6(capture_lambda);
  int b{0};
  if (call2(1) == (2 * call1(1))) {
    b = 1;
  }
  if ((call3(2) == call4(2))) {
    b += 2;
  }
  return b;
}