#include "sgl/named_tuple.hpp"

#include <iostream>
using namespace sgl::string_view_literals;
#include <iostream>
using namespace sgl;
constexpr auto name1 = NAME("name1");

int main() {
  auto a1 = NAME("arg1");
  auto a2 = NAME("arg2");

  auto tuple = NamedTuple(NAME("arg1") <<= 5, a2 <<= 1.15, name1 <<= 5.3f);

  std::cout << tuple[a1] << std::endl
            << tuple[a2] << std::endl
            << tuple[name1] << std::endl
            << std::endl;

  std::cout << tuple[NAME("arg1")] << std::endl
            << tuple[NAME("arg2")] << std::endl
            << tuple[NAME("name1")] << std::endl
            << std::endl;

  std::cout << tuple.get<0>() << std::endl
            << tuple.get<1>() << std::endl
            << tuple.get<2>() << std::endl
            << std::endl;

  std::cout << sgl::get<0>(tuple) << std::endl
            << sgl::get<1>(tuple) << std::endl
            << sgl::get<2>(tuple) << std::endl
            << std::endl;
  tuple.for_each([](const auto& elem) { std::cout << elem << std::endl; });
  std::cout << std::endl;
  tuple.for_each([](auto& elem) { std::cout << elem << std::endl; });
  std::cout << std::endl;
  tuple.for_each_with_name([](sgl::string_view<char> name, const auto& elem) {
    std::cout << name.data() << ": " << elem << std::endl;
  });
  std::cout << std::endl;
  tuple.for_each_with_name([](sgl::string_view<char> name, auto& elem) {
    std::cout << name.data() << ": " << elem << std::endl;
  });
}