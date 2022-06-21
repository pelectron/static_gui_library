#include "../include/sgl_named_tuple.hpp"

#include <iostream>
using namespace sgl::string_view_literals;
#include <iostream>

int main() {
  auto             a1 = CSTRING("arg1");
  auto             a2 = CSTRING("arg2");
  sgl::named_tuple tuple{sgl::Arg{a1, 5}, sgl::Arg{a2, 1.0}};
  std::cout << tuple[a1] << std::endl << tuple[a2] << std::endl;
}