#ifndef ENCODER_HPP
#define ENCODER_HPP
#include <cstdint>
enum class state { reset, left, right, error };
struct encoder {
  static constexpr uint8_t left_states[]{0b0010, 0b1011, 0b1101, 0b0100};

  static constexpr uint8_t right_states[]{0b0001, 0b0111, 0b1110, 0b1000};

  static constexpr uint8_t error_states[]{0b0001, 0b0111, 0b1110, 0b1000};

  state tick(bool a, bool b) {
    buffer = static_cast<uint8_t>(buffer << 2) | static_cast<uint8_t>(a << 1) |
             static_cast<uint8_t>(b);
    const uint8_t status = buffer & 0x0FU;
    for (size_t i = 0; i < 4; ++i) {
      if (left_states[i] == status)
        return state = state::left;
      if (right_states[i] == status)
        return state = state::right;
      if (error_states[i] == status)
        return state = state::error;
    }
    return state;
  }

  uint8_t buffer{0};
  state   state{state::reset};
};
#endif