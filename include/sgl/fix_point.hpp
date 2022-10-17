#ifndef SGL_FIX_POINT_HPP
#define SGL_FIX_POINT_HPP
#include "gcem.hpp"

#include <cassert>
#include <type_traits>

namespace sgl {
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
   * \brief unsigned fix point type.
   * \tparam IntDigits
   * \tparam FracDigits
   */
  template <size_t IntDigits, size_t FracDigits>
  class unsigned_fixed {
  public:
    static constexpr size_t num_frac_digits = FracDigits;
    static constexpr size_t num_int_digits = IntDigits;
    static constexpr size_t num_digits = num_int_digits + num_frac_digits;

    using value_type = fixpoint_value_type_t<num_digits>;

    static_assert(
        not std::is_same_v<void, value_type>,
        "unsigned_fixed digits error: maximum supported number of digits is 64. Either choose less "
        "integer or fractional digits.");

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

  /// @brief signed fix point type
  /// @tparam IntDigits
  /// @tparam FracDigits
  template <size_t IntDigits, size_t FracDigits>
  class signed_fixed {
  public:
    static constexpr size_t num_frac_digits = FracDigits;
    static constexpr size_t num_int_digits = IntDigits;
    static constexpr size_t num_digits = num_int_digits + num_frac_digits;

    using value_type = std::conditional_t<
        num_digits <= 8,
        uint8_t,
        std::conditional_t<
            num_digits <= 16,
            uint16_t,
            std::conditional_t<num_digits <= 32,
                               uint32_t,
                               std::conditional_t<num_digits <= 64, uint64_t, void>>>>;

    static_assert(
        not std::is_same_v<void, value_type>,
        "signed_fixed digits error: maximum supported number of digits is 64. Either choose less "
        "integer or fractional digits.");
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
    value_type value_;
  };

  // unsigned_fixed operators
  template <size_t I, size_t F>
  constexpr unsigned_fixed<I, F> operator+(unsigned_fixed<I, F> value) noexcept;

  template <size_t I, size_t F>
  constexpr signed_fixed<I + 1, F> operator-(unsigned_fixed<I, F> value) noexcept;

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
  constexpr auto operator/(const unsigned_fixed<I1, F1>& f1, const unsigned_fixed<I2, F2>& f2);

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr unsigned_fixed<I1, F1> resize(unsigned_fixed<I2, F2> value) noexcept;

  // signed_fixed operators
  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator+(signed_fixed<I, F> value) noexcept;

  template <size_t I, size_t F>
  constexpr signed_fixed<I, F> operator-(signed_fixed<I, F> value) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator+(const signed_fixed<I1, F1>& f1, const signed_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<gcem::max(I1, I2) + 1, gcem::max(F1, F2)>
      operator-(const signed_fixed<I1, F1>& f1, const signed_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr signed_fixed<I1 + I2 - 1, F1 + F2> operator*(const signed_fixed<I1, F1>& f1,
                                                         const signed_fixed<I2, F2>& f2) noexcept;

  template <size_t I1, size_t F1, size_t I2, size_t F2>
  constexpr auto operator/(const signed_fixed<I1, F1>& f1, const signed_fixed<I2, F2>& f2);

  /// @brief convert signed_fixed to unsigned_fixed. This function just copies the internal
  /// representation.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr sgl::unsigned_fixed<I, F> to_unsigned(sgl::signed_fixed<I, F> v) noexcept {
    return v.value();
  }

  /// @brief convert unsigned_fixed to signed_fixed. This function just copies the internal
  /// representation.
  /// @tparam I number of integer digits
  /// @tparam F number of fractional digits
  /// @param v value to convert
  template <size_t I, size_t F>
  constexpr sgl::signed_fixed<I, F> to_signed(sgl::unsigned_fixed<I, F> v) noexcept {
    return v.value();
  }

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
