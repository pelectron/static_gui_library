//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_FIX_POINT_HPP
#define SGL_FIX_POINT_HPP
#include "gcem.hpp"

#include <cassert>
#include <type_traits>

namespace sgl {
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

  template <size_t num_digits>
  using fixpoint_value_type_t = std::conditional_t<
      num_digits <= 8,
      uint8_t,
      std::conditional_t<num_digits <= 16,
                         uint16_t,
                         std::conditional_t<num_digits <= 32,
                                            uint32_t,
                                            std::conditional_t<num_digits <= 64, uint64_t, void>>>>;
  static_assert(std::is_same_v<uint64_t, fixpoint_value_type_t<64>>);
  static_assert(std::is_same_v<uint32_t, fixpoint_value_type_t<32>>);

  /**
   * \brief unsigned fix point type.
   * \tparam IntDigits number of integer digits
   * \tparam FracDigits number of fractional digits
   */
  template <size_t IntDigits, size_t FracDigits>
  class unsigned_fixed {
  public:
    using value_type = fixpoint_value_type_t<FracDigits + IntDigits>;

    static_assert(
        (FracDigits + IntDigits) <= 64,
        "unsigned_fixed digits error: maximum supported number of digits is 64. Either choose less "
        "integer or fractional bits.");

    /// @brief number of fractional bits
    static constexpr size_t num_frac_digits = FracDigits;
    /// @brief number of integer bits
    static constexpr size_t num_int_digits = IntDigits;
    /// @brief total number of bits
    static constexpr size_t num_digits = num_int_digits + num_frac_digits;
    /// @brief mask for fractional part
    static constexpr value_type fraction_mask = mask<value_type, num_frac_digits>();
    /// @brief mask for integer part
    static constexpr value_type integer_mask = mask<value_type, num_int_digits>();
    /// @brief mask for whole value
    static constexpr value_type value_mask = mask<value_type, num_digits>();

    constexpr unsigned_fixed() = default;
    constexpr unsigned_fixed(const unsigned_fixed&) = default;
    constexpr unsigned_fixed(unsigned_fixed&&) = default;
    constexpr unsigned_fixed& operator=(const unsigned_fixed&) = default;
    constexpr unsigned_fixed& operator=(unsigned_fixed&&) = default;

    template <size_t I, size_t F>
    constexpr unsigned_fixed(unsigned_fixed<I, F> other) noexcept;
    constexpr unsigned_fixed(value_type value) noexcept;

    explicit unsigned_fixed(float value) noexcept;

    explicit unsigned_fixed(double value) noexcept;

    /// access the fixpoint's raw value
    [[nodiscard]] constexpr value_type value() const;

    /// get the raw integer portion
    [[nodiscard]] constexpr fixpoint_value_type_t<num_int_digits> integer() const;

    /// get the raw fraction portion
    [[nodiscard]] constexpr fixpoint_value_type_t<num_frac_digits> fraction() const;

  private:
    value_type value_{0};
  };

  /// @brief signed fix point type. holds its value in two's complement.
  /// @tparam IntDigits number of integer digits
  /// @tparam FracDigits number of fractional digits
  template <size_t IntDigits, size_t FracDigits>
  class signed_fixed {
  public:
    using value_type = fixpoint_value_type_t<FracDigits + IntDigits>;

    static_assert(
        (FracDigits + IntDigits) <= 64,
        "signed_fixed digits error: maximum supported number of digits is 64. Either choose less "
        "integer or fractional bits.");

    /// @brief number of fractional bits
    static constexpr size_t num_frac_digits = FracDigits;
    /// @brief number of integer bits
    static constexpr size_t num_int_digits = IntDigits;
    /// @brief total number of bits
    static constexpr size_t num_digits = num_int_digits + num_frac_digits;
    /// @brief mask for fractional part
    static constexpr value_type fraction_mask = mask<value_type, num_frac_digits>();
    /// @brief mask for integer part
    static constexpr value_type integer_mask = mask<value_type, num_int_digits>();
    /// @brief mask for whole value
    static constexpr value_type value_mask = mask<value_type, num_digits>();

