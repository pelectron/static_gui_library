//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_FIX_POINT_IMPL_HPP
#define SGL_IMPL_FIX_POINT_IMPL_HPP
#include "sgl/fix_point.hpp"

namespace sgl {

  template <typename T>
  constexpr T mask(size_t msb, size_t lsb) noexcept {
    T val{0};
    for (size_t i = lsb; i <= msb; ++i) {
      val |= (T{1} << i);
    }
    return val;
  }

  static_assert(mask<uint8_t>(7, 0) == 0xFFu);
  static_assert(mask<uint8_t>(3, 0) == 0x0Fu);
  static_assert(mask<uint8_t>(7, 4) == 0xF0u);

  /// @brief get internal value of f correctly sign extended
  template <size_t I, size_t F>
  typename signed_fixed<I, F>::value_type sign_extended_value(signed_fixed<I, F> f) {
    if (f.is_negative()) {
      using T = typename signed_fixed<I, F>::value_type;
      // value or'd together with all ones in the 'guard' bits of the value type
      return f.value() | mask<T>(sizeof(T) * 8 - 1, I + F);
    }
    return f.value();
  }

  template <size_t IntDigits, size_t FracDigits>
  template <size_t I, size_t F>
  constexpr unsigned_fixed<IntDigits, FracDigits>::unsigned_fixed(
      unsigned_fixed<I, F> other) noexcept {
    *this = resize<IntDigits, FracDigits>(other);
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr unsigned_fixed<IntDigits, FracDigits>::unsigned_fixed(
      unsigned_fixed<IntDigits, FracDigits>::value_type value) noexcept
      : value_(value & value_mask) {}

  template <size_t IntDigits, size_t FracDigits>
  constexpr unsigned_fixed<IntDigits, FracDigits>::unsigned_fixed(float value) noexcept {

    const value_type int_part = (static_cast<value_type>(value) & integer_mask) << num_frac_digits;

    const value_type frac_part = static_cast<value_type>((value - static_cast<value_type>(value)) *
                                                         gcem::pow(2.0f, num_frac_digits)) &
                                 fraction_mask;

    value_ = int_part | frac_part;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr unsigned_fixed<IntDigits, FracDigits>::unsigned_fixed(double value) noexcept {

    const value_type int_part =
        ((static_cast<value_type>(value) & integer_mask) << num_frac_digits);

    const value_type frac_part = static_cast<value_type>((value - static_cast<value_type>(value)) *
                                                         gcem::pow(2.0, num_frac_digits)) &
                                 fraction_mask;

    value_ = int_part | frac_part;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr typename unsigned_fixed<IntDigits, FracDigits>::value_type
      unsigned_fixed<IntDigits, FracDigits>::value() const {
    return value_;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr fixpoint_value_type_t<unsigned_fixed<IntDigits, FracDigits>::num_int_digits>
      unsigned_fixed<IntDigits, FracDigits>::integer() const {
    return value_ >> num_frac_digits;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr fixpoint_value_type_t<unsigned_fixed<IntDigits, FracDigits>::num_frac_digits>
      unsigned_fixed<IntDigits, FracDigits>::fraction() const {
    return value_ & fraction_mask;
  }

  template <size_t IntDigits, size_t FracDigits>
  template <size_t I, size_t F>
  constexpr signed_fixed<IntDigits, FracDigits>::signed_fixed(signed_fixed<I, F> other) noexcept {
    *this = resize<IntDigits, FracDigits>(other);
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr signed_fixed<IntDigits, FracDigits>::signed_fixed(
      signed_fixed<IntDigits, FracDigits>::value_type value) noexcept
      : value_(value & value_mask) {}

  template <size_t IntDigits, size_t FracDigits>
  constexpr signed_fixed<IntDigits, FracDigits>::signed_fixed(float value) noexcept {
    if (value < 0.0f) {
      value = -value;
      const value_type int_part = (static_cast<value_type>(value) & integer_mask)
                                  << num_frac_digits;

      const value_type frac_part =
          static_cast<value_type>((value - static_cast<value_type>(value)) *
                                  gcem::pow(2.0f, num_frac_digits)) &
          fraction_mask;
      value_ = int_part | frac_part;
      value_ = static_cast<value_type>((~(value_) + 1)) & value_mask;
    } else {
      const value_type int_part =
          ((static_cast<value_type>(value) & integer_mask) << num_frac_digits);

      const value_type frac_part =
          static_cast<value_type>((value - static_cast<value_type>(value)) *
                                  gcem::pow(2.0f, num_frac_digits)) &
          fraction_mask;

      value_ = int_part | frac_part;
    }
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr signed_fixed<IntDigits, FracDigits>::signed_fixed(double value) noexcept {
    if (value < 0.0) {
      value = -value;
      const value_type int_part = (static_cast<value_type>(value) & integer_mask)
                                  << num_frac_digits;

      const value_type frac_part =
          static_cast<value_type>((value - static_cast<value_type>(value)) *
                                  gcem::pow(2.0, num_frac_digits)) &
          fraction_mask;

      value_ = int_part | frac_part;
      value_ = (~(value_) + 1) & value_mask;
    } else {
      const value_type int_part = (static_cast<value_type>(value) & integer_mask)
                                  << num_frac_digits;

      const value_type frac_part =
          static_cast<value_type>((value - static_cast<value_type>(value)) *
                                  gcem::pow(2.0, num_frac_digits)) &
          fraction_mask;

      value_ = int_part | frac_part;
    }
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr typename signed_fixed<IntDigits, FracDigits>::value_type
      signed_fixed<IntDigits, FracDigits>::value() const noexcept {
    return value_;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr fixpoint_value_type_t<signed_fixed<IntDigits, FracDigits>::num_int_digits>
      signed_fixed<IntDigits, FracDigits>::integer() const {
    return value_ >> num_frac_digits;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr fixpoint_value_type_t<signed_fixed<IntDigits, FracDigits>::num_frac_digits>
      signed_fixed<IntDigits, FracDigits>::fraction() const {
    return value_ & fraction_mask;
  }

  template <size_t IntDigits, size_t FracDigits>
  constexpr bool signed_fixed<IntDigits, FracDigits>::is_negative() const noexcept {
    return (value_ & value_type{1} << (num_digits - 1)) == (value_type{1} << (num_digits - 1));
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2) noexcept {
    using result_type = unsigned_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>;
    using value_type = typename result_type::value_type;
    if constexpr (F1 > F2) {
      return static_cast<value_type>(f1.value()) +
             (static_cast<value_type>(f2.value()) << (F1 - F2));
    } else if constexpr (F2 > F1) {
      return static_cast<value_type>(f2.value()) +
             (static_cast<value_type>(f1.value()) << (F2 - F1));
    } else {
      return static_cast<value_type>(static_cast<value_type>(f1.value()) +
                                     static_cast<value_type>(f2.value()));
    }
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<gcem::max(I1, I2), gcem::max(F1, F2)>
      operator-(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2) noexcept {
    using result_type = unsigned_fixed<gcem::max(I1, I2), gcem::max(F1, F2)>;
    using value_type = typename result_type::value_type;
    if constexpr (F1 > F2) {
      return static_cast<value_type>(f1.value()) -
             (static_cast<value_type>(f2.value()) << (F1 - F2));
    } else if constexpr (F2 > F1) {
      return (static_cast<value_type>(f1.value()) << (F2 - F1)) -
             static_cast<value_type>(f2.value());
    } else {
      return static_cast<value_type>(static_cast<value_type>(f1.value()) -
                                     static_cast<value_type>(f2.value()));
    }
  }

  template <size_t I, size_t F>
  constexpr unsigned_fixed<I, F> operator+(const unsigned_fixed<I, F>& value) noexcept {
    return value;
  }

  template <size_t I, size_t F>
  constexpr signed_fixed<I + 1, F> operator-(const unsigned_fixed<I, F>& value) noexcept {
    using T = typename signed_fixed<I + 1, F>::value_type;
    return static_cast<T>((~static_cast<T>(value.value()) + 1u) &
                          mask<T, signed_fixed<I + 1, F>::num_digits>());
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1 + I2, F1 + F2> operator*(const unsigned_fixed<I1, F1>& f1,
                                                       const unsigned_fixed<I2, F2>& f2) noexcept {
    using result_type = unsigned_fixed<I1 + I2, F1 + F2>;
    using value_type = typename result_type::value_type;
    return static_cast<value_type>(f1.value()) * static_cast<value_type>(f2.value());
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1 + F2, I2 + F1> operator/(const unsigned_fixed<I1, F1>& f1,
                                                       const unsigned_fixed<I2, F2>& f2) {
    // v1= f1.value()
    // v2 = f2.value()
    // f1 / f2 = v1*2^-F1/(v2*2^-F2)= v1/v2 * 2^-(F1 - F2) = x * 2^-(F1 - F2)
    // now x needs to be converted to form: v3* 2^-(F1+I2), which is a left shift by (F1+I2) -
    // (F1-F2) = I2+F2
    using result_type = unsigned_fixed<I1 + F2, I2 + F1>;
    using value_type = typename result_type::value_type;
    return static_cast<value_type>((static_cast<value_type>(f1.value()) << (I2 + F2)) /
                                   static_cast<value_type>(f2.value()));
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1, F1> resize(unsigned_fixed<I2, F2> value) noexcept {
    if constexpr (F1 > F2) {
      return static_cast<typename unsigned_fixed<I1, F1>::value_type>(
          static_cast<typename unsigned_fixed<I1, F1>::value_type>(value.value()) << (F1 - F2));
    } else {
      return static_cast<typename unsigned_fixed<I1, F1>::value_type>(
          static_cast<typename unsigned_fixed<I1, F1>::value_type>(value.value()) >> (F2 - F1));
    }
  }

  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator+(const signed_fixed<I, F>& value) noexcept {
    return value;
  }

  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator-(const signed_fixed<I, F>& value) noexcept {
    using T = typename signed_fixed<I, F>::value_type;
    return static_cast<T>((~static_cast<T>(value.value()) + 1) &
                          mask<T, signed_fixed<I, F>::num_digits>());
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const signed_fixed<I1, F1>& f1, const signed_fixed<I2, F2>& f2) noexcept {
    using result_type = signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>;
    using value_type = typename result_type::value_type;
    value_type v1 = sign_extended_value(resize<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>(f1));
    value_type v2 = sign_extended_value(resize<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>(f2));
    return v1 + v2;
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator-(signed_fixed<I1, F1> f1, signed_fixed<I2, F2> f2) noexcept {
    using result_type = signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>;
    using value_type = typename result_type::value_type;
    value_type v1 = sign_extended_value(sgl::resize<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>(f1));
    value_type v2 = sign_extended_value(sgl::resize<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>(f2));
    return v1 - v2;
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + I2 - 1, F1 + F2> operator*(signed_fixed<I1, F1> f1,
                                                         signed_fixed<I2, F2> f2) noexcept {
    using result_type = signed_fixed<I1 + I2 - 1, F1 + F2>;
    using value_type = typename result_type::value_type;
    const bool neg_result = f1.is_negative() xor f2.is_negative();
    if (f1.is_negative()) {
      f1 = -f1;
    }
    if (f2.is_negative()) {
      f2 = -f2;
    }
    if (neg_result) {
      return -result_type{static_cast<value_type>(f1.value()) *
                          static_cast<value_type>(f2.value())};
    }
    return static_cast<value_type>(f1.value()) * static_cast<value_type>(f2.value());
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + F2, I2 + F1> operator/(signed_fixed<I1, F1> f1,
                                                     signed_fixed<I2, F2> f2) {
    using result_type = signed_fixed<I1 + F2, I2 + F1>;
    using value_type = typename result_type::value_type;
    const bool neg_result = f1.is_negative() xor f2.is_negative();
    if (f1.is_negative()) {
      f1 = -f1;
    }
    if (f2.is_negative()) {
      f2 = -f2;
    }
    if (neg_result) {
      return -result_type{
          static_cast<value_type>((static_cast<value_type>(f1.value()) << (I2 + F2)) /
                                  static_cast<value_type>(f2.value()))};
    }
    return static_cast<value_type>((static_cast<value_type>(f1.value()) << (I2 + F2)) /
                                   static_cast<value_type>(f2.value()));
  }

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1, F1> resize(signed_fixed<I2, F2> value) noexcept {
    using value_type = typename signed_fixed<I1, F1>::value_type;
    constexpr auto sign_extend_mask = mask<value_type>(sizeof(value_type) * 8 - 1, I2 + F1);
    if constexpr (F1 > F2) {
      const value_type val =
          static_cast<value_type>(static_cast<value_type>(value.value()) << (F1 - F2));
      if (value.is_negative()) {
        return static_cast<value_type>(sign_extend_mask | val);
      }
      return val;

    } else {
      const auto val = static_cast<value_type>(value.value() >> (F2 - F1));
      if (value.is_negative()) {
        return static_cast<value_type>(sign_extend_mask | val);
      }
      return val;
    }
  }

  template <size_t I, size_t F>
  constexpr float to_float(sgl::unsigned_fixed<I, F> v) noexcept {
    return v.integer() + v.fraction() / (gcem::pow(2.0f, F));
  }

  template <size_t I, size_t F>
  constexpr float to_float(sgl::signed_fixed<I, F> v) noexcept {
    if (v.is_negative()) {
      v = -v;
      return -(v.integer() + v.fraction() / gcem::pow(2.0f, F));
    }
    return v.integer() + v.fraction() / gcem::pow(2.0f, F);
  }

  template <size_t I, size_t F>
  constexpr double to_double(sgl::unsigned_fixed<I, F> v) noexcept {
    return v.integer() + v.fraction() / (gcem::pow(2.0, F));
  }

  template <size_t I, size_t F>
  constexpr double to_double(sgl::signed_fixed<I, F> v) noexcept {
    if (v.is_negative()) {
      v = -v;
      return -(v.integer() + v.fraction() / gcem::pow(2.0, F));
    }
    return v.integer() + v.fraction() / gcem::pow(2.0, F);
  }

  template <size_t I, size_t F>
  constexpr sgl::signed_fixed<I, F> to_signed(sgl::unsigned_fixed<I, F> v) noexcept {
    return v.value();
  }

  template <size_t I, size_t F>
  constexpr sgl::unsigned_fixed<I, F> to_unsigned(sgl::signed_fixed<I, F> v) noexcept {
    return v.value();
  }
} // namespace sgl

#endif /* SGL_IMPL_FIX_POINT_IMPL_HPP */
