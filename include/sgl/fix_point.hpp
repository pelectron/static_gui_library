/**
 * @file fix_point.hpp
 * @author Pelé Constam (pelectron1602@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-10-23
 * @copyright Boost Software License
 */
#ifndef SGL_FIX_POINT_HPP
#define SGL_FIX_POINT_HPP
#include <cstdint>
#include <gcem.hpp>
#include <type_traits>

namespace sgl {
  /// @cond
  template <typename T, size_t num_digits>
  constexpr T mask() noexcept {
    T val{0};
    for (size_t i = 0; i < num_digits; ++i) {
      val |= (T{1} << i);
    }
    return val;
  }

  static_assert(mask<uint8_t, 3>() == 0b111);
  static_assert(mask<uint8_t, 4>() == 0b1111);
  static_assert(mask<uint64_t, 64>() == 0xFFFFFFFFFFFFFFFFu);
  /// @endcond

  /// type trait to determine the unsigned type needed to store at least num_digits bits.
  template <size_t num_digits>
  using fixpoint_value_type_t = std::conditional_t<
      num_digits <= 8,
      uint8_t,
      std::conditional_t<num_digits <= 16,
                         uint16_t,
                         std::conditional_t<num_digits <= 32,
                                            uint32_t,
                                            std::conditional_t<num_digits <= 64, uint64_t, void>>>>;

  /**
    Unsigned fix point type with basic math and formatting support.
    The operations this type supports are the follwoing:
    - conversion to and from float and double
    - conversion to and from bit pattern
    - addition, subtraction, multiplication and division only using integer math.
    - formatting with sgl::to_chars(). See "sgl/format.hpp".
    @tparam IntDigits number of integer digits
    @tparam FracDigits number of fractional digits
   */
  template <size_t IntDigits, size_t FracDigits>
  class unsigned_fixed {
  public:
    /// smallest unsigned integer type with required bit width
    using value_type = fixpoint_value_type_t<FracDigits + IntDigits>;

    static_assert(
        (FracDigits + IntDigits) <= 64,
        "unsigned_fixed digits error: maximum supported number of digits is 64. Either choose less "
        "integer or fractional bits.");

    /// number of fractional bits
    static constexpr size_t num_frac_digits = FracDigits;
    /// number of integer bits
    static constexpr size_t num_int_digits = IntDigits;
    /// total number of bits
    static constexpr size_t num_digits = num_int_digits + num_frac_digits;
    /// mask for fractional part
    static constexpr value_type fraction_mask = mask<value_type, num_frac_digits>();
    /// mask for integer part
    static constexpr value_type integer_mask = mask<value_type, num_int_digits>();
    /// mask for whole value
    static constexpr value_type value_mask = mask<value_type, num_digits>();

    constexpr unsigned_fixed() = default;
    constexpr unsigned_fixed(const unsigned_fixed&) = default;
    constexpr unsigned_fixed(unsigned_fixed&&) = default;
    constexpr unsigned_fixed& operator=(const unsigned_fixed&) = default;
    constexpr unsigned_fixed& operator=(unsigned_fixed&&) = default;

    template <size_t I, size_t F>
    constexpr unsigned_fixed(unsigned_fixed<I, F> other) noexcept;

    /// construct from binary value.
    constexpr unsigned_fixed(value_type value) noexcept;

    /// construct from floating point.
    /// @{
    constexpr explicit unsigned_fixed(float value) noexcept;
    constexpr explicit unsigned_fixed(double value) noexcept;
    /// @}

    /// access the raw value
    /// @return smallest unsigned integer type with at least num_digits bits
    [[nodiscard]] constexpr value_type value() const;

    /// get the raw integer bits
    /// @return smallest unsigned integer with at least num_int_digits bits
    [[nodiscard]] constexpr fixpoint_value_type_t<IntDigits> integer() const;

    /// get the raw fraction bits
    /// @return smallest unsigned integer with at least num_frac_digits bits
    [[nodiscard]] constexpr fixpoint_value_type_t<FracDigits> fraction() const;

  private:
    value_type value_{0};
  };