    constexpr signed_fixed() = default;
    constexpr signed_fixed(const signed_fixed&) = default;
    constexpr signed_fixed(signed_fixed&&) = default;
    constexpr signed_fixed& operator=(const signed_fixed&) = default;
    constexpr signed_fixed& operator=(signed_fixed&&) = default;

    template <size_t I, size_t F>
    constexpr signed_fixed(signed_fixed<I, F> other) noexcept;
    constexpr signed_fixed(value_type value) noexcept;
    explicit signed_fixed(float value) noexcept;
    explicit signed_fixed(double value) noexcept;

    /// access the fixpoint's raw value (stored in two's complement)
    [[nodiscard]] constexpr value_type value() const noexcept;

    /// get the raw integer portion
    [[nodiscard]] constexpr fixpoint_value_type_t<num_int_digits> integer() const;

    /// get the raw fraction portion
    [[nodiscard]] constexpr fixpoint_value_type_t<num_frac_digits> fraction() const;

    /// return true if this is smaller than 0.
    [[nodiscard]] constexpr bool is_negative() const noexcept;

  private:
    value_type value_; ///< value in two's complement, regardless of architecture.
  };

  // unsigned_fixed operators
  template <size_t I, size_t F>
  constexpr unsigned_fixed<I, F> operator+(const unsigned_fixed<I, F>& value) noexcept;

  template <size_t I, size_t F>
  constexpr signed_fixed<I + 1, F> operator-(const unsigned_fixed<I, F>& value) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<gcem::max(I1, I2), gcem::max(F1, F2)>
      operator-(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1 + I2, F1 + F2> operator*(const unsigned_fixed<I1, F1>& f1,
                                                       const unsigned_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1 + F2, I2 + F1> operator/(const unsigned_fixed<I1, F1>& f1,
                                                       const unsigned_fixed<I2, F2>& f2);

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1, F1> resize(unsigned_fixed<I2, F2> value) noexcept;

  // signed_fixed operators
  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator+(const signed_fixed<I, F>& value) noexcept;

  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator-(const signed_fixed<I, F>& value) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const signed_fixed<I1, F1>& f1, const signed_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator-(signed_fixed<I1, F1> f1, signed_fixed<I2, F2> f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + I2 - 1, F1 + F2> operator*(signed_fixed<I1, F1> f1,
                                                         signed_fixed<I2, F2> f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + F2, I2 + F1> operator/(signed_fixed<I1, F1> f1,
                                                     signed_fixed<I2, F2> f2);

  /// @brief resize value from <I2,F2> to <I1,F1>
  /// @tparam I1
  /// @tparam F1
  /// @tparam I2
  /// @tparam F2
  /// @param value
  /// @return
  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1, F1> resize(signed_fixed<I2, F2> value) noexcept;

  /// @brief convert signed_fixed to unsigned_fixed of the same bit width. This function just copies
  /// the internal representation.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr sgl::unsigned_fixed<I, F> to_unsigned(sgl::signed_fixed<I, F> v) noexcept;

  /// @brief convert unsigned_fixed to signed_fixed of same bit width. This function just copies the
  /// internal representation.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr sgl::signed_fixed<I, F> to_signed(sgl::unsigned_fixed<I, F> v) noexcept;

  /// @brief convert unsigned_Fixed to float.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr float to_float(sgl::unsigned_fixed<I, F> v) noexcept;

  /// @brief convert signed_Fixed to float.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr float to_float(sgl::signed_fixed<I, F> v) noexcept;

  /// @brief convert unsigned_Fixed to double.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr double to_double(sgl::unsigned_fixed<I, F> v) noexcept;

  /**
   * @brief convert signed_Fixed to double.
   * @tparam I number of integer digits
   * @tparam F number of fractional digits
   * @param v value to convert
   */
  template <size_t I, size_t F>
  constexpr double to_double(sgl::unsigned_fixed<I, F> v) noexcept;
} // namespace sgl
#include "sgl/impl/fix_point_impl.hpp"
#endif /* SGL_FIX_POINT_HPP */
