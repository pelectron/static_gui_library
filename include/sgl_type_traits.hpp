/**
 * \file sgl_traits.hpp
 * \author Pelé Constam (you\domain.com)
 * \brief This file implements some of the type traits found in the <type_traits> header.
 * \version 0.1
 * \date 2022-06-14
 *
 * \copyright Copyright (c) 2022
 *
 */
#ifndef SGL_TRAITS_HPP
#define SGL_TRAITS_HPP
#include <cfloat>
#include <climits>
#include <cstdint>
#include <type_traits>
#define SGL_CONSTRAIN(predicate, /*args*/...) std::enable_if_t<predicate<__VA_ARGS__>, bool> = true
#define SGL_CONSTRAIN_FOR_ALL(predicate, param_pack) \
  std::enable_if_t<(predicate<param_pack> && ...), bool> = true
namespace sgl {} // namespace sgl
#endif