  /**
    Signed fix point type with basic math and formatting support. Holds its value in two's
    complement, unused upper bits are 0 (not sign extended).
 
    The operations this type supports are the follwoing:
    - conversion to and from float and double
    - conversion to and from bit pattern
    - addition, subtraction, multiplication and division only using integer math.
    - formatting with sgl::to_chars(). See "sgl/format.hpp".
    @tparam IntDigits number of integer digits
    @tparam FracDigits number of fractional digits
   */
  template <size_t IntDigits, size_t FracDigits>
  class signed_fixed {
  public:
    /// smallest unsigned integer type with required bit width
    using value_type = fixpoint_value_type_t<FracDigits + IntDigits>;

    static_assert(
        (FracDigits + IntDigits) <= 64,
        "signed_fixed digits error: maximum supported number of digits is 64. Either choose less "
        "integer or fractional bits.");

    /// number of fractional bits
    static constexpr size_t num_frac_digits = FracDigits;
    /// number of integer bits
    static constexpr size_t num_int_digits = IntDigits;
    /// total number of bits
    static constexpr size_t num_digits = num_int_digits + num_frac_digits;
    /// mask for fractional part
    static constexpr value_type fraction_mask = mask<value_type, num_frac_digits>();
    /// mask for integer part
    static constexpr value_type integer_mask = mask<value_type, num_int_digits>();
    /// mask for whole value
    static constexpr value_type value_mask = mask<value_type, num_digits>();

    constexpr signed_fixed() = default;
    constexpr signed_fixed(const signed_fixed&) = default;
    constexpr signed_fixed(signed_fixed&&) = default;
    constexpr signed_fixed& operator=(const signed_fixed&) = default;
    constexpr signed_fixed& operator=(signed_fixed&&) = default;

    template <size_t I, size_t F>
    constexpr signed_fixed(signed_fixed<I, F> other) noexcept;
    /// construct from binary value.
    constexpr signed_fixed(value_type value) noexcept;

    /// construct from floating point.
    /// @{
    constexpr explicit signed_fixed(float value) noexcept;
    constexpr explicit signed_fixed(double value) noexcept;
    /// @}

    /// access the fixpoint's raw value (stored in two's complement)
    /// @return smallest unsigned integer type with at least num_digits bits
    [[nodiscard]] constexpr value_type value() const noexcept;

    /// get the raw integer bits
    /// @return smallest unsigned integer with at least num_int_digits bits
    [[nodiscard]] constexpr fixpoint_value_type_t<IntDigits> integer() const;

    /// get the raw fraction bits
    /// @return smallest unsigned integer with at least num_frac_digits bits
    [[nodiscard]] constexpr fixpoint_value_type_t<FracDigits> fraction() const;

    /// check if this is negative.
    /// @return true if this is smaller than 0.
    [[nodiscard]] constexpr bool is_negative() const noexcept;

  private:
    value_type value_; ///< value in two's complement, regardless of architecture.
  };

  /**
    unary plus operator
    @tparam I number of integer bits
    @tparam F number of fractional bits
    @param value
    @return unsigned_fixed<I, F>
   */
  template <size_t I, size_t F>
  constexpr unsigned_fixed<I, F> operator+(const unsigned_fixed<I, F>& value) noexcept;

  /**
    unary minus operator
    @tparam I number of integer bits
    @tparam F number of fractional bits
    @param value value to invert
    @return signed_fixed<I + 1, F>
   */
  template <size_t I, size_t F>
  constexpr signed_fixed<I + 1, F> operator-(const unsigned_fixed<I, F>& value) noexcept;

  /**
    unsigned fixed point addition
    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 first summand
    @param f2 second summand
    @return f1 + f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2) noexcept;

  /**
    unsigned fixed point subtraction
    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 minuend
    @param f2 subtrahend
    @return f1 - f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<gcem::max(I1, I2), gcem::max(F1, F2)>
      operator-(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2) noexcept;

  /**
    unsigned fixed point multiplication
    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 first multiplicand
    @param f2 second multiplicand
    @return f1 * f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1 + I2, F1 + F2> operator*(const unsigned_fixed<I1, F1>& f1,
                                                       const unsigned_fixed<I2, F2>& f2) noexcept;

  /**
    unsigned fixed point division
    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 numerator
    @param f2 denominator
    @return f1 / f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1 + F2, I2 + F1> operator/(const unsigned_fixed<I1, F1>& f1,
                                                       const unsigned_fixed<I2, F2>& f2);

  /**
    resize unsigned_fixed from <I2,F2> to <I1,F1>
    @tparam I1 number of integer bits the return value should have
    @tparam F1 number of fractional bits the return value should have
    @tparam I2 number of integer bits of value
    @tparam F2 number of fractional bits of value
    @param value value to resize
    @return unsigned_fixed<I1, F1>
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1, F1> resize(unsigned_fixed<I2, F2> value) noexcept;

  /**
    unary plus operator
    @tparam I number of integer bits
    @tparam F number of fractional bits
    @param value
    @return signed_fixed<I, F>
   */
  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator+(const signed_fixed<I, F>& value) noexcept;

  /**
    unary minus operator
    @tparam I number of integer bits
    @tparam F number of fractional bits
    @param value value to invert
    @return signed_fixed<I, F>
   */
  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator-(const signed_fixed<I, F>& value) noexcept;

  /**
    signed fixed point addition
    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 first summand
    @param f2 second summand
    @return f1 + f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const signed_fixed<I1, F1>& f1, const signed_fixed<I2, F2>& f2) noexcept;

  /**
    signed fixed point subtraction
    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 minuend
    @param f2 subtrahend
    @return f1 - f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator-(signed_fixed<I1, F1> f1, signed_fixed<I2, F2> f2) noexcept;

  /**
    signed fixed point multiplication

    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 first multiplicand
    @param f2 second multiplicand
    @return f1 * f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + I2 - 1, F1 + F2> operator*(signed_fixed<I1, F1> f1,
                                                         signed_fixed<I2, F2> f2) noexcept;

  /**
    signed fixed point division

    @tparam I1 number of integer bits of f1
    @tparam F1 number of fractional bits of f1
    @tparam I2 number of integer bits of f2
    @tparam F2 number of fractional bits of f2
    @param f1 numerator
    @param f2 denominator
    @return f1 / f2
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + F2, I2 + F1> operator/(signed_fixed<I1, F1> f1,
                                                     signed_fixed<I2, F2> f2);

  /**
    resize signed_fixed from <I2,F2> to <I1,F1>
    @tparam I1 number of integer bits the return value should have
    @tparam F1 number of fractional bits the return value should have
    @tparam I2 number of integer bits of value
    @tparam F2 number of fractional bits of value
    @param value value to resize
    @return signed_fixed<I1, F1>
   */
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1, F1> resize(signed_fixed<I2, F2> value) noexcept;

  /**
    convert signed_fixed to unsigned_fixed of the same bit width. This function just copies
    the internal representation.
    @tparam I number of integer digits
    @tparam F number of fractional digits
    @param v value to convert
    @return sgl::unsigned_fixed<I, F>
   */
  template <size_t I, size_t F>
  constexpr sgl::unsigned_fixed<I, F> to_unsigned(sgl::signed_fixed<I, F> v) noexcept;

  /**
    convert unsigned_fixed to signed_fixed of same bit width. This function just copies the
    internal representation.
    @tparam I number of integer digits
    @tparam F number of fractional digits
    @param v value to convert
    @return sgl::signed_fixed<I, F>
   */
  template <size_t I, size_t F>
  constexpr sgl::signed_fixed<I, F> to_signed(sgl::unsigned_fixed<I, F> v) noexcept;

  /**
    convert unsigned_fixed to float.
    @tparam I number of integer digits
    @tparam F number of fractional digits
    @param v value to convert
    @return float
   */
  template <size_t I, size_t F>
  constexpr float to_float(sgl::unsigned_fixed<I, F> v) noexcept;

  /**
    convert signed_fixed to float.
    @tparam I number of integer digits
    @tparam F number of fractional digits
    @param v value to convert
    @return float
   */
  template <size_t I, size_t F>
  constexpr float to_float(sgl::signed_fixed<I, F> v) noexcept;

  /**
    convert unsigned_fixed to double.
    @tparam I number of integer digits
    @tparam F number of fractional digits
    @param v value to convert
    @return double
   */
  template <size_t I, size_t F>
  constexpr double to_double(sgl::unsigned_fixed<I, F> v) noexcept;

  /**
    convert signed_fixed to double.
    @tparam I number of integer digits
    @tparam F number of fractional digits
    @param v value to convert
    @return double
   */
  template <size_t I, size_t F>
  constexpr double to_double(sgl::unsigned_fixed<I, F> v) noexcept;
} // namespace sgl

#include "sgl/impl/fix_point_impl.hpp"
#endif /* SGL_FIX_POINT_HPP */
