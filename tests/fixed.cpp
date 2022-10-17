#include "sgl/fix_point.hpp"
#include "sgl/format.hpp"

#include <catch2/catch.hpp>
#include <iostream>

 sgl::unsigned_fixed<3, 4> f1(7.75234);
 sgl::unsigned_fixed<3, 6> f2(3.5);
 sgl::unsigned_fixed<3, 6> f3(7.75234);
 sgl::unsigned_fixed<3, 4> f4(3.5);
TEST_CASE("fixpoint") {
  auto                i0 = ryu::cx::to_bits(264e100);
  auto                i2 = ryu::cx::to_bits(1.0);
  auto                i3 = ryu::cx::to_bits(123451.23);
  char                string[64]{0};
  sgl::unsigned_fixed div_res = f1 / f2;
  sgl::unsigned_fixed mul_res = f1 * f2;
  sgl::unsigned_fixed add_res = f1 + f2;
  sgl::unsigned_fixed sub_res = f1 - f2;
  std::cout << decltype(sgl::unsigned_fixed<10, 5>(52.0) /
                        sgl::unsigned_fixed<1, 10>(0.674))::num_int_digits
            << ", "
            << decltype(sgl::unsigned_fixed<10, 22>(52.0) /
                        sgl::unsigned_fixed<10, 22>(0.674))::num_frac_digits
            << std::endl;
  std::cout << "f1: " << sgl::to_double(f1) << std::endl;
  std::cout << "f2: " << sgl::to_double(f2) << std::endl;
  std::cout << "f3: " << sgl::to_double(f3) << std::endl;
  std::cout << "f4: " << sgl::to_double(f4) << std::endl;
  std::cout << "div: " << sgl::to_double(div_res) << std::endl;
  std::cout << "52.25 / 0.674 = "
            << sgl::to_double(sgl::unsigned_fixed<10, 5>(52.0) / sgl::unsigned_fixed<10, 10>(0.674))
            << std::endl;
  std::cout << "f3 / f4 = " << sgl::to_double(f3 / f4) << std::endl;
  std::cout << "mul: " << sgl::to_double(mul_res) << std::endl;
  std::cout << "add: " << sgl::to_double(add_res) << std::endl;
  std::cout << "sub: " << sgl::to_double(sub_res) << std::endl;
  auto res = sgl::to_chars(string, 64, f1, 2, sgl::format::fixed);
  std::cout << string << std::endl;
  res = sgl::to_chars(string, 64, f2, 2, sgl::format::fixed);
  string[res.size] = 0;
  std::cout << string << std::endl;
}
