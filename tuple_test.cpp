#include "tuple.hpp"

#include <iostream>

using seq = sgl::index_sequence_for<int, char, double>;
using seq2 = sgl::index_sequence_for<int, char, double, float, unsigned>;
using seq3 = typename sgl::tuple<int, char, double>::indices_t;
template <size_t I, size_t... Is>
void print(sgl::index_seq_t<I, Is...>) {
  if constexpr (sizeof...(Is) != 0) {
    std::cout << I << std::endl;
    print(sgl::index_seq_t<Is...>{});
  } else {
    std::cout << I << std::endl;
  }
}
int main() {
  print(seq{});
  print(seq3{});
  // sgl::tuple t(1,'k',1.0);
  // print(decltype(t)::indices_t{});
}