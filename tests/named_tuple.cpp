#include "../include/sgl_named_tuple.hpp"

#include <iostream>
using namespace sgl::string_view_literals;
#include <iostream>
constexpr auto name1 = IDX("name1");
int            main() {
  auto             a1 = IDX("arg1");
  auto             a2 = IDX("arg2");
  sgl::named_tuple tuple{sgl::Arg{a1, 5}, sgl::Arg{a2, 1.15}, sgl::Arg{name1, 5.4f}};

  std::cout << tuple[a1] << std::endl
            << tuple[a2] << std::endl
            << tuple[name1] << std::endl
            << std::endl;

  std::cout << tuple.get<0>() << std::endl
            << tuple.get<1>() << std::endl
            << tuple.get<2>() << std::endl
            << std::endl;

  std::cout << sgl::get<0>(tuple) << std::endl
            << sgl::get<1>(tuple) << std::endl
            << sgl::get<2>(tuple) << std::endl
            << std::endl;
}