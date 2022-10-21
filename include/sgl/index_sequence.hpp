//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_INDEX_SEQUENCE_HPP
#define SGL_INDEX_SEQUENCE_HPP

namespace sgl {
  ///\cond
  template <size_t... I>
  struct index_seq_t {};

  template <size_t I, typename Seq>
  struct push_back_seq;

  template <size_t I, size_t... Is>
  struct push_back_seq<I, index_seq_t<Is...>> {
    using type = index_seq_t<Is..., I>;
  };
  template <size_t I, typename Seq>
  struct push_front_seq;

  template <size_t I, size_t... Is>
  struct push_front_seq<I, index_seq_t<Is...>> {
    using type = index_seq_t<I, Is...>;
  };

  template <size_t I>
  struct make_index_seq : make_index_seq<I - 1> {
    using type = typename push_back_seq<I, typename make_index_seq<I - 1>::type>::type;
  };

  template <>
  struct make_index_seq<0> {
    using type = index_seq_t<0>;
  };

  template <size_t I>
  using make_index_seq_t = typename make_index_seq<I>::type;
  template <typename... Ts>
  using index_sequence_for = typename make_index_seq<sizeof...(Ts) - 1>::type;

  /// \endcond
} // namespace sgl
#endif /* SGL_INDEX_SEQUENCE_HPP */